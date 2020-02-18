#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
//ayout(location = 1) in vec4 ptColor;
//layout(location = 2) in vec2 texCoord;

//out vec2 v_TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 lightPos;
out vec3 viewPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = vec3(model* vec4(aPos, 1.0f));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	lightPos = lightPosition;
	viewPos = viewPosition;
	//v_TexCoord = texCoord;
	//v_Color = ptColor;
}