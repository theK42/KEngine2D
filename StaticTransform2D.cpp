#include "StaticTransform2D.h"
#include "Tweening.h"
#include <cmath>
#ifdef __cpp_lib_math_constants
#include <numbers>
#else //Embarrassing workaround for Android
namespace std {
    namespace numbers {
        const double pi = 3.14159265358979323846;
    }
}
#endif
#include <assert.h>

const char KEngine2D::StaticTransform::MetaName[] = "KEngine2D.StaticTransform";

KEngine2D::StaticTransform::StaticTransform(Point const & translation /* = Point::Origin() */, double radians /* = 0.0f */, Point scale /* = Point::Identity() */)
{
	mTranslation = translation;
	mRotation = radians;
	mScale = scale;
    UpdateMatrix();
}

KEngine2D::Point KEngine2D::StaticTransform::GetTranslation() const
{
	return mTranslation;
}

double KEngine2D::StaticTransform::GetRotation() const
{
	return mRotation;
}

KEngine2D::Point KEngine2D::StaticTransform::GetScale() const
{
	return mScale;
}

const KEngine2D::Matrix& KEngine2D::StaticTransform::GetAsMatrix() const
{
    return mMatrix;
}

void KEngine2D::StaticTransform::SetTranslation( Point const & translation )
{
	mTranslation = translation;
    UpdateMatrix();
}

void KEngine2D::StaticTransform::SetRotation( double rotation )
{
	mRotation = rotation;
    UpdateMatrix();
}

void KEngine2D::StaticTransform::SetScale(Point const& scale )
{
	mScale = scale;
    UpdateMatrix();
}

KEngine2D::StaticTransform const & KEngine2D::StaticTransform::Identity()
{
	static StaticTransform identity(Point::Origin(), 0.0f, Point::Identity());
	return identity;
}

KEngine2D::StaticTransform& KEngine2D::StaticTransform::operator=(Transform const& other)
{
    mTranslation = other.GetTranslation();
    mRotation = other.GetRotation();
    mScale = other.GetScale();
    UpdateMatrix();
    return *this;
}

void KEngine2D::StaticTransform::UpdateMatrix()
{
    float sinTheta = sin(mRotation);
    float cosTheta = cos(mRotation);
    float xScale = mScale.x;
    float yScale = mScale.y;
    mMatrix.data[0][0] = xScale * cosTheta;
    mMatrix.data[0][1] = - yScale * sinTheta;
    mMatrix.data[0][2] = 0;
    mMatrix.data[0][3] = mTranslation.x;
    mMatrix.data[1][0] = xScale * sinTheta;
    mMatrix.data[1][1] = yScale * cosTheta;
    mMatrix.data[1][2] = 0;
    mMatrix.data[1][3] = mTranslation.y;
    mMatrix.data[2][0] = 0.0f;
    mMatrix.data[2][1] = 0.0f;
    mMatrix.data[2][2] = 1.0f;
    mMatrix.data[2][3] = 0.0f;
    mMatrix.data[3][0] = 0.0f;
    mMatrix.data[3][1] = 0.0f;
    mMatrix.data[3][2] = 0.0f;
    mMatrix.data[3][3] = 1.0f;
}

KEngine2D::TransformLibrary::TransformLibrary()
{
}

KEngine2D::TransformLibrary::~TransformLibrary()
{
    Deinit();
}

void KEngine2D::TransformLibrary::Init(KEngineCore::LuaScheduler* scheduler, KEngineCore::TweenSystem * tweenSystem)
{
    assert(mScheduler == nullptr);
    mScheduler = scheduler;
    mTweenSystem = tweenSystem;
    RegisterLibrary(scheduler->GetMainState());
}

void KEngine2D::TransformLibrary::Deinit()
{
    mScheduler = nullptr;
}

void KEngine2D::TransformLibrary::RegisterLibrary(lua_State* luaState, char const* name)
{
    mTransformLuaWrapping.Init(StaticTransform::MetaName);
    auto luaopen_transforms = [](lua_State* luaState) {

        auto createMoveHorizontalBy = [](lua_State* luaState) {
            TransformLibrary* transformLib = (TransformLibrary*)lua_touserdata(luaState, lua_upvalueindex(1));
            KEngineCore::LuaScheduler* scheduler = transformLib->mScheduler;
            KEngine2D::StaticTransform* transform = transformLib->mTransformLuaWrapping.Unwrap(luaState, 1);
            double distance = luaL_checknumber(luaState, 2);

            KEngineCore::TweenTo<double>* tween = new (lua_newuserdata(luaState, sizeof(KEngineCore::TweenTo<double>))) KEngineCore::TweenTo<double>;
            luaL_getmetatable(luaState, KEngineCore::Tween::MetaName);
            lua_setmetatable(luaState, -2);

            tween->Init(transformLib->mTweenSystem, &transform->mTranslation.y, transform->mTranslation.x + distance, transform, [transform]() {
                transform->UpdateMatrix();
            });

            return 1;
        };

        auto createMoveVerticalBy = [](lua_State* luaState) {
            TransformLibrary* transformLib = (TransformLibrary*)lua_touserdata(luaState, lua_upvalueindex(1));
            KEngineCore::LuaScheduler* scheduler = transformLib->mScheduler;
            KEngine2D::StaticTransform* transform = transformLib->mTransformLuaWrapping.Unwrap(luaState, 1);
            double distance = luaL_checknumber(luaState, 2);

            KEngineCore::TweenTo<double>* tween = new (lua_newuserdata(luaState, sizeof(KEngineCore::TweenTo<double>))) KEngineCore::TweenTo<double>;
            luaL_getmetatable(luaState, KEngineCore::Tween::MetaName);
            lua_setmetatable(luaState, -2);

            tween->Init(transformLib->mTweenSystem, &transform->mTranslation.y, transform->mTranslation.y + distance, transform, [transform]() {
                transform->UpdateMatrix();
            });


            return 1;
        };

        auto createMoveHorizontalTo = [](lua_State* luaState) {
            TransformLibrary* transformLib = (TransformLibrary*)lua_touserdata(luaState, lua_upvalueindex(1));
            KEngineCore::LuaScheduler* scheduler = transformLib->mScheduler;
            KEngine2D::StaticTransform* transform = transformLib->mTransformLuaWrapping.Unwrap(luaState, 1);
            double position = luaL_checknumber(luaState, 2);
     
            KEngineCore::TweenTo<double>* tween = new (lua_newuserdata(luaState, sizeof(KEngineCore::TweenTo<double>))) KEngineCore::TweenTo<double>;
            luaL_getmetatable(luaState, KEngineCore::Tween::MetaName);
            lua_setmetatable(luaState, -2);

            tween->Init(transformLib->mTweenSystem, &transform->mTranslation.x, position, transform, [transform]() {
                transform->UpdateMatrix();
            });

            return 1;
        };

        auto createMoveVerticalTo = [](lua_State* luaState) {
            TransformLibrary* transformLib = (TransformLibrary*)lua_touserdata(luaState, lua_upvalueindex(1));
            KEngineCore::LuaScheduler* scheduler = transformLib->mScheduler;
            KEngine2D::StaticTransform* transform = transformLib->mTransformLuaWrapping.Unwrap(luaState, 1);
            double position = luaL_checknumber(luaState, 2);

            KEngineCore::TweenTo<double>* tween = new (lua_newuserdata(luaState, sizeof(KEngineCore::TweenTo<double>))) KEngineCore::TweenTo<double>;
            luaL_getmetatable(luaState, KEngineCore::Tween::MetaName);
            lua_setmetatable(luaState, -2);

            tween->Init(transformLib->mTweenSystem, &transform->mTranslation.y, position, transform, [transform]() {
                transform->UpdateMatrix();
            });

            return 1;
        };

        auto createRotateBy = [](lua_State* luaState) {
            TransformLibrary* transformLib = (TransformLibrary*)lua_touserdata(luaState, lua_upvalueindex(1));
            KEngineCore::LuaScheduler* scheduler = transformLib->mScheduler;
            KEngine2D::StaticTransform* transform = transformLib->mTransformLuaWrapping.Unwrap(luaState, 1);
            double rotation = luaL_checknumber(luaState, 2);

            KEngineCore::TweenTo<double>* tween = new (lua_newuserdata(luaState, sizeof(KEngineCore::TweenTo<double>))) KEngineCore::TweenTo<double>;
            luaL_getmetatable(luaState, KEngineCore::Tween::MetaName);
            lua_setmetatable(luaState, -2);

            tween->Init(transformLib->mTweenSystem, &transform->mRotation, transform->mRotation + rotation, transform, [transform]() {
                transform->UpdateMatrix();
             });

            return 1;
        };

        auto createRotateTo = [](lua_State* luaState) {
            TransformLibrary* transformLib = (TransformLibrary*)lua_touserdata(luaState, lua_upvalueindex(1));
            KEngineCore::LuaScheduler* scheduler = transformLib->mScheduler;
            KEngine2D::StaticTransform* transform = transformLib->mTransformLuaWrapping.Unwrap(luaState, 1);
            double targetRotation = luaL_checknumber(luaState, 2);
            while (targetRotation - transform->mRotation > std::numbers::pi)
            {
                targetRotation -= 2.0 * std::numbers::pi;
            }
            while (targetRotation - transform->mRotation < -std::numbers::pi)
            {
                targetRotation += 2.0 * std::numbers::pi;
            }

            KEngineCore::TweenTo<double>* tween = new (lua_newuserdata(luaState, sizeof(KEngineCore::TweenTo<double>))) KEngineCore::TweenTo<double>;
            luaL_getmetatable(luaState, KEngineCore::Tween::MetaName);
            lua_setmetatable(luaState, -2);

            tween->Init(transformLib->mTweenSystem, &transform->mRotation, targetRotation, transform, [transform]() {
                transform->UpdateMatrix();
            });

            return 1;
        };


        auto createScaleHorizontalBy = [](lua_State* luaState) {
            TransformLibrary* transformLib = (TransformLibrary*)lua_touserdata(luaState, lua_upvalueindex(1));
            KEngineCore::LuaScheduler* scheduler = transformLib->mScheduler;
            KEngine2D::StaticTransform* transform = transformLib->mTransformLuaWrapping.Unwrap(luaState, 1);
            double scale = luaL_checknumber(luaState, 2);

            KEngineCore::TweenTo<double>* tween = new (lua_newuserdata(luaState, sizeof(KEngineCore::TweenTo<double>))) KEngineCore::TweenTo<double>;
            luaL_getmetatable(luaState, KEngineCore::Tween::MetaName);
            lua_setmetatable(luaState, -2);

            tween->Init(transformLib->mTweenSystem, &transform->mScale.x, transform->mScale.x * scale, transform, [transform]() {
                transform->UpdateMatrix();
            });

            return 1;
        };

        auto createScaleVerticalBy = [](lua_State* luaState) {
            TransformLibrary* transformLib = (TransformLibrary*)lua_touserdata(luaState, lua_upvalueindex(1));
            KEngineCore::LuaScheduler* scheduler = transformLib->mScheduler;
            KEngine2D::StaticTransform* transform = transformLib->mTransformLuaWrapping.Unwrap(luaState, 1);
            double scale = luaL_checknumber(luaState, 2);

            KEngineCore::TweenTo<double>* tween = new (lua_newuserdata(luaState, sizeof(KEngineCore::TweenTo<double>))) KEngineCore::TweenTo<double>;
            luaL_getmetatable(luaState, KEngineCore::Tween::MetaName);
            lua_setmetatable(luaState, -2);

            tween->Init(transformLib->mTweenSystem, &transform->mScale.y, transform->mScale.y + scale, transform, [transform]() {
                transform->UpdateMatrix();
            });

            return 1;
        };



        const luaL_Reg transformLibrary[] = {
            {"createMoveHorizontal", createMoveHorizontalBy},
            {"createMoveVertical", createMoveVerticalBy},
            {"createMoveToHorizontal", createMoveHorizontalTo},
            {"createMoveToVertical", createMoveVerticalTo},
            {"createRotate", createRotateBy},
            {"createRotateTo", createRotateTo},
            {"createScaleHorizontal", createScaleHorizontalBy},
            {"createScaleVertical", createScaleVerticalBy},
            {nullptr, nullptr}
        };

        CreateEmptyMetaTableForClass<StaticTransform, StaticTransform::MetaName>(luaState);

        luaL_newlibtable(luaState, transformLibrary);
        lua_pushvalue(luaState, lua_upvalueindex(1));
        luaL_setfuncs(luaState, transformLibrary, 1);
        return 1;
    };

    PreloadLibrary(luaState, name, luaopen_transforms);
}

const KEngineCore::LuaWrapping<KEngine2D::StaticTransform>& KEngine2D::TransformLibrary::GetTransformWrapping() const
{
    return mTransformLuaWrapping;
}
