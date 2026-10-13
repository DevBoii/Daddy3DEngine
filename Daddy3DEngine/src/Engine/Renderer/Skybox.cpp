#include "Skybox.h"
#include <stb/stb_image.h>
#include <glad/glad.h>
#include <utility>

#include "Engine/Editor/ConsolePanel.h"

#include <json/json.hpp>

using json = nlohmann::json;

namespace DaddyEngine
{
	//-------------------------------------------------------------------------
	//---------------------------SKYBOX----------------------------------------
	//-------------------------------------------------------------------------
	Skybox::Skybox(const std::string& path)
	{
		std::fstream file(path);

		if (!file.is_open())
		{
			std::string error = "Failed to open skybox file: " + path;
			ConsolePanel::InsertError(error);
			return;
		}

		json data;
		file >> data;

		skyboxFaces =
		{
			data["Faces"]["PositiveX"],
			data["Faces"]["NegativeX"],
			data["Faces"]["PositiveY"],
			data["Faces"]["NegativeY"],
			data["Faces"]["PositiveZ"],
			data["Faces"]["NegativeZ"]
		};

		m_Cubemap = std::make_unique<Cubemap>(skyboxFaces);
	}

	Skybox::~Skybox()
	{

	}

	//-------------------------------------------------------------------------
	//---------------------------SKYBOXRENDERER--------------------------------
	//-------------------------------------------------------------------------

	SkyboxRenderer::SkyboxRenderer()
	{
		// Setup Shader
		std::string vertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 aPos;
			
			out vec3 TexCoords;
			
			uniform mat4 projection;				
			uniform mat4 view;

			void main()
			{
				TexCoords = aPos;
				gl_Position = projection * view * vec4(aPos, 1.0f);
			}				
		)";

		std::string fragSrc = R"(
			#version 330 core
			out vec4 FragColor;

			in vec3 TexCoords;

			uniform samplerCube skybox;

			uniform bool useSkyFog;
			uniform vec3 skyFogColor;
			uniform float skyFogIntensity;

			void main()
			{
				vec3 finalColor;
				vec3 skyColor = texture(skybox, TexCoords).rgb;
				
				if(useSkyFog)
				{
					vec3 direction = normalize(TexCoords);
					float horizon = 1.0 - direction.y;
					float fogAmount = horizon * skyFogIntensity;
					fogAmount = clamp(fogAmount, 0.0, 1.0);
					finalColor = mix(skyColor, skyFogColor, fogAmount);
					
				}
				else
				{
					finalColor = skyColor;
				}
				
				FragColor = vec4(finalColor, 1.0f);
			}
		
		)";

		m_SkyboxShader = std::make_unique<Shader>(vertexSrc, fragSrc);

		// Setup VAO and VBO
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	SkyboxRenderer::~SkyboxRenderer()
	{

	}

	void SkyboxRenderer::SetSkybox(std::shared_ptr<Skybox> skybox)
	{
		m_Skybox = skybox;
	}

	void SkyboxRenderer::RemoveSkybox()
	{
		if (m_Skybox == nullptr) return;
		m_Skybox = nullptr;
	}

	void SkyboxRenderer::RenderSkyBox(Camera& camera, Environment& environment)
	{
		if (!m_Skybox) return;

		glDepthMask(GL_FALSE);
		m_SkyboxShader->Bind();

		// Remove camera translation
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		m_SkyboxShader->SetUniformMat4("view", view);
		m_SkyboxShader->SetUniformMat4("projection", camera.GetProjectionMatrix());

		m_Skybox->GetCubemap()->Bind(0);
		m_SkyboxShader->SetUniform1i("skybox", 0);

		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glDepthMask(GL_TRUE);
		glm::vec3 fogColor = environment.m_FogSettings.skyFogColor;
		if (environment.m_FogSettings.useSameColor)
		{
			fogColor = environment.m_FogSettings.fogColor;
			environment.m_FogSettings.skyFogColor = environment.m_FogSettings.fogColor;
		}
		m_SkyboxShader->SetUniform1i("useSkyFog", environment.m_FogSettings.useSkyFog);
		m_SkyboxShader->SetUniformVec3("skyFogColor", fogColor);
		m_SkyboxShader->SetUniform1f("skyFogIntensity", environment.m_FogSettings.skyFogIntensity);
	}
}