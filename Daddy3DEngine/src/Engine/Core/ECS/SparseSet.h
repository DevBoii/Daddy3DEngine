#pragma once

#include <vector>
#include <utility> 

namespace DaddyEngine
{
	class ISparseSet
	{
	public:
		virtual ~ISparseSet() = default;
		virtual void EntityDestroyed(uint32_t entity) = 0;
		virtual size_t Size() const = 0;
		virtual const std::vector<uint32_t>& GetDense() const = 0;
	};

	template <typename T>
	class SparseSet : public ISparseSet
	{
	private:

	public:
		std::vector<uint32_t> m_Dense;  // entity list
		std::vector<uint32_t> m_Sparse; // mapping
		std::vector<T>        m_Data;   // data

		bool Has(uint32_t entity)
		{
			if (m_Sparse.size() <= entity) return false;

			uint32_t index = m_Sparse[entity];

			return index < m_Dense.size() && m_Dense[index] == entity;
		}

		T& Get(uint32_t entity)
		{
			return m_Data[m_Sparse[entity]];
		}
		size_t Size() const override
		{
			return m_Data.size();
		}

		const std::vector<uint32_t>& GetDense() const override
		{
			return m_Dense;
		}

		void Insert(uint32_t entity, const T& component)
		{
			// Check if entity id is greter than sparse set if yes rezise it with id + 1 
			if (entity >= m_Sparse.size())
				m_Sparse.resize(entity + 1);

			//sparse hold component at enity id
			uint32_t index = m_Dense.size();

			// hold the entity id at entity index
			m_Sparse[entity] = index;
			m_Dense.push_back(entity);
			m_Data.push_back(component);
		}
		
		void Remove(uint32_t entity)
		{
			// remove the entity specific component
			// Get entity component index from list
			uint32_t index = m_Sparse[entity];
			uint32_t lastIndex = m_Data.size() - 1;

			// Swap it with from last element
			m_Data[index] = m_Data[lastIndex];
			m_Dense[index] = m_Dense[lastIndex];
			uint32_t movedEntity = m_Dense[lastIndex];
			m_Sparse[movedEntity] = index;

			// Remove it from m_Data
			m_Data.pop_back();
			m_Dense.pop_back();
		}

		void EntityDestroyed(uint32_t entity)
		{
			if (Has(entity))
				Remove(entity);
		}

		std::vector<T>& GetData()
		{
			return m_Data;
		}
	};
}