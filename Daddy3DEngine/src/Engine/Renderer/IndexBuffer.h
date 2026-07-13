#pragma once

namespace DaddyEngine
{
	class IndexBuffer
	{
	private:
		unsigned int m_RendererID;
		int m_count;

	public:
		IndexBuffer(const unsigned int* indices, unsigned int count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		unsigned int GetCount() const;
	};
}