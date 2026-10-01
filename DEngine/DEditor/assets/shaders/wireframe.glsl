#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProj;
uniform mat4 u_ModelMat;

out vec3 v_WorldPos;

void main()
{
    vec4 worldPos = u_ModelMat * vec4(a_Position, 1.0f);
    v_WorldPos = worldPos.xyz;
    gl_Position = u_ViewProj * worldPos;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_WorldPos;
uniform vec3 u_ViewPos;

void main()
{
    vec3 baseColor = vec3(1., 1., 0.) + dot(u_ViewPos, vec3(0.));
    color = vec4(baseColor, 0.7f);
}
