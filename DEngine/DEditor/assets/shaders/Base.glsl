#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_Texcoord;
layout(location = 3) in vec3 a_Tangent;

layout(set = 0, binding = 0) uniform Camera
{
    mat4 u_ViewProj;
};

layout(push_constant) uniform Model
{
    mat4 u_ModelMat;
} pc;

layout(location = 0) out vec3 v_WorldPos;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_Texcoord;

void main()
{
    vec4 worldPos = pc.u_ModelMat * vec4(a_Position, 1.0);
    v_WorldPos = worldPos.xyz;
    v_Normal = normalize(mat3(transpose(inverse(pc.u_ModelMat))) * a_Normal);
    v_Texcoord = a_Texcoord;

    gl_Position = u_ViewProj * worldPos;
}

#type fragment
#version 450

layout(location = 0) in vec3 v_WorldPos;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_Texcoord;

layout(location = 0) out vec4 color;

layout(set = 1, binding = 0) uniform sampler2D u_Texture;

layout(set = 0, binding = 1) uniform Light
{
    vec3 u_LightDirection;
    vec3 u_LightColor;
    vec3 u_ViewPos;
};

void main()
{
    vec4 texColor = texture(u_Texture, v_Texcoord);
    vec3 baseColor = texColor.rgb;

    vec3 normal = normalize(v_Normal);
    vec3 lightDir = normalize(-u_LightDirection);

    const float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * u_LightColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;

    const float specularStrength = 0.5;
    const float shininess = 32.0;

    vec3 viewDir = normalize(u_ViewPos - v_WorldPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * u_LightColor;

    color = vec4((ambient + diffuse + specular) * baseColor, 1.0);
}
