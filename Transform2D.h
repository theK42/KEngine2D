#pragma once
#include "LuaScheduler.h"

namespace KEngine2D
{
    struct Matrix
    {
        float  data[4][4];
        static Matrix const & Identity();
    };
    
	struct Point
	{
		double x;
		double y;
		static Point const& Origin();
		static Point const& Identity();

		Point & operator+=(Point const & other);
		Point & operator-=(Point const & other);
		Point & operator*=(Point const & other);
		Point & operator*=(double const & scalar);
		Point & operator/=(double const & scalar);
        Point & operator*=(Matrix const & transform);
		Point operator-();
		Point operator+(Point const & other);
		Point operator-(Point const& other);
		Point operator*(Point const& other);
		Point operator*(double const& scalar);
	};

	float DotProduct(Point const & vec1, Point const & vec2);
	float PseudoCrossProduct(Point const & vec1, Point const & vec2);
	Point PseudoCrossProduct(Point const & vec1, float scalar);
	Point Project(Point const & axis, Point const & vec, bool positiveOnly = false);




	class Transform
	{
	public:
		virtual ~Transform() {}
		virtual Point GetTranslation() const = 0;
		virtual double GetRotation() const = 0;
		virtual Point GetScale() const = 0;
        virtual const Matrix& GetAsMatrix() const = 0;

		virtual Point LocalToGlobal(Point const & point, bool asVector = false) const;
		virtual Point GlobalToLocal(Point const & point) const;
	};
}

namespace KEngineCore
{
	template<>
	inline void pushToLua<KEngine2D::Point>(lua_State* state, KEngine2D::Point t)
	{
		lua_checkstack(state, 2);
		lua_newtable(state);
		lua_pushnumber(state, t.x);
		lua_setfield(state, -2, "x");
		lua_pushnumber(state, t.y);
		lua_setfield(state, -2, "y");
	}
}