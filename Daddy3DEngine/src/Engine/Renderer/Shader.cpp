#include "Shader.h"
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <string>

namespace DaddyEngine
{
	Shader::Shader(const std::string& filepath)
	{
		m_FilePath = filepath;
		ShaderProgramSource source = ParseShader(filepath);

		m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	}

	Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
	{
		m_RendererID = CreateShader(vertexSource, fragmentSource);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::Bind()
	{
		glUseProgram(m_RendererID);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
	}

	unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
	{
		unsigned int id;
		const char* src = source.c_str();
		id = glCreateShader(type);
		glShaderSource(id, 1, &src, NULL);
		glCompileShader(id);

		int success;
		char msg[512];
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(id, 512, NULL, msg);
			std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "VERTEX " : "FRAGMENT ") << "shader!" << std::endl;
			std::cout << msg << std::endl;
			glDeleteShader(id);
			return 0;
		}

		return id;
	}

	unsigned int Shader::CreateShader(const std::string& vertexSource, const std::string& fragSource)
	{
		unsigned int program;

		unsigned int vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
		unsigned int fragment = CompileShader(GL_FRAGMENT_SHADER, fragSource);

		program = glCreateProgram();
		
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);
		glValidateProgram(program);

		int result;
		glGetProgramiv(program, GL_VALIDATE_STATUS, &result);

		if (result == GL_FALSE) {
			char message[512];
			glGetProgramInfoLog(program, 512, NULL, message);
			std::cout << "Validation failed: " << message << std::endl;
		}
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		
		return program;
	}

	int Shader::GetUniformLocation(const std::string& name)
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];

		int location = glGetUniformLocation(m_RendererID, name.c_str());
		if(location == -1)
			std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;

		m_UniformLocationCache[name] = location;
		return location;
	}

	void Shader::SetInt(const std::string& name, int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
	{
		glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
	}

	void Shader::SetUniformMat4(const std::string& name, const glm::mat4& mat4)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat4));
	}

	void Shader::SetUniform1f(const std::string& name, float value) 
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
	{
		glUniform3f(GetUniformLocation(name), v0, v1, v2);
	}

	void Shader::SetUniformVec3(const std::string& name, const glm::vec3 vec)
	{
		glUniform3f(GetUniformLocation(name), vec.x, vec.y, vec.z);
	}

	void Shader::SetUniform1i(const std::string& name, int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	ShaderProgramSource Shader::ParseShader(const std::string& filepath)
	{
		std::ifstream sourceFile(filepath);

		if (!sourceFile.is_open()) 
		{
			std::cout << "Warning: Could not open shader file at " << filepath << std::endl;
			return { "", "" };
		}

		std::string vertexSource;
		std::string fragSource;
		std::string line;
		
		enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
		ShaderType type = ShaderType::NONE;

		while (getline(sourceFile, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("fragment") != std::string::npos)
					type = ShaderType::FRAGMENT;
				else
					type = ShaderType::NONE;

				continue; // skip rest of loop early once find shader type
			}
			else
			{
				if (type == ShaderType::VERTEX)							
					vertexSource += line + "\n";
				else if (type == ShaderType::FRAGMENT)					
					fragSource += line + "\n";
			}
		}

		sourceFile.close();

		return { vertexSource, fragSource };
	}
}