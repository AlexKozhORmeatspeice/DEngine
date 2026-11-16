#type vertex
#version 450

layout(location = 0) in vec3 a_Position;

layout(set = 0, binding = 0) uniform Camera
{
    mat4 u_ViewProj;
    mat4 u_ModelMat;
};

layout(location = 0) out vec3 v_WorldPos;

void main()
{
    vec4 worldPos = u_ModelMat * vec4(a_Position, 1.0);
    v_WorldPos = worldPos.xyz;
    gl_Position = u_ViewProj * worldPos;
}

#type fragment
#version 450

layout(location = 0) in vec3 v_WorldPos;

layout(location = 0) out vec4 color;

layout(set = 0, binding = 0) uniform View
{
    vec3 u_ViewPos;
};

void main()
{
    vec3 baseColor = vec3(1.0, 1.0, 0.0) + dot(u_ViewPos, vec3(0.0));
    color = vec4(baseColor, 1.0);
}
