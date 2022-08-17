#include "Transform2D.h"
#include "Transform2D.h"
#include <assert.h>
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

KEngine2D::Point & KEngine2D::Point::operator*=(double const & scalar)
{
	x *= scalar;
	y *= scalar;
	return *this;
}

KEngine2D::Point & KEngine2D::Point::operator/=(double const & scalar)
{
	assert(scalar != 0.0f);
	x /= scalar;
	y /= scalar;
	return *this;
}

KEngine2D::Point KEngine2D::Point::operator-()
{
	Point retVal = {-x, -y};
	return retVal;
}

KEngine2D::Point KEngine2D::Point::operator+(Point const & other)
{
	return{ x + other.x, y + other.y };
}

KEngine2D::Point KEngine2D::Point::operator-(Point const & other) 
{
	return{ x - other.x, y - other.y };
}

float KEngine2D::DotProduct(Point const & vec1, Point const & vec2)
{
	return (vec1.x * vec2.x) + (vec1.y * vec2.y);
}

//Cross Product is undefined, but this gets the z-value (magnitude) of the vector we'd get if these were 3d vectors
float KEngine2D::PseudoCrossProduct(Point const & vec1, Point const & vec2)
{
	return (vec1.x * vec2.y) - (vec1.y * vec2.x);
}

//Cross Product is even more undefined, but this gets the vector this were a 3d vector and the scalar was the z value of another 3d vector
KEngine2D::Point KEngine2D::PseudoCrossProduct(Point const & vec1, float scalar)
{
	return{ -1.0 * vec1.y * scalar, vec1.x * scalar }; 
}

KEngine2D::Point KEngine2D::Project( Point const & axis, Point const & vec, bool positiveOnly /*= false*/ )
{
	//Yes this does project on any length vector without square root.  Thanks for noticing.
	double dotProduct = DotProduct(vec, axis);
	double axisLength2 = DotProduct(axis, axis);
	double scalarResolute = axisLength2 != 0 ? dotProduct / axisLength2 : 0; ///Make sure not to divide by zero
	if (positiveOnly && scalarResolute < 0.0f)
	{
		scalarResolute = 0.0f;
	}

	KEngine2D::Point projection = axis;
	projection *= scalarResolute;
	return projection;
}

KEngine2D::Point KEngine2D::Transform::LocalToGlobal(Point const & point, bool asVector) const
{
	Point retVal = point;
	double scale = GetScale();
	double radians = GetRotation();
	Point translation = GetTranslation();
	if (!asVector)
	{
		retVal.x *= scale;
		retVal.y *= scale;
	}
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);
	retVal = { (retVal.x * cosTheta) - (retVal.y * sinTheta), (retVal.y * cosTheta) + (retVal.x * sinTheta) };
	if (!asVector)
	{
		retVal.x += translation.x;
		retVal.y += translation.y;
	}
	return retVal;
}

KEngine2D::Point KEngine2D::Transform::GlobalToLocal(Point const & point) const
{
	Point retVal;
	double radians = GetRotation();
	Point translation = GetTranslation();
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);
	retVal.x = ((point.x - translation.x) * cosTheta) + ((point.y - translation.y) * sinTheta);
	retVal.y = ((point.y - translation.y) * cosTheta) - ((point.x - translation.x) * sinTheta);
	return retVal;
}
