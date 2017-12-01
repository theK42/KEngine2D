#include "StaticTransform2D.h"
#include <cmath>

KEngine2D::StaticTransform::StaticTransform(Point const & translation /* = Point::Origin() */, double radians /* = 0.0f */, double scale /* = 1.0f */)
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

double KEngine2D::StaticTransform::GetScale() const
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

void KEngine2D::StaticTransform::SetScale( double scale )
{
	mScale = scale;
    UpdateMatrix();
}

KEngine2D::StaticTransform const & KEngine2D::StaticTransform::Identity()
{
	static StaticTransform identity(Point::Origin(), 0.0f, 1.0f);
	return identity;
}

void KEngine2D::StaticTransform::UpdateMatrix()
{
    float sinTheta = sin(mRotation);
    float cosTheta = cos(mRotation);
    float xScale = mScale;
    float yScale = mScale;
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
