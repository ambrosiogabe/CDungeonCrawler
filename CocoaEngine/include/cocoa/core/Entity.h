#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/scenes/SceneData.h"
#include "cocoa/core/EntityStruct.h"

#include <entt/core/type_info.hpp>

namespace Cocoa
{
	namespace NEntity
	{
		COCOA Entity CreateEntity(entt::entity raw);
		COCOA void SetScene(SceneData* scene);
		COCOA SceneData* GetScene();
		COCOA Entity CreateNull();

		template<typename Component>
		void RegisterComponentType()
		{
			SceneData* scene = GetScene();
			scene->Registry.prepare<Component>();
		}

		template<typename Type>
		void Clear()
		{
			SceneData* scene = GetScene();
			scene->Registry.clear<Type>();
		}

		template<typename... Component>
		bool HasComponent(Entity entity)
		{
			SceneData* scene = GetScene();
			return scene->Registry.has<Component...>(entity.Handle);
		}

		template<typename T, typename ... Args>
		T& AddComponent(Entity entity, Args&&... args)
		{
			Logger::Assert(!HasComponent<T>(entity), "Entity already has component.");
			SceneData* scene = GetScene();
			return scene->Registry.emplace<T>(entity.Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent(Entity entity)
		{
			Logger::Assert(HasComponent<T>(entity), "Entity does not have component.");
			SceneData* scene = GetScene();
			return scene->Registry.get<T>(entity.Handle);
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			Logger::Assert(HasComponent<T>(entity), "Entity does not have component.");
			SceneData* scene = GetScene();
			scene->Registry.remove<T>(entity.Handle);
		}

		inline bool IsNull(Entity entity)
		{
			return entity.Handle == entt::null;
		}

		inline uint32 GetID(Entity entity)
		{
			return (uint32)(entt::to_integral(entity.Handle));
		}

		template<typename T>
		Entity FromComponent(const T& component)
		{
			SceneData* scene = GetScene();
			size_t offset = &component - scene->Registry.raw<T>();
			Logger::Assert(offset < scene->Registry.size(), "Tried to get nonexistent entity.");
			return Entity{*(scene->Registry.data<T>() + offset)};
		}
	}

	COCOA bool operator==(const Entity& a, const Entity& b);
	COCOA bool operator==(Entity& a, Entity& other);
	COCOA bool operator!=(const Entity& a, const Entity& b);
	COCOA bool operator!=(Entity& a, Entity& b);
}