#include "ConsolePanel.h"

namespace DaddyEngine
{
	std::vector<LogStruct> ConsolePanel::m_Outputs;
	ConsolePanel::ConsolePanel()
	{

	}

	ConsolePanel::~ConsolePanel()
	{

	}

	void ConsolePanel::OnImGuiRender()
	{
		ImGui::Begin("Console");

		if (!m_Outputs.empty())
		{
			for (auto& log : m_Outputs)
			{
				ImGui::TextColored(log.TextColor, "%s", log.Text.c_str());
			}

			if (ImGui::Button("Clear Log"))
			{
				ClearLog();
			}
		}

		ImGui::End();
	}

	void ConsolePanel::Log(std::string& text)
	{
		LogStruct log;

		log.Text = text;
		log.TextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		m_Outputs.push_back(log);
	}

	void ConsolePanel::InsertWarning(std::string& text)
	{
		LogStruct log;

		log.Text = text;
		log.TextColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

		m_Outputs.push_back(log);
	}

	void ConsolePanel::InsertError(std::string& text)
	{
		LogStruct log;

		log.Text = text;
		log.TextColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

		m_Outputs.push_back(log);
	}

	void ConsolePanel::ClearLog()
	{
		m_Outputs.clear();
	}
}