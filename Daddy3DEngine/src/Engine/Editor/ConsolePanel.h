#pragma once
#include <string>
#include <vector>
#include <imgui.h>

namespace DaddyEngine
{
	struct LogStruct
	{
		std::string Text;
		ImVec4 TextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	};

	class ConsolePanel
	{
	public:
		ConsolePanel();
		~ConsolePanel();


		void OnImGuiRender();
		static void Log(std::string& text);
		static void InsertWarning(std::string& text);
		static void InsertError(std::string& text);
		void ClearLog();

	private:
		static std::vector<LogStruct> m_Outputs;
	};
}