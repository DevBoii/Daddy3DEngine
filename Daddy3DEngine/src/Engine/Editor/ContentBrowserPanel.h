#pragma once

#include <filesystem>
#include <string>
#include <iostream>

namespace DaddyEngine
{
	class ContentBrowserPanel
	{
	private:
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_BaseDirectory;

		void DrawDirectoryTree(const std::filesystem::path& directory);

	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	};
}