#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include "Shader.h"
#include "Texture.h"
#include "Engine/Core/Camera.h"

namespace DaddyEngine
{
    class Skybox
    {
    public:

        Skybox(const std::string& path);
        ~Skybox();

        std::vector<std::string> GetSkyboxFaces() { return skyboxFaces; }
        Cubemap* GetCubemap() const { return m_Cubemap.get(); }

    private:
        std::vector<std::string> skyboxFaces;
        std::unique_ptr<Cubemap> m_Cubemap;
    };

	class SkyboxRenderer
	{
	public:
        SkyboxRenderer();
		~SkyboxRenderer();
        void SetSkybox(std::shared_ptr<Skybox> skybox);
        void RemoveSkybox();
        std::shared_ptr<Skybox> GetSkybox() { return m_Skybox; }

		void RenderSkyBox(Camera& camera);

	private:

        unsigned int m_VAO;
        unsigned int m_VBO;

        float skyboxVertices[108] = {

            // =========================
            // BACK FACE
            // =========================

            // Triangle 1
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

             // Triangle 2
              1.0f, -1.0f, -1.0f,
              1.0f,  1.0f, -1.0f,
             -1.0f,  1.0f, -1.0f,


             // =========================
             // FRONT FACE
             // =========================

             // Triangle 1
             -1.0f, -1.0f,  1.0f,
             -1.0f,  1.0f,  1.0f,
              1.0f,  1.0f,  1.0f,

              // Triangle 2
               1.0f,  1.0f,  1.0f,
               1.0f, -1.0f,  1.0f,
              -1.0f, -1.0f,  1.0f,


              // =========================
              // LEFT FACE
              // =========================

              // Triangle 1
              -1.0f,  1.0f,  1.0f,
              -1.0f,  1.0f, -1.0f,
              -1.0f, -1.0f, -1.0f,

              // Triangle 2
              -1.0f, -1.0f, -1.0f,
              -1.0f, -1.0f,  1.0f,
              -1.0f,  1.0f,  1.0f,


              // =========================
              // RIGHT FACE
              // =========================

              // Triangle 1
               1.0f,  1.0f, -1.0f,
               1.0f,  1.0f,  1.0f,
               1.0f, -1.0f,  1.0f,

               // Triangle 2
                1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,


                // =========================
                // BOTTOM FACE
                // =========================

                // Triangle 1
                -1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f, -1.0f,
                 1.0f, -1.0f,  1.0f,

                 // Triangle 2
                  1.0f, -1.0f,  1.0f,
                 -1.0f, -1.0f,  1.0f,
                 -1.0f, -1.0f, -1.0f,


                 // =========================
                 // TOP FACE
                 // =========================

                 // Triangle 1
                 -1.0f,  1.0f,  1.0f,
                  1.0f,  1.0f,  1.0f,
                  1.0f,  1.0f, -1.0f,

                  // Triangle 2
                   1.0f,  1.0f, -1.0f,
                  -1.0f,  1.0f, -1.0f,
                  -1.0f,  1.0f,  1.0f
        };
        std::unique_ptr<Shader> m_SkyboxShader;
        std::shared_ptr<Skybox> m_Skybox;
	};
}