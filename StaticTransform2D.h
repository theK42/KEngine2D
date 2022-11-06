#pragma  once

#include "Transform2D.h"

namespace KEngineCore
{
	class LuaScheduler;
	class TweenSystem;
}

namespace KEngine2D
{
	class StaticTransform : public Transform
	{
	public:
		StaticTransform(Point const & translation = Point::Origin(), double radians = 0.0f, Point scale = Point::Identity());
	
		void Init() {};
		void Deinit() {};

		virtual Point GetTranslation() const;
		virtual double GetRotation() const;
		virtual Point GetScale() const;
        virtual const Matrix & GetAsMatrix() const;
	
		void SetTranslation(Point const & translation);
		void SetRotation(double rotation);
		void SetScale(Point const& scale);

		static StaticTransform const & Identity();

		StaticTransform& operator=(Transform const& other);

		static const char MetaName[];
	private:
        void UpdateMatrix();
        
		Point mTranslation;
		double mRotation;
		Point mScale;
        Matrix mMatrix;
		friend class TransformLibrary;
	};

	class TransformLibrary : public KEngineCore::LuaLibrary
	{
	public:
		TransformLibrary();
		virtual ~TransformLibrary() override;
		void Init(KEngineCore::LuaScheduler* scheduler, KEngineCore::TweenSystem * tweenSystem);
		void Deinit();
		virtual void RegisterLibrary(lua_State* luaState, char const* name = "transforms") override;
		const KEngineCore::LuaWrapping<StaticTransform>& GetTransformWrapping() const;
	private:
		KEngineCore::LuaWrapping<StaticTransform>	mTransformLuaWrapping; 
		KEngineCore::LuaScheduler*					mScheduler{ nullptr };
		KEngineCore::TweenSystem*					mTweenSystem{ nullptr };
	};

}

