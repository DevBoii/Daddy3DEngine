#pragma once

#include <vector>
#include <cstdint>
#include <limits>

namespace DaddyEngine
{
	constexpr uint32_t NullEntity = std::numeric_limits<uint32_t>::max();

	struct RelationshipComponent
	{
		uint32_t Parent = NullEntity;
		std::vector<uint32_t> Children;
	};
}