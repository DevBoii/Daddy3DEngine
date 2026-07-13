#pragma once

#include <string>

namespace DaddyEngine
{
	struct Layer
	{
		int ID;
		std::string Name;
	};

	inline std::vector<Layer> m_Layers =
	{
		{0, "Default"},
		{1, "TransparentFX"},
		{2, "Ignore Raycast"},
		{3, "Player"},
		{4, "Enemy"}
	};

	inline std::vector<std::string> m_Tags =
	{
		"Default",
		"Player",
		"Enemy"
	};

	struct IdentityComponent
	{
		std::string Name = "Entity";
		std::string Tag = "Default";
		uint32_t Layer = 0;
		bool isActive = true;

		IdentityComponent() = default;
		IdentityComponent(const std::string& name, const std::string& tag = "Default", uint32_t layer = 0)
			: Name(name), Tag(tag), Layer(layer) 
		{
		}

	};
}