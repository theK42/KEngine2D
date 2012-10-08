#pragma once

namespace KEngine2D
{
	class Renderer {
	public:
		virtual ~Renderer() {};
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
	};
}