#include "Texture.h"
#include "Engine/Editor/ConsolePanel.h"
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <iostream>

namespace DaddyEngine
{
	Texture2D::Texture2D(const std::filesystem::path& path)
		:m_FilePath(path.string())
	{
		int width, height, channels;
		stbi_uc* data = stbi_load(m_FilePath.c_str(), &width, &height, &channels, 0);

		if (!data)
		{
			std::string warning = "[DaddyEngine] Failed to load texture at path: " + m_FilePath;
			//std::cerr << warning << std::endl;
			ConsolePanel::InsertWarning(warning);
			return;
		}

		m_Width = static_cast<uint32_t>(width);
		m_Height = static_cast<uint32_t>(height);
		m_Channels = static_cast<uint32_t>(channels);

		GLenum internalFormat;
		GLenum dataFormat;

		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		else if (channels == 1)
		{
			internalFormat = GL_R8;
			dataFormat = GL_RED;
		}

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		// Texture Filtering (Linear interpolation for smooth scaling)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Texture Wrapping (Repeat mode)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Upload pixel data to GPU
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}

	// Fallback Constructor for White texture
	Texture2D::Texture2D(uint32_t width, uint32_t height, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		m_Width = width;
		m_Height = height;

		uint8_t pixelData[4]{ r,g,b,a };

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture2D::Bind(uint32_t slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);		// This chooses which slot you're about to modify.
		glBindTexture(GL_TEXTURE_2D, m_RendererID);	// For the next draw calls, use this texture.
	}

	void Texture2D::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}