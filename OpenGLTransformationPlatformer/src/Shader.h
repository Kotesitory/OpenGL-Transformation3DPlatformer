#pragma once

#include<string>
#include <unordered_map>

#include "glm/glm.hpp"

class Shader
{
private:
	unsigned int m_RendererId;
	std::string m_VertexShFilePath;
	std::string m_FragmentShFilePath;
	std::unordered_map<std::string, unsigned int> m_UniformLocationCache;
public:
	Shader(const std::string & vertexShaderFilePath, const std::string& fragmentShaderFilePath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformVec3(const std::string& name, const glm::vec3& value);
	void SetUniformMat4f(const std::string& name, const glm::mat4 matrix);
private:
	std::string ParseShader(const std::string& filepath);
	unsigned int CompileShader(const std::string& source, unsigned int type);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	int GetUniformLocation(const std::string& name);
	bool CompileShader();
};