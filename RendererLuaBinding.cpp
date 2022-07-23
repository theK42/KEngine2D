#include "RendererLuaBinding.h"
#include <assert.h> 
#include "lauxlib.h"

KEngine2D::RendererBinding * KEngine2D::RendererBinding::mInstance = nullptr;

KEngine2D::RendererBinding * KEngine2D::RendererBinding::GetInstance() {
	return mInstance;
}

KEngine2D::RendererBinding::RendererBinding() {
	mRenderer = nullptr;
	mLuaState = nullptr;
}

KEngine2D::RendererBinding::~RendererBinding() {
	Deinit();
}

int getDimensions(lua_State * luaState) {
	KEngine2D::RendererBinding * binding = KEngine2D::RendererBinding::GetInstance();
	assert(binding);
	KEngine2D::Renderer const * renderer = binding->GetRenderer();
	int width = renderer->GetWidth();
	int height = renderer->GetHeight();
	lua_checkstack(luaState, 2);
	lua_pushnumber(luaState, width);
	lua_pushnumber(luaState, height);
	return 2;
}

const struct luaL_Reg rendererLibrary [] = {
	{"getDimensions", getDimensions},
	{nullptr, nullptr}
};

int luaopen_renderer (lua_State * luaState) {
	luaL_newlib(luaState, rendererLibrary);	
	return 1;
};

void KEngine2D::RendererBinding::Init(lua_State * luaState, Renderer const * renderer) {
	assert(mInstance == nullptr);
	mInstance = this;
	mLuaState = luaState;
	mRenderer = renderer;
	
	lua_checkstack(luaState, 3);
	lua_getglobal(luaState, "package");
	lua_getfield(luaState, -1, "preload");
	lua_pushcfunction(luaState, luaopen_renderer);
	lua_setfield(luaState, -2, "renderer");
	lua_pop(luaState, 2);
}

void KEngine2D::RendererBinding::Deinit() {
	if (mInstance == this) {
		mInstance = nullptr;
	}
	
	lua_checkstack(mLuaState, 3);
	lua_getglobal(mLuaState, "package");
	lua_getfield(mLuaState, -1, "preload");
	lua_pushnil(mLuaState);
	lua_setfield(mLuaState, -2, "renderer");
	lua_pop(mLuaState, 2);
}

KEngine2D::Renderer const * KEngine2D::RendererBinding::GetRenderer() const {
	return mRenderer;
}