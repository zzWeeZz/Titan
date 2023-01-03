#pragma once
#include <glm/glm.hpp>
namespace Titan
{
	template<typename T>
	void RemoveDuplicates(std::vector<T>& container)
	{
		const size_t size = container.size();
		if (size == 0 || size == 1)
			return;

		// To store index of next unique element
		int32_t j = 0;

		for (int32_t i = 0; i < size - 1; i++)
		{
			if (container[i] != container[i + 1])
			{
				container[j++] = container[i];
			}
		}
			
		container[j++] = container[size - 1];
	};

	// thanks to https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/ for the implementation of octahedron-normal vectors
	inline glm::vec2 OctWrap(const glm::vec2& v)
	{
		const glm::vec2 absOfVec = { abs(v.y),abs(v.x) };
		float fraction = 0;
		if (v.x >= 0.0 && v.y >= 0.0) // maybe should be && instead of ||
		{
			fraction = 1.0f;
		}
		else
		{
			fraction = -1.0f;
		}

		const glm::vec2 flippedAbsVec = { 1.0f - absOfVec.x, 1.0f - absOfVec.y };
		
		return flippedAbsVec * fraction;
	};

	inline glm::vec2 EncodeOctahedronVectors(glm::vec3 n)
	{
		n /= (abs(n.x) + abs(n.y) + abs(n.z));
		glm::vec2 xy = { n.x, n.y };
		if (!(n.z >= 0.0))
		{
			const auto xyOctWrap = OctWrap(xy);
			n.x = xyOctWrap.x;
			n.y = xyOctWrap.y;
		}
		n.x = n.x * 0.5 + 0.5;
		n.y = n.y * 0.5 + 0.5;
		xy = { n.x, n.y };
		return xy;
	}

	inline glm::vec3 DecodeOctahedronVectors(glm::vec2 f)
	{
		// https://twitter.com/Stubbesaurus/status/937994790553227264

		f = f * 2.0f - 1.0f;

		glm::vec3 n = glm::vec3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
		float t = glm::clamp(-n.z, 0.f, 1.f);

		if (n.x >= 0.0f || n.y >= 0.0f) // maybe should be && instead of ||
		{
			n.x += -t;
			n.y += -t;
		}
		else
		{
			n.x += t;
			n.y += t;
		}

		return glm::normalize(n);
	}
}