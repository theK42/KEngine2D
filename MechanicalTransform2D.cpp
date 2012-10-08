#include <cassert>
#include "MechanicalTransform2D.h"

KEngine2D::MechanicalTransform::MechanicalTransform()
{
	mCurrentTransform = StaticTransform::Identity();
	mVelocity = Point::Origin();
	mAngularVelocity = 0.0f;
}

KEngine2D::MechanicalTransform::~MechanicalTransform()
{
	Deinit();
}

void KEngine2D::MechanicalTransform::Init( StaticTransform const & currentTransform /*= StaticTransform::Identity()*/, Point const & velocity /*= Point::Origin()*/, double angularVelocity /*= 0.0f*/ )
{
	mCurrentTransform = currentTransform;
	mVelocity = velocity;
	mAngularVelocity = angularVelocity;
}

void KEngine2D::MechanicalTransform::Deinit()
{
	mCurrentTransform = StaticTransform::Identity();
	mVelocity = Point::Origin();
	mAngularVelocity = 0.0f;
}

void KEngine2D::MechanicalTransform::Update( double fTime )
{
	Point currentTranslation = mCurrentTransform.GetTranslation();
	Point scaledVelocity = mVelocity;
	scaledVelocity *= fTime;
	currentTranslation += scaledVelocity;
	double currentRotation = mCurrentTransform.GetRotation();
	currentRotation += fTime * mAngularVelocity;
	mCurrentTransform.SetTranslation(currentTranslation);
	mCurrentTransform.SetRotation(currentRotation);
}

KEngine2D::Point KEngine2D::MechanicalTransform::GetTranslation() const
{
	return mCurrentTransform.GetTranslation();
}

double KEngine2D::MechanicalTransform::GetRotation() const
{
	return mCurrentTransform.GetRotation();
}

double KEngine2D::MechanicalTransform::GetScale() const
{
	return mCurrentTransform.GetScale();
}

void KEngine2D::MechanicalTransform::SetCurrentTransform( StaticTransform const & currentTransform )
{
	mCurrentTransform = currentTransform;
}

void KEngine2D::MechanicalTransform::SetVelocity( Point const & veloctiy )
{
	mVelocity = veloctiy;
}

void KEngine2D::MechanicalTransform::SetAngularVelocity( double angularVelocity )
{
	mAngularVelocity = angularVelocity;
}

KEngine2D::Point const & KEngine2D::MechanicalTransform::GetVelocity() const
{
	return mVelocity;
}

double KEngine2D::MechanicalTransform::GetAngularVelocity() const
{
	return mAngularVelocity;
}

void KEngine2D::UpdatingMechanicalTransform::Init( KEngineCore::Updater<MechanicalTransform> * updater, StaticTransform const & currentTransform /*= StaticTransform::Identity()*/, Point const & velocity /*= Point::Origin()*/, double angularVelocity /*= 0.0f*/ )
{
	KEngineCore::Updating<MechanicalTransform>::Init(updater);
	mUpdatable.Init(currentTransform, velocity, angularVelocity);
	Start();
}