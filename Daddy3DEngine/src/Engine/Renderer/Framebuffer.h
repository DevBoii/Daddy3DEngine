#pragma once

namespace DaddyEngine
{
	class Framebuffer
	{
	private:
	public:
		unsigned int m_framebuffer, m_texture, m_renderbuffer;
		int width, height;

		Framebuffer();
		~Framebuffer();

		void Bind();
		void Unbind();

		void Resize(int w, int h);
	};
}