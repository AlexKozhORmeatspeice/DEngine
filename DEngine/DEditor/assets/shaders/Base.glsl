#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_Texcoord;
layout(location = 3) in vec3 a_Tangent;

uniform mat4 u_ViewProj;
uniform mat4 u_ModelMat;

out vec3 v_WorldPos;
out vec3 v_Normal;
out vec2 v_Texcoord;

void main()
{
    vec4 worldPos = u_ModelMat * vec4(a_Position, 1.0f);
    v_WorldPos = worldPos.xyz;
    v_Normal = normalize(mat3(transpose(inverse(u_ModelMat))) * a_Normal);
    v_Texcoord = a_Texcoord;
    
    gl_Position = u_ViewProj * worldPos;
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D u_Texture;

// Light uniforms
uniform vec3 u_LightDirection = vec3(-0.5f, -1.0f, -0.5f);
uniform vec3 u_LightColor = vec3(1.0f, 1.0f, 1.0f);
uniform float u_AmbientStrength = 0.2f;
uniform vec3 u_ViewPos;

in vec3 v_WorldPos;
in vec3 v_Normal;
in vec2 v_Texcoord;

void main()
{
    vec4 texColor = texture(u_Texture, v_Texcoord);
    vec3 baseColor = texColor.rgb;
    
    vec3 normal = normalize(v_Normal);
    vec3 lightDir = normalize(-u_LightDirection);
    
    // Ambient
    vec3 ambient = u_AmbientStrength * u_LightColor;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * u_LightColor;
    
    // Specular
    float specularStrength = 0.5f;
    float shininess = 32.0f;
    
    vec3 viewDir = normalize(u_ViewPos - v_WorldPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0f), shininess);
    vec3 specular = specularStrength * spec * u_LightColor;
    
    color = vec4((ambient + diffuse + specular) * baseColor, 1.0f);
}
