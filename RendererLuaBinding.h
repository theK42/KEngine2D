#pragma once

#include "Lua/lua.hpp"
#include "Renderer2D.h"

namespace KEngine2D {

	class RendererBinding {
	public:
		RendererBinding();
		~RendererBinding();
		void Init(lua_State * luaState, Renderer const * renderer);
		void Deinit();
		Renderer const * GetRenderer() const;
		static RendererBinding * GetInstance();
	private:
		lua_State * mLuaState;
		Renderer const * mRenderer;
		static RendererBinding * mInstance;
	};
}

