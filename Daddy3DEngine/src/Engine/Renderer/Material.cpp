#include "Material.h"

namespace DaddyEngine
{
	void Material::Bind(Shader& shader)
	{
        shader.Bind();

        // Pass material color and lighting parameters
        shader.SetUniformVec3("u_Material.AlbedoColor", AlbedoColor);
        shader.SetUniform1f("u_Material.SpecularIntensity", SpecularIntensity);
        shader.SetUniform1f("u_Material.Shininess", Shininess);

        // Bind Albedo (Diffuse) Texture if available
        if (AlbedoMap != nullptr)
        {
            AlbedoMap->Bind(0); // Bind to GPU Texture Unit 0
            shader.SetUniform1i("u_Material.UseAlbedoMap", 1);
            shader.SetUniform1i("u_Material.AlbedoMap", 0);
        }
        else
        {
            shader.SetUniform1i("u_Material.UseAlbedoMap", 0);
        }

        // Bind Specular Texture if available
        if (SpecularMap != nullptr)
        {
            SpecularMap->Bind(1); // Bind to GPU Texture Unit 1
            shader.SetUniform1i("u_Material.UseSpecularMap", 1);
            shader.SetUniform1i("u_Material.SpecularMap", 1);
        }
        else
        {
            shader.SetUniform1i("u_Material.UseSpecularMap", 0);
        }
	}
}