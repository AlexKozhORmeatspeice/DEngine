#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 1) in vec2 a_Texcoord;

uniform mat4 u_ViewProj;
uniform mat4 u_ModelMat;

out vec3 v_Pos;
out vec4 v_Color;
out vec2 v_Texcoord;

void main()
{
	v_Pos = a_Position;
	v_Color = a_Color;
	v_Texcoord = a_Texcoord;
	gl_Position = u_ViewProj * u_ModelMat * vec4(a_Position, 1.0f);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D u_Texture;

in vec3 v_Pos;
in vec4 v_Color;
in vec2 v_Texcoord;

void main()
{
	color = texture(u_Texture, v_Texcoord);
}
