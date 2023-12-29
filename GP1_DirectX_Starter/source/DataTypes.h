#pragma once
#include "Math.h"

namespace dae
{
	struct Vertex_PosCol
	{
		Vector3 position{};
		ColorRGB color{ };
		Vector2 uv{}; 
		Vector3 normal;
		Vector3 tangent;
	};
}