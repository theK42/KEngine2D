#include <cassert>
#include "Boundaries2D.h"

KEngine2D::BoundaryLine::BoundaryLine()
{
	mXCoefficient = 0.0f;
	mYCoefficient = 0.0f;
	mConstantCoefficient = 0.0f;
}

KEngine2D::BoundaryLine::~BoundaryLine()
{
	Deinit();
}

void KEngine2D::BoundaryLine::Init( double xCoefficient, double yCoefficient, double constantCoefficient )
{
	assert(xCoefficient != 0.0f || yCoefficient != 0.0f);
	mXCoefficient = xCoefficient;
	mYCoefficient = yCoefficient;
	mConstantCoefficient = constantCoefficient;
}

void KEngine2D::BoundaryLine::Deinit()
{
	mXCoefficient = 0.0f;
	mYCoefficient = 0.0f;
	mConstantCoefficient = 0.0f;
}

double KEngine2D::BoundaryLine::GetSignedDistance( Point const & point ) const
{
	assert(mXCoefficient != 0.0f || mYCoefficient != 0.0f);
	double fDistance = mXCoefficient * point.x + mYCoefficient * point.y + mConstantCoefficient;
	return fDistance;
}

KEngine2D::Point KEngine2D::BoundaryLine::GetNormal() const
{
	Point normal = {mXCoefficient, mYCoefficient};
	return normal;
}

KEngine2D::BoundingCircle::BoundingCircle()
{
	mTransform = 0;
	mRadius = 0.0f;
}

KEngine2D::BoundingCircle::~BoundingCircle()
{
	Deinit();
}

void KEngine2D::BoundingCircle::Init( Transform * transform, double radius )
{
	assert(transform != 0);
	assert(radius >= 0.0f);
	mTransform = transform;
	mRadius = radius;
}

void KEngine2D::BoundingCircle::Deinit()
{
	mTransform = 0;
	mRadius = 0.0f;
}

double KEngine2D::BoundingCircle::GetRadius() const
{	
	assert(mTransform != 0);
	return mRadius * mTransform->GetScale();
}

KEngine2D::Point KEngine2D::BoundingCircle::GetCenter() const
{
	assert(mTransform != 0);
	return mTransform->GetTranslation();
}

bool KEngine2D::BoundingCircle::Collides( BoundingCircle const & other )
{
	double xDiff = other.GetCenter().x - GetCenter().x;
	double yDiff = other.GetCenter().y - GetCenter().y;
	double distance2 = (xDiff * xDiff) + (yDiff * yDiff);
	double minDistance = GetRadius() + other.GetRadius();
	double minDistance2 = minDistance * minDistance;  // Cheaper than sqrt
	return (distance2 <= minDistance2);
}

bool KEngine2D::BoundingCircle::Collides( BoundaryLine const & boundary )
{
	double distance = boundary.GetSignedDistance(GetCenter());
	double minDistance = GetRadius();
	return (distance <= minDistance);
}