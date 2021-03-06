#pragma once
#include "externalLibs.h"
#include "cocoa/core/Entity.h"
#include "cocoa/components/TransformStruct.h"

namespace Cocoa
{
	namespace Transform
	{
		COCOA TransformData CreateTransform();
		COCOA TransformData CreateTransform(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation);

		COCOA void Update(TransformData& data, float dt);

		COCOA void Serialize(json& j, Entity entity, const TransformData& transform);
		COCOA void Deserialize(const json& j, Entity entity, Entity parent = NEntity::CreateNull());
		COCOA void Serialize(json& j, const TransformData& transform);
		COCOA void Deserialize(const json& j, TransformData& transform);
	}

	namespace TransformSystem
	{
		COCOA void Update(SceneData& scene, float dt);
	}
}