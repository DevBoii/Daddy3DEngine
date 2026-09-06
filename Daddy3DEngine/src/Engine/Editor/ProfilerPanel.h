#pragma once

#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/Renderer.h"
#include <imgui.h>

namespace DaddyEngine
{
	class ProfilerPanel
	{
	public:

		ProfilerPanel()
		{

		}

		static void OnImGuiRender(Timestep ts)
		{
			ImGui::Begin("Profiler");

			float fps = ts.GetSeconds() > 0.0f ? (1.0f / ts.GetSeconds()) : 0.0f;
			ImGui::Text("FPS: %.1f", fps);
			ImGui::Text("Frame Time: %.3f ms", ts.GetMilliseconds());
			ImGui::Separator();
			ImGui::Text("Draw Calls: %u", Renderer::GetDrawCalls());
			ImGui::Separator();
			ImGui::Text("Triangles: %u", Renderer::GetTriangleCount());

			ImGui::End();
		}

	private:

	};
}