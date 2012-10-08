#include "StaticTransform2D.h"

KEngine2D::StaticTransform::StaticTransform(Point const & translation /* = Point::Origin() */, double radians /* = 0.0f */, double scale /* = 1.0f */)
{
	mTranslation = translation;
	mRotation = radians;
	mScale = scale;
}

KEngine2D::Point KEngine2D::StaticTransform::GetTranslation() const
{
	return mTranslation;
}

double KEngine2D::StaticTransform::GetRotation() const
{
	return mRotation;
}

double KEngine2D::StaticTransform::GetScale() const
{
	return mScale;
}

void KEngine2D::StaticTransform::SetTranslation( Point const & translation )
{
	mTranslation = translation;
}

void KEngine2D::StaticTransform::SetRotation( double rotation )
{
	mRotation = rotation;
}

void KEngine2D::StaticTransform::SetScale( double scale )
{
	mScale = scale;
}

KEngine2D::StaticTransform const & KEngine2D::StaticTransform::Identity()
{
	static StaticTransform identity(Point::Origin(), 0.0f, 1.0f);
	return identity;

}