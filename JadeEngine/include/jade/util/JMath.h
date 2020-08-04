#pragma once
#include "externalLibs.h"

namespace Jade
{
	namespace JMath
	{
		// Float Comparison functions, using custom epsilon
		bool Compare(float x, float y, float epsilon = std::numeric_limits<float>::min());
		bool Compare(const glm::vec3& vec1, const glm::vec3& vec2, float epsilon = std::numeric_limits<float>::min());
		bool Compare(const glm::vec2& vec1, const glm::vec2& vec2, float epsilon = std::numeric_limits<float>::min());
		bool Compare(const glm::vec4& vec1, const glm::vec4& vec2, float epsilon = std::numeric_limits<float>::min());

		// Vector conversions
		glm::vec2 Vector2From3(const glm::vec3& vec);
		glm::vec3 Vector3From2(const glm::vec2& vec);

		// Math functions
		void Rotate(glm::vec2& vec, float angleDeg, const glm::vec2& origin);
		void Rotate(glm::vec3& vec, float angleDeg, const glm::vec3& origin);

		float ToRadians(float degrees);
		float ToDegrees(float radians);

		// Serialize math components
		json Serialize(const std::string& name, const glm::vec4& vec);
		glm::vec4 DeserializeVec4(const json& json);
		glm::vec4 DeserializeVec4(const json& json, bool& success);
		json Serialize(const std::string& name, const glm::vec3& vec);
		glm::vec3 DeserializeVec3(const json& json);
		glm::vec3 DeserializeVec3(const json& json, bool& success);
		json Serialize(const std::string& name, const glm::vec2& vec);
		glm::vec2 DeserializeVec2(const json& json);
		glm::vec2 DeserializeVec2(const json& json, bool& success);
	}
}