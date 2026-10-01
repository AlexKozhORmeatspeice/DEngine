#include "dpch.h"
#include "PhysicSystem.h"

#include "DEngine/Scene/Components.h"


namespace DEngine
{
    // ============================================================
    //  Solver parameters
    // ============================================================
    static constexpr float kRestitutionThreshold = 1.0f;
    static constexpr float kSleepLinearThreshold = 0.02f;
    static constexpr float kSleepAngularThreshold = 0.01f;
    static constexpr float kDefaultLinearDamping = 0.05f;
    static constexpr float kDefaultAngularDamping = 0.2f;    // free-flight damping
    static constexpr float kContactAngularDamping = 1.0f;    // weak extra damping while touching something
    static constexpr int   kSolverIterations = 8;
    static constexpr float kDefaultFriction = 0.4f;
    static constexpr float kPenetrationSlop = 0.001f;
    static constexpr float kPositionalPercent = 0.5f;

    static constexpr int kSingleContact = -2; // contact is a single point (sphere tests)

    struct OBB
    {
        glm::vec3 center;
        glm::vec3 halfExtents;
        glm::mat3 rotation;
    };

    OBB MakeOBB(const TransformComponent& transform, const ColliderComponent& collider)
    {
        OBB o;
        o.center = transform.GetPosition() + transform.GetRotation() * collider.offset;
        o.halfExtents = collider.size * 0.5f;
        o.rotation = glm::mat3_cast(transform.GetRotation());
        return o;
    }

    struct CollisionResult
    {
        bool hit = false;
        float penetration = 0.0f;
        glm::vec3 normal;
        glm::vec3 contactPoint = glm::vec3(0.0f);
        int axisIndex = -1; // 0-2: face of A, 3-5: face of B, 6+: edge-edge
    };

    static float ProjectRadius(const OBB& o, const glm::vec3& L)
    {
        return std::abs(glm::dot(o.rotation[0], L)) * o.halfExtents.x +
            std::abs(glm::dot(o.rotation[1], L)) * o.halfExtents.y +
            std::abs(glm::dot(o.rotation[2], L)) * o.halfExtents.z;
    }

    CollisionResult TestOBB(const OBB& A, const OBB& B)
    {
        CollisionResult result;
        glm::vec3 d = B.center - A.center;

        glm::vec3 axes[15];
        int n = 0;
        for (int i = 0; i < 3; ++i) axes[n++] = A.rotation[i];
        for (int i = 0; i < 3; ++i) axes[n++] = B.rotation[i];
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                glm::vec3 cross = glm::cross(A.rotation[i], B.rotation[j]);
                if (glm::dot(cross, cross) > 1e-8f)
                    axes[n++] = glm::normalize(cross);
            }
        }

        float minPenetration = FLT_MAX;
        glm::vec3 minAxis(0.0f);
        int minIndex = -1;

        for (int i = 0; i < n; ++i)
        {
            const glm::vec3& L = axes[i];
            float rA = ProjectRadius(A, L);
            float rB = ProjectRadius(B, L);
            float dist = std::abs(glm::dot(d, L));
            float overlap = rA + rB - dist;

            if (overlap <= 0.0f)
                return result;

            // Edge-edge axes must win clearly, otherwise the axis flickers between
            // face and edge axes and the contact manifold becomes unstable.
            const bool better = (i < 6)
                ? (overlap < minPenetration)
                : (overlap < minPenetration * 0.95f - 0.0005f);
            if (better)
            {
                minPenetration = overlap;
                minAxis = L;
                minIndex = i;
            }
        }

        if (glm::dot(d, minAxis) < 0.0f)
            minAxis = -minAxis;

        result.hit = true;
        result.penetration = minPenetration;
        result.normal = minAxis;
        result.axisIndex = minIndex;
        result.contactPoint = (A.center + B.center) * 0.5f
            - minAxis * (minPenetration * 0.5f);
        return result;
    }

    // Contact manifold: vertices of one box lying inside the other box.
    // A single point in the middle of the overlap gives zero lever arm,
    // so a box could never tip over an edge or settle flat.
    static bool PointInsideOBB(const OBB& o, const glm::vec3& p, float eps)
    {
        glm::vec3 local = p - o.center;
        for (int i = 0; i < 3; ++i)
        {
            if (std::abs(glm::dot(local, o.rotation[i])) > o.halfExtents[i] + eps)
                return false;
        }
        return true;
    }

    static void CollectVertices(const OBB& src, const OBB& other, std::vector<glm::vec3>& out)
    {
        for (int sx = -1; sx <= 1; sx += 2)
            for (int sy = -1; sy <= 1; sy += 2)
                for (int sz = -1; sz <= 1; sz += 2)
                {
                    glm::vec3 p = src.center
                        + src.rotation[0] * (src.halfExtents.x * sx)
                        + src.rotation[1] * (src.halfExtents.y * sy)
                        + src.rotation[2] * (src.halfExtents.z * sz);
                    if (PointInsideOBB(other, p, 0.01f))
                        out.push_back(p);
                }
    }

    // Keep the part of the polygon where dot(n, p) <= dist (Sutherland-Hodgman).
    static void ClipPolygon(std::vector<glm::vec3>& poly, const glm::vec3& n, float dist)
    {
        std::vector<glm::vec3> out;
        const size_t cnt = poly.size();
        for (size_t i = 0; i < cnt; ++i)
        {
            const glm::vec3& a = poly[i];
            const glm::vec3& b = poly[(i + 1) % cnt];
            float da = glm::dot(n, a) - dist;
            float db = glm::dot(n, b) - dist;
            if (da <= 0.0f) out.push_back(a);
            if ((da < 0.0f && db > 0.0f) || (da > 0.0f && db < 0.0f))
            {
                float t = da / (da - db);
                out.push_back(a + (b - a) * t);
            }
        }
        poly = std::move(out);
    }

    // Face-face manifold: clip the incident face against the side planes of the
    // reference face. The resulting polygon is the REAL contact patch, so the
    // support area is correct (a cube overhanging a platform keeps its support
    // right up to the platform edge instead of only up to the nearest vertex).
    static void ClipFaceManifold(const OBB& ref, int refAxis, const glm::vec3& refOutward,
        const OBB& inc, std::vector<glm::vec3>& out)
    {
        // Incident face: the face of 'inc' whose outward normal is most opposite to refOutward.
        int m = 0; float best = 0.0f;
        for (int i = 0; i < 3; ++i)
        {
            float d = std::abs(glm::dot(inc.rotation[i], refOutward));
            if (d > best) { best = d; m = i; }
        }
        float sgn = glm::dot(inc.rotation[m], refOutward) > 0.0f ? -1.0f : 1.0f;
        int p = (m + 1) % 3, q = (m + 2) % 3;
        glm::vec3 fc = inc.center + inc.rotation[m] * (sgn * inc.halfExtents[m]);
        glm::vec3 up = inc.rotation[p] * inc.halfExtents[p];
        glm::vec3 uq = inc.rotation[q] * inc.halfExtents[q];

        std::vector<glm::vec3> poly = { fc + up + uq, fc - up + uq, fc - up - uq, fc + up - uq };

        for (int k = 1; k <= 2; ++k)
        {
            int ax = (refAxis + k) % 3;
            for (int s = -1; s <= 1; s += 2)
            {
                glm::vec3 n = ref.rotation[ax] * float(s);
                ClipPolygon(poly, n, glm::dot(n, ref.center) + ref.halfExtents[ax]);
                if (poly.empty()) return;
            }
        }

        glm::vec3 faceCenter = ref.center + refOutward * ref.halfExtents[refAxis];
        for (const glm::vec3& pt : poly)
        {
            float depth = glm::dot(pt - faceCenter, refOutward); // <0 means below the reference face
            if (depth <= 0.01f)
                out.push_back(pt - refOutward * (depth * 0.5f));     // halfway between the two surfaces
        }
    }

    static std::vector<glm::vec3> GetContactPoints(const OBB& A, const OBB& B, const CollisionResult& col)
    {
        std::vector<glm::vec3> pts;
        if (col.axisIndex >= 0 && col.axisIndex < 3)
            ClipFaceManifold(A, col.axisIndex, col.normal, B, pts);
        else if (col.axisIndex >= 3 && col.axisIndex < 6)
            ClipFaceManifold(B, col.axisIndex - 3, -col.normal, A, pts);
        else
        {
            CollectVertices(A, B, pts);
            CollectVertices(B, A, pts);
        }
        if (pts.empty())
            pts.push_back(col.contactPoint); // edge-edge fallback
        return pts;
    }

    // ============================================================
    //  Sphere collisions
    // ============================================================
    static glm::vec3 SphereCenter(const TransformComponent& tr, const ColliderComponent& col)
    {
        return tr.GetPosition() + tr.GetRotation() * col.offset;
    }

    // Sphere vs sphere. Normal points from A to B.
    static CollisionResult TestSphereSphere(const glm::vec3& cA, float rA, const glm::vec3& cB, float rB)
    {
        CollisionResult res;
        glm::vec3 d = cB - cA;
        float distSq = glm::dot(d, d);
        float rSum = rA + rB;
        if (distSq >= rSum * rSum) return res;

        float dist = std::sqrt(distSq);
        glm::vec3 n = (dist > 1e-6f) ? d / dist : glm::vec3(0.0f, 1.0f, 0.0f);

        res.hit = true;
        res.penetration = rSum - dist;
        res.normal = n;
        res.contactPoint = cA + n * (rA - res.penetration * 0.5f);
        res.axisIndex = kSingleContact;
        return res;
    }

    // Sphere vs box. The returned normal points from the BOX to the SPHERE.
    static CollisionResult TestSphereOBB(const glm::vec3& c, float r, const OBB& box)
    {
        CollisionResult res;
        glm::vec3 local = c - box.center;

        // Closest point on the box to the sphere center.
        glm::vec3 closest = box.center;
        for (int i = 0; i < 3; ++i)
        {
            float d = glm::clamp(glm::dot(local, box.rotation[i]), -box.halfExtents[i], box.halfExtents[i]);
            closest += box.rotation[i] * d;
        }

        glm::vec3 diff = c - closest;
        float distSq = glm::dot(diff, diff);
        if (distSq > r * r) return res;

        res.hit = true;
        res.axisIndex = kSingleContact;

        if (distSq > 1e-10f)
        {
            float dist = std::sqrt(distSq);
            res.normal = diff / dist;
            res.penetration = r - dist;
            res.contactPoint = closest + res.normal * (res.penetration * 0.5f);
        }
        else
        {
            // Sphere center is inside the box: push out through the nearest face.
            int best = 0; float bestDepth = FLT_MAX; float bestSign = 1.0f;
            for (int i = 0; i < 3; ++i)
            {
                float p = glm::dot(local, box.rotation[i]);
                float depth = box.halfExtents[i] - std::abs(p);
                if (depth < bestDepth)
                {
                    bestDepth = depth; best = i; bestSign = (p >= 0.0f) ? 1.0f : -1.0f;
                }
            }
            res.normal = box.rotation[best] * bestSign;
            res.penetration = bestDepth + r;
            res.contactPoint = c + res.normal * (bestDepth * 0.5f - r * 0.5f);
        }
        return res;
    }

    // Generic dispatcher. Normal always points from A to B.
    // Sphere colliders use the sphere tests; everything else is treated as a box.
    static CollisionResult TestCollision(
        const TransformComponent& trA, const ColliderComponent& colA,
        const TransformComponent& trB, const ColliderComponent& colB)
    {
        const bool sphA = colA.type == ColliderType::Sphere;
        const bool sphB = colB.type == ColliderType::Sphere;

        if (sphA && sphB)
            return TestSphereSphere(SphereCenter(trA, colA), colA.radius, SphereCenter(trB, colB), colB.radius);

        if (sphA)
        {
            CollisionResult r = TestSphereOBB(SphereCenter(trA, colA), colA.radius, MakeOBB(trB, colB));
            r.normal = -r.normal; // box->sphere  ==>  sphere(A)->box(B)
            return r;
        }

        if (sphB)
            return TestSphereOBB(SphereCenter(trB, colB), colB.radius, MakeOBB(trA, colA));

        return TestOBB(MakeOBB(trA, colA), MakeOBB(trB, colB));
    }

    void ResolvePositional(RigidbodyComponent& rbA, TransformComponent& trA,
        RigidbodyComponent& rbB, TransformComponent& trB,
        const CollisionResult& colRes)
    {
        if (rbA.isKinematic && rbB.isKinematic) return;

        float invMassA = rbA.isKinematic ? 0.0f : 1.0f / rbA.mass;
        float invMassB = rbB.isKinematic ? 0.0f : 1.0f / rbB.mass;
        float invSum = invMassA + invMassB;
        if (invSum < 1e-8f) return;

        float mag = std::max(colRes.penetration - kPenetrationSlop, 0.0f)
            / invSum * kPositionalPercent;
        glm::vec3 correction = colRes.normal * mag;

        trA.SetPosition(trA.GetPosition() - correction * invMassA);
        trB.SetPosition(trB.GetPosition() + correction * invMassB);
    }

    void ResolveImpulse(RigidbodyComponent& rbA, TransformComponent& trA,
        RigidbodyComponent& rbB, TransformComponent& trB,
        const CollisionResult& colRes, float restitution = 0.2f)
    {
        if (rbA.isKinematic && rbB.isKinematic) return;

        const float invMassA = rbA.isKinematic ? 0.0f : 1.0f / rbA.mass;
        const float invMassB = rbB.isKinematic ? 0.0f : 1.0f / rbB.mass;

        glm::vec3 rA = colRes.contactPoint - trA.GetPosition();
        glm::vec3 rB = colRes.contactPoint - trB.GetPosition();

        glm::mat3 RA = glm::mat3_cast(trA.GetRotation());
        glm::mat3 RB = glm::mat3_cast(trB.GetRotation());
        glm::mat3 IA_inv_world = rbA.isKinematic
            ? glm::mat3(0.0f)
            : RA * rbA.inertia_inv * glm::transpose(RA);
        glm::mat3 IB_inv_world = rbB.isKinematic
            ? glm::mat3(0.0f)
            : RB * rbB.inertia_inv * glm::transpose(RB);

        glm::vec3 angA = rbA.isKinematic ? glm::vec3(0.0f) : rbA.angVel;
        glm::vec3 angB = rbB.isKinematic ? glm::vec3(0.0f) : rbB.angVel;

        float jn = 0.0f;

        // --- Normal impulse ---
        {
            glm::vec3 velA = rbA.velocity + glm::cross(angA, rA);
            glm::vec3 velB = rbB.velocity + glm::cross(angB, rB);
            glm::vec3 relVel = velB - velA;

            float velAlongNormal = glm::dot(relVel, colRes.normal);
            if (velAlongNormal > 0.0f) return;

            glm::vec3 rAxN = glm::cross(rA, colRes.normal);
            glm::vec3 rBxN = glm::cross(rB, colRes.normal);
            float angularA = glm::dot(colRes.normal, glm::cross(IA_inv_world * rAxN, rA));
            float angularB = glm::dot(colRes.normal, glm::cross(IB_inv_world * rBxN, rB));

            float invSum = invMassA + invMassB + angularA + angularB;
            if (invSum < 1e-8f) return;

            float e = (std::abs(velAlongNormal) < kRestitutionThreshold) ? 0.0f : restitution;

            jn = -(1.0f + e) * velAlongNormal / invSum;
            glm::vec3 impulse = jn * colRes.normal;

            if (!rbA.isKinematic) rbA.velocity -= impulse * invMassA;
            if (!rbB.isKinematic) rbB.velocity += impulse * invMassB;

            if (!rbA.isKinematic) rbA.angVel -= IA_inv_world * glm::cross(rA, impulse);
            if (!rbB.isKinematic) rbB.angVel += IB_inv_world * glm::cross(rB, impulse);

            if (std::abs(velAlongNormal) < kRestitutionThreshold)
            {
                if (!rbA.isKinematic)
                {
                    float vnA = glm::dot(rbA.velocity, colRes.normal);
                    if (vnA < 0.0f) rbA.velocity -= vnA * colRes.normal;
                }
                if (!rbB.isKinematic)
                {
                    float vnB = glm::dot(rbB.velocity, colRes.normal);
                    if (vnB > 0.0f) rbB.velocity -= vnB * colRes.normal;
                }
            }
        }

        // --- Tangent impulse (friction) ---
        {
            // Recompute angular velocities: the normal impulse has just changed them.
            angA = rbA.isKinematic ? glm::vec3(0.0f) : rbA.angVel;
            angB = rbB.isKinematic ? glm::vec3(0.0f) : rbB.angVel;

            glm::vec3 velA = rbA.velocity + glm::cross(angA, rA);
            glm::vec3 velB = rbB.velocity + glm::cross(angB, rB);
            glm::vec3 relVel = velB - velA;

            glm::vec3 tangent = relVel - colRes.normal * glm::dot(relVel, colRes.normal);
            float tangentLen = glm::length(tangent);
            if (tangentLen < 1e-6f) return;

            tangent /= tangentLen;
            float velAlongTangent = glm::dot(relVel, tangent);

            glm::vec3 rAxT = glm::cross(rA, tangent);
            glm::vec3 rBxT = glm::cross(rB, tangent);
            float angularAT = glm::dot(tangent, glm::cross(IA_inv_world * rAxT, rA));
            float angularBT = glm::dot(tangent, glm::cross(IB_inv_world * rBxT, rB));

            float invSumT = invMassA + invMassB + angularAT + angularBT;
            if (invSumT < 1e-8f) return;

            float jt = -velAlongTangent / invSumT;
            float maxFriction = kDefaultFriction * std::abs(jn);
            jt = glm::clamp(jt, -maxFriction, maxFriction);

            glm::vec3 impulseT = jt * tangent;

            if (!rbA.isKinematic) rbA.velocity -= impulseT * invMassA;
            if (!rbB.isKinematic) rbB.velocity += impulseT * invMassB;

            if (!rbA.isKinematic) rbA.angVel -= IA_inv_world * glm::cross(rA, impulseT);
            if (!rbB.isKinematic) rbB.angVel += IB_inv_world * glm::cross(rB, impulseT);
        }
    }

    void PhysicsSystem::ApplyTorque(RigidbodyComponent& rigidbody, const glm::vec3& torque)
    {
        rigidbody.torque += torque;
    }

    void PhysicsSystem::OnUpdate(const Timestep& ts, const Scene* scene)
    {
        auto physicsComponents = scene->View<ColliderComponent, RigidbodyComponent, TransformComponent>();
        float dt = ts.GetSeconds();

        // ========================================================
        //  PASS 1: integration
        // ========================================================
        for (auto [entity, collider, rigidbody, transform] : physicsComponents.each())
        {
            if (rigidbody.isKinematic)
                continue;

            if (rigidbody.useGravity)
            {
                rigidbody.ApplyForce(glm::vec3(0.0f, -9.81f * rigidbody.mass, 0.0f));
            }

            rigidbody.acceleration = rigidbody.force / rigidbody.mass;
            rigidbody.velocity += rigidbody.acceleration * dt;
            transform.SetPosition(transform.GetPosition() + rigidbody.velocity * dt);

            rigidbody.inertia = GetInertia(collider, rigidbody);
            rigidbody.inertia_inv = glm::inverse(rigidbody.inertia);

            glm::quat q = transform.GetRotation();
            glm::vec3 torqueLocal = glm::inverse(q) * rigidbody.torque;
            glm::vec3 angAccLocal = rigidbody.inertia_inv * torqueLocal;
            glm::vec3 angAccWorld = q * angAccLocal;

            rigidbody.angAcc = angAccWorld;
            rigidbody.angVel += rigidbody.angAcc * dt;

            glm::quat omegaQuat(0.0f,
                rigidbody.angVel.x,
                rigidbody.angVel.y,
                rigidbody.angVel.z);
            glm::quat dq = 0.5f * omegaQuat * q * dt;

            q = glm::normalize(q + dq);
            transform.SetRotation(q);

            float linFactor = glm::clamp(1.0f - kDefaultLinearDamping * dt, 0.0f, 1.0f);
            float angFactor = glm::clamp(1.0f - kDefaultAngularDamping * dt, 0.0f, 1.0f);

            rigidbody.velocity *= linFactor;
            rigidbody.angVel *= angFactor;

            rigidbody.ClearForces();
            rigidbody.ClearTorque();
        }

        // ========================================================
        //  PASS 2: collisions (collect, then iterate the solver)
        // ========================================================
        struct Contact
        {
            RigidbodyComponent* rbA; TransformComponent* trA;
            RigidbodyComponent* rbB; TransformComponent* trB;
            CollisionResult col;
            std::vector<glm::vec3> points;
        };
        std::vector<Contact> contacts;
        std::unordered_set<EntityHandle> inContact;
        std::unordered_set<EntityHandle> visitedEntities;

        for (auto [entityA, colliderA, rigidbodyA, transformA] : physicsComponents.each())
        {
            for (auto [entityB, colliderB, rigidbodyB, transformB] : physicsComponents.each())
            {
                if (visitedEntities.find(entityB) == visitedEntities.end()) continue;
                if (entityA == entityB) continue;
                if (colliderA.isTrigger || colliderB.isTrigger) continue;

                CollisionResult colRes = TestCollision(transformA, colliderA, transformB, colliderB);
                if (!colRes.hit) continue;

                ResolvePositional(rigidbodyA, transformA, rigidbodyB, transformB, colRes);

                // Contact points are computed after the positional correction.
                Contact c;
                c.rbA = &rigidbodyA; c.trA = &transformA;
                c.rbB = &rigidbodyB; c.trB = &transformB;
                c.col = colRes;

                if (colRes.axisIndex == kSingleContact)
                {
                    // Sphere contact: one point, refreshed after the positional correction.
                    CollisionResult after = TestCollision(transformA, colliderA, transformB, colliderB);
                    c.points.push_back(after.hit ? after.contactPoint : colRes.contactPoint);
                }
                else
                {
                    c.points = GetContactPoints(MakeOBB(transformA, colliderA),
                        MakeOBB(transformB, colliderB), colRes);
                }
                contacts.push_back(std::move(c));

                inContact.insert(entityA);
                inContact.insert(entityB);
            }
            visitedEntities.insert(entityA);
        }

        // Sequential impulses: several iterations, one impulse per contact point.
        for (int it = 0; it < kSolverIterations; ++it)
        {
            for (auto& c : contacts)
            {
                for (const glm::vec3& p : c.points)
                {
                    CollisionResult point = c.col;
                    point.contactPoint = p;
                    ResolveImpulse(*c.rbA, *c.trA, *c.rbB, *c.trB, point);
                }
            }
        }

        // ========================================================
        //  PASS 3: contact damping + sleep
        // ========================================================
        for (auto [entity, collider, rigidbody, transform] : physicsComponents.each())
        {
            if (rigidbody.isKinematic) continue;

            const bool touching = inContact.find(entity) != inContact.end();

            if (touching)
                rigidbody.angVel *= std::exp(-kContactAngularDamping * dt);

            float linSq = glm::dot(rigidbody.velocity, rigidbody.velocity);
            float angSq = glm::dot(rigidbody.angVel, rigidbody.angVel);

            // Sleep only when BOTH linear and angular motion are tiny.
            // Zeroing angVel on its own freezes a cube that has just started to tip
            // (its angular velocity is small at that moment) - that was the "stops
            // after a while" bug.
            if (touching &&
                linSq < kSleepLinearThreshold * kSleepLinearThreshold &&
                angSq < kSleepAngularThreshold * kSleepAngularThreshold)
            {
                rigidbody.velocity = glm::vec3(0.0f);
                rigidbody.angVel = glm::vec3(0.0f);
            }
        }
    }

    glm::mat3 PhysicsSystem::GetInertia(ColliderComponent collider, RigidbodyComponent rigidbody)
    {
        float inerX = 1.0f;
        float inerY = 1.0f;
        float inerZ = 1.0f;

        switch (collider.type)
        {
        case ColliderType::Box:
            inerX = rigidbody.mass / 12.0f * (collider.size[1] * collider.size[1] + collider.size[2] * collider.size[2]);
            inerY = rigidbody.mass / 12.0f * (collider.size[0] * collider.size[0] + collider.size[2] * collider.size[2]);
            inerZ = rigidbody.mass / 12.0f * (collider.size[0] * collider.size[0] + collider.size[1] * collider.size[1]);
            break;
        case ColliderType::Sphere:
            inerX = 0.4f * rigidbody.mass * collider.radius * collider.radius;
            inerY = 0.4f * rigidbody.mass * collider.radius * collider.radius;
            inerZ = 0.4f * rigidbody.mass * collider.radius * collider.radius;
            break;
        case ColliderType::Capsule:
            inerX = rigidbody.mass / 12.0f * (3.0f * collider.radius * collider.radius + collider.height * collider.height);
            inerY = rigidbody.mass / 12.0f * (3.0f * collider.radius * collider.radius + collider.height * collider.height);
            inerZ = 0.5f * rigidbody.mass * collider.radius * collider.radius;
            break;
        default:
            break;
        }

        return glm::mat3(inerX, 0.0f, 0.0f,
            0.0f, inerY, 0.0f,
            0.0f, 0.0f, inerZ);
    }
}