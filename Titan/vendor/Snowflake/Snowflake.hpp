#pragma once
#include <algorithm>
#include <cstdint>
#include <vector>
#include <array>
#include <bitset>
#include <cassert>
#include <unordered_map>
#include <functional>
#include "SnowID.h"
#include <stdexcept>
#define COMPONENT(comp) struct comp \
						
#define REGISTER_COMPONENT(GUID) const SnowID hashID = GUID

namespace Snowflake
{
	inline std::unordered_map<SnowID, size_t> componentSizes;
	constexpr uint32_t InvalidEntity = ~0;

	using Entity = uint32_t;
	using ByteSet = std::vector<uint8_t>;

	class ComponentPool
	{
		friend class Registry;
	public:
		ComponentPool() = default;
		ComponentPool(SnowID id) : m_Id(id)
		{
		}
		template<class T>
		void RegisterEntity(Entity entity)
		{
			if (m_ComponentMap.find(entity) == m_ComponentMap.end())
			{
				m_ComponentMap[entity] = ByteSet();
				m_ComponentMap[entity].resize(sizeof(T));
				T comp{};
				memcpy(m_ComponentMap[entity].data(), &comp, sizeof(T));
			}
		}

		void RegisterEntity(Entity entity, ByteSet& byteSet)
		{
			if (m_ComponentMap.find(entity) == m_ComponentMap.end())
			{
				m_ComponentMap[entity] = ByteSet();
				m_ComponentMap[entity].resize(byteSet.size());
				memcpy(m_ComponentMap[entity].data(), byteSet.data(), byteSet.size());
			}
		}

		void DeRegisterEntity(Entity entity)
		{
			m_ComponentMap.erase(entity);
		}

		bool IsEntityRegistered(Entity entity)
		{
			return m_ComponentMap.find(entity) != m_ComponentMap.end();
		}

		template<typename T>
		T& GetComponent(Entity entity)
		{
			return *(T*)(m_ComponentMap[entity].data());
		}

		std::vector<uint8_t> GetComponentData(Entity entity)
		{
			std::vector<uint8_t> data;
			data.resize(m_ComponentMap[entity].size());
			memcpy(data.data(), m_ComponentMap[entity].data(), m_ComponentMap[entity].size());
			return data;
		}
	private:
		SnowID m_Id;
		std::unordered_map<Entity, ByteSet> m_ComponentMap;
	};

	class Registry
	{
		friend class RegistrySerializer;
	public:
		Entity CreateEntity()
		{
			Entity entity = (uint32_t)m_Entities.size();
			m_Entities.emplace_back(entity);
			return entity;
		}

		// fix complexety to O(1)
		bool DestroyEntity(Entity& entity)
		{
			if (entity == InvalidEntity) return false;
			auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);
			if (it != m_Entities.end())
			{
				if (m_Entities.size() > 1)
					std::swap(*it, m_Entities.back());
				m_Entities.pop_back();
				while (!m_Registry[entity].empty())
				{
					m_ComponentPools[m_Registry[entity].back()].DeRegisterEntity(entity);
					m_Registry[entity].pop_back();
				}
				entity = InvalidEntity;
				return true;
			}
			return false;
		}

		bool ValidateEntity(Entity entity)
		{
			auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);
			return it != m_Entities.end();
		}

		
		template<class TComponent>
		TComponent& AddComponent(Entity entity)
		{
			if (entity == InvalidEntity)
			{
				throw std::invalid_argument("AddComponent called with invalid entity.");
			}
			auto& pool = MakeOrGetPool<TComponent>();

			pool.template RegisterEntity<TComponent>(entity);
			m_Registry[entity].push_back(TComponent().hashID);
			return pool.template GetComponent<TComponent>(entity);
		}

		template<class TComponent>
		TComponent* TryGetComponent(Entity entity)
		{
			if (entity == InvalidEntity)
			{
				throw std::invalid_argument("TryGetComponent called with invalid entity.");
			}
			auto& component = MakeOrGetPool<TComponent>();
			if (HasComponent<TComponent>(entity))
			{
				return &component.GetComponent<TComponent>(entity);
			}
			return nullptr;
		}

		template<class TComponent>
		TComponent& GetComponent(Entity entity)
		{
			if (entity == InvalidEntity)
			{
				throw std::invalid_argument("GetComponent called with invalid entity.");
			}
			auto& component = MakeOrGetPool<TComponent>();
			return component.GetComponent<TComponent>(entity);
		}

		template<class TComponent>
		void RemoveComponent(Entity entity)
		{
			if (entity == InvalidEntity)
			{
				throw std::invalid_argument("RemoveComponent called with invalid entity.");
			}
			auto& pool = MakeOrGetPool<TComponent>();
			pool.DeRegisterEntity(entity);
			auto& registry = m_Registry[entity];
			auto it = std::find(registry.begin(), registry.end(), TComponent().hashID);
			if (it != registry.end())
			{
				registry.erase(it);
			}
		}

		template<typename TComponent>
		bool HasComponent(Entity entity)
		{
			if (entity == InvalidEntity)
			{
				throw std::invalid_argument("HasComponent called with invalid entity.");
			}
			auto& component = MakeOrGetPool<TComponent>();
			return component.IsEntityRegistered(entity);
		}

		template<typename ...TComponents>
		bool HasComponents(Entity entity)
		{
			if (entity == InvalidEntity)
			{
				throw std::invalid_argument("HasComponents called with invalid entity.");
			}
			return (HasComponent<TComponents>(entity) && ...);
		}

		template<class TFunction>
		void ForEach(TFunction&& func)
		{
			for (auto entity : m_Entities)
			{
				func(entity);
			}
		}

		template<class ...TComponents, class TFunction>
		void Execute(TFunction&& func)
		{
			for (auto entity : m_Entities)
			{
				if (HasComponents<TComponents...>(entity))
				{
					func(entity, GetComponent<TComponents>(entity)...);
				}
			}
		}
	private:

		void AddComponentFromData(std::vector<uint8_t>& data, SnowID& id, Entity entt)
		{
			if (entt == InvalidEntity)
			{
				throw std::invalid_argument("AddComponent called with invalid entity.");
			}
			
			auto it = m_ComponentPools.find(id);
			if (it == m_ComponentPools.end())
			{
				m_ComponentPools[id] = ComponentPool(id);
				componentSizes[id] = data.size();
			}
			auto& pool = m_ComponentPools[id];
			

			pool.RegisterEntity(entt, data);
			m_Registry[entt].push_back(id);
		}

		template<class TComponent>
		ComponentPool& MakeOrGetPool()
		{
			SnowID hash = TComponent().hashID;
			auto it = m_ComponentPools.find(hash);
			if (it == m_ComponentPools.end())
			{
				m_ComponentPools[hash] = ComponentPool(hash);
				componentSizes[hash] = sizeof(TComponent);
			}
			return m_ComponentPools[hash];
		}

		std::vector<Entity> m_Entities;
		std::unordered_map<Entity, std::vector<SnowID>> m_Registry;
		std::unordered_map<SnowID, ComponentPool> m_ComponentPools;

	};
}