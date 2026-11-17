#type vertex
#version 450

layout(location = 0) in vec3 a_Position;

layout(set = 0, binding = 0) uniform Camera
{
    mat4 u_ViewProj;
};

layout(push_constant) uniform Model
{
    mat4 u_ModelMat;
} pc;

layout(location = 0) out vec3 v_WorldPos;

void main()
{
    vec4 worldPos = pc.u_ModelMat * vec4(a_Position, 1.0);
    v_WorldPos = worldPos.xyz;
    gl_Position = u_ViewProj * worldPos;
}

#type fragment
#version 450

layout(location = 0) in vec3 v_WorldPos;

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(1.0, 1.0, 0.0, 1.0);
}
