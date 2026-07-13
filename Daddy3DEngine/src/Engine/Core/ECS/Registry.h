#pragma once

#include "SparseSet.h"
#include <unordered_map>
#include <memory>
#include <cassert>
#include <tuple>
#include <typeindex>
#include <iostream>

#include "Engine/Scene/Components/IdentityComponent.h"

namespace DaddyEngine
{
	class Registry;

	template <typename... Components>
	class View
	{
	private:
		Registry& m_Registry;
		ISparseSet* m_smallestPool;

	public:
		View(Registry& registry) : m_Registry(registry)
		{
			// Get the smallest pool
			auto pools = std::tuple{ (&m_Registry.template GetPool<Components>())... };
			m_smallestPool = std::get<0>(pools);

			std::apply( 
				[&](auto*...poolPtrs) {
					
					auto Compare = [&](ISparseSet* next)
						{
							if (next->Size() < m_smallestPool->Size())
								m_smallestPool = next;
						};

					(Compare(poolPtrs), ...);
				}
				, pools
			);
		}

		class Iterator
		{
		private:
			Registry& m_Registry;
			ISparseSet* m_smallestPool;
			size_t m_index = 0;

			//	Filtering
			void SkipInvalid()
			{
				while (m_index < m_smallestPool->GetDense().size())
				{
					uint32_t currentEntity = m_smallestPool->GetDense()[m_index];
					// Use fold expression to check pack
					if ((m_Registry.template HasComponent<Components>(currentEntity) && ...))
					{
						break;
					}

					++m_index;
				}
			}

		public:

			Iterator(Registry& registry, ISparseSet* smallestPool, size_t index)
				:m_Registry(registry), m_smallestPool(smallestPool), m_index(index)
			{
				SkipInvalid();
			}

			Iterator& operator++()
			{
				++m_index;
				SkipInvalid();
				return *this;
			}

			bool operator!=(const Iterator& other) const
			{
				return m_index != other.m_index;
			}

			uint32_t operator*() const
			{
				return m_smallestPool->GetDense()[m_index];
			}
		};

		Iterator begin() const
		{
			return Iterator(m_Registry, m_smallestPool, 0);
		}

		Iterator end() const
		{
			return Iterator(m_Registry, m_smallestPool, m_smallestPool->GetDense().size());
		}
	};

	class Registry
	{
	private:
		uint32_t m_NextEntity = 0;
		std::unordered_map<size_t, std::shared_ptr<ISparseSet>> m_ComponentPools;

	public:

		uint32_t CreateEntity();
		void DestroyEntity(uint32_t entity);

		size_t GetEntityCount() const
		{
			size_t type = typeid(IdentityComponent).hash_code();
			auto it = m_ComponentPools.find(type);

			// Agar abhi tak ek bhi entity nahi bani aur pool nahi bana, toh size 0 hai
			if (it == m_ComponentPools.end())
				return 0;

			auto pool = std::static_pointer_cast<SparseSet<IdentityComponent>>(it->second);
			return pool->Size(); // <--- Direct SparseSet ka Size() return karega bina View ke!
		}

		template <typename T>
		void AddComponent(uint32_t entity, const T& component)
		{
			size_t type = typeid(T).hash_code();

			if (m_ComponentPools.find(type) == m_ComponentPools.end())
			{
				m_ComponentPools[type] = std::make_shared<SparseSet<T>>();
			}

			// Cast the base pointer to the specific template type SparseSet<T>
			auto pool = std::static_pointer_cast<SparseSet<T>>(m_ComponentPools[type]);
			pool->Insert(entity, component);
		}

		template <typename T>
		T& GetComponent(uint32_t entity)
		{
			size_t type = typeid(T).hash_code();
			// Crashes in Debug mode if you forgot to call AddComponent for this type at least once
			assert(m_ComponentPools.find(type) != m_ComponentPools.end() && "Component pool does not exist!");
			auto pool = std::static_pointer_cast<SparseSet<T>>(m_ComponentPools[type]);
			return pool->Get(entity);
		}

		template <typename T>
		bool HasComponent(uint32_t entity)
		{
			size_t type = typeid(T).hash_code();
			// pool doesn't exist
			if (m_ComponentPools.find(type)	== m_ComponentPools.end())
			{
				return false;
			}

			auto pool = std::static_pointer_cast<SparseSet<T>>(m_ComponentPools[type]);
			return pool->Has(entity);
		}

		template <typename T>
		void RemoveComponent(uint32_t entity)
		{
			size_t type = typeid(T).hash_code();

			if (m_ComponentPools.find(type) != m_ComponentPools.end())
			{
				// Cast the base pointer to the specific template type SparseSet<T>
				auto pool = std::static_pointer_cast<SparseSet<T>>(m_ComponentPools[type]);
				pool->Remove(entity);
			}
		}

		template <typename T>
		SparseSet<T>& GetPool()
		{
			size_t type = typeid(T).hash_code();
			auto it = m_ComponentPools.find(type);
			assert(it != m_ComponentPools.end() && "Component pool does not exist!");
			/*
			* // Create a base class shared pointer pointing to a derived instance
					std::shared_ptr<Base> base_ptr = std::make_shared<Derived>();

			   // Downcast to the derived type pointer safely using static_pointer_cast
					std::shared_ptr<Derived> derived_ptr = std::static_pointer_cast<Derived>(base_ptr);
			*/
			auto pool = std::static_pointer_cast<SparseSet<T>>(m_ComponentPools[type]);

			return* pool;
		}

		template<typename... Components>
		View<Components...> GetView()
		{
			(EnsurePoolExists<Components>(), ...);
			return DaddyEngine::View<Components...>(*this);
		}

		template<typename T>
		void EnsurePoolExists()
		{
			size_t typeHash = typeid(T).hash_code();
			auto it = m_ComponentPools.find(typeHash);

			if (it == m_ComponentPools.end())
			{
				m_ComponentPools[typeHash] = std::make_shared<SparseSet<T>>();

				std::cout << "[Registry] Lazy initialized empty pool for component hash: " << typeHash << "\n";
			}
		}
	};
}