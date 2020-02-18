#version 330 core

//layout(location = 0) out vec4 color;

//in vec2 v_TexCoord;
//in vec4 v_Color;

//uniform vec4 u_Color;
//uniform sampler2D u_Texture;
in vec3 Normal;
in vec3 FragPos;
in vec3 lightPos;
in vec3 viewPos;

out vec4 FragColor;
void main()
{
	//AMBIENT LIGHT
	float ambientFactor = 0.1f;
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 objectColor = vec3(1.0f, 1.0f, 0.5f);
	//vec3 lightPos = vec3(0.0f, 0.0f, 0.0f);

	vec3 ambient = ambientFactor * lightColor;

	//DIFUSE LIGHT
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
		
	//float diff = max(dot(norm, lightDir), 0.0);
	float distance = length(lightPos - FragPos) / 30.0f;
	distance = max(distance, 0.65f);
	float diff = abs(dot(norm, lightDir));
	vec3 diffuse = diff * lightColor * 0.7f / distance;

	//SPECULAR LIGHT
	float specularStrength = 0.2;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 20);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0f);
	
	//FragColor = vec4(1.0f, 1.0f, 0.5f, 1.0f);
}