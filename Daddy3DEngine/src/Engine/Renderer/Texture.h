#pragma once

#include <string>
#include <filesystem>

namespace DaddyEngine
{
	class Texture2D
	{
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_Channels = 0;
		std::string m_FilePath;

	public:
		Texture2D(const std::filesystem::path& path);
		// Fallback Constructor for White texture
		Texture2D(uint32_t width, uint32_t height, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255);
		~Texture2D();

		void Bind(uint32_t slot);
		void UnBind();

		uint32_t GetRendererID() const { return m_RendererID; }
		uint32_t GetHeight() const { return m_Height; }
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetChannel() const { return m_Channels; }
	};

	class Cubemap
	{
	public:

		Cubemap(const std::vector<std::string>& faces);
		~Cubemap();


		uint32_t LoadCubemap(const std::vector<std::string>& faces);
		void Bind(uint32_t slot);
		void Unbind();

		uint32_t GetRendererID() const { return m_RendererID; }

	private:

		uint32_t m_RendererID;
	};
}