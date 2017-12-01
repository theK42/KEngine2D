#include "Transform2D.h"
#include <cmath>

const KEngine2D::Matrix & KEngine2D::Matrix::Identity()
{
    static Matrix identity = {{
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {0,0,0,1}
    }};
    return identity;
}

KEngine2D::Point const & KEngine2D::Point::Origin()
{
	static Point origin = {0.0f, 0.0f};
	return origin;
}

KEngine2D::Point & KEngine2D::Point::operator+=( Point const & other )
{
	x += other.x;
	y += other.y;
	return *this;
}

KEngine2D::Point & KEngine2D::Point::operator-=( Point const & other )
{
	x -= other.x;
	y -= other.y;
	return *this;
}

KEngine2D::Point & KEngine2D::Point::operator*=( double const & scalar )
{
	x *= scalar;
	y *= scalar;
	return *this;
}

KEngine2D::Point KEngine2D::Point::operator-()
{
	Point retVal = {-x, -y};
	return retVal;
}

KEngine2D::Point KEngine2D::Project( Point const & axis, Point const & vec, bool positiveOnly /*= false*/ )
{
	double dotProduct = (vec.x * axis.x) + (vec.y * axis.y);
	double axisLength2 = (axis.x * axis.x) + (axis.y * axis.y);
	double scalarResolute = axisLength2 != 0 ? dotProduct / axisLength2 : 0; ///Make sure not to divide by zero

	if (positiveOnly && scalarResolute < 0.0f)
	{
		scalarResolute = 0.0f;
	}

	KEngine2D::Point projection = axis;
	projection *= scalarResolute;
	return projection;
}

KEngine2D::Point KEngine2D::Transform::Apply( Point const & point ) const
{
	Point retVal = point;
	double scale = GetScale();
	double radians = GetRotation();
	Point translation = GetTranslation();
	retVal.x *= scale;
	retVal.y *= scale;
	retVal.x = point.x * cos(radians) - point.y * sin(radians);
	retVal.y = point.y * cos(radians) + point.x * sin(radians);
	retVal.x += translation.x;
	retVal.y += translation.y;
	return retVal;
}

