#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace DaddyEngine
{
	// A small helper struct to return two strings from one function
	struct ShaderProgramSource 
	{
		std::string VertexSource;
		std::string FragmentSource;
	};

	class Shader
	{
	private:

		unsigned int m_RendererID;
		std::string m_FilePath;

	public:

		Shader(const std::string& filepath);
		Shader(const std::string& vertexSource, const std::string& fragmentSource);
		~Shader();

		void Bind();
		void Unbind();

	private:

		std::unordered_map<std::string, int> m_UniformLocationCache;

		ShaderProgramSource ParseShader(const std::string& filepath);
		unsigned int CreateShader(const std::string& vertexSource, const std::string& fragSource);
		unsigned int CompileShader(unsigned int type, const std::string& source);
	public:
		int GetUniformLocation(const std::string& name);
		inline unsigned int GetShaderID() { return m_RendererID; }
		void SetInt(const std::string& name, int value);
		void SetUniform1f(const std::string& name, float value);
		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniformVec3(const std::string& name, const glm::vec3 vec);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniformMat4(const std::string& name, const glm::mat4& mat4);
		void SetUniform1i(const std::string& name, int value);
	};
}