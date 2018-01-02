#include "Physics2D.h"
#include <algorithm>

KEngine2D::PhysicalObject::PhysicalObject()
{
	mMass = 0.0f;
	mMechanics = 0;
}

KEngine2D::PhysicalObject::~PhysicalObject()
{
	Deinit();
}

void KEngine2D::PhysicalObject::Init( PhysicsSystem * physicsSystem, MechanicalTransform * mechanics, BoundingArea * collisionVolume, double mass )
{
	assert(physicsSystem != 0);
	assert(mechanics != 0);
	assert(mass >= 0.0f); //Not supporting zero mass
	mMechanics = mechanics;
	mCollisionVolume = collisionVolume;
	mMass = mass;
	mPhysicsSystem = physicsSystem;
	physicsSystem->AddPhysicalObject(this);
}

void KEngine2D::PhysicalObject::Deinit()
{
	if (mPhysicsSystem != nullptr)
	{
		mPhysicsSystem->RemovePhysicalObject(this);
	}
	mPhysicsSystem = nullptr;
	mMass = 0.0f;
	mMechanics = nullptr;
	mCollisionVolume = nullptr;
}

double KEngine2D::PhysicalObject::GetMass() const
{
	return mMass;
}

void KEngine2D::PhysicalObject::SetMass( double mass )
{
	mMass = mass;
}


double KEngine2D::PhysicalObject::GetMomentOfInertia() const
{
	return mCollisionVolume->GetAreaMomentOfInertia() * GetMass();
}


double KEngine2D::PhysicalObject::GetEnergy() const
{
	Point linearVelocity = mMechanics->GetVelocity();
	double angularVelocity = mMechanics->GetAngularVelocity();
	return 0.5f * ((GetMass() * DotProduct(linearVelocity, linearVelocity)) + (GetMomentOfInertia() * (angularVelocity * angularVelocity)));
}

KEngine2D::Point KEngine2D::PhysicalObject::GetVelocity( KEngine2D::Point const & offset /*= KEngine2D::Point::Origin()*/ ) const
{
	double angularVelocity = mMechanics->GetAngularVelocity();
	//Technically tangentialVelocity, simplified from definition of angular velocity as cross product of vector along axis of rotation and radius
	KEngine2D::Point linearVelocity = {-angularVelocity * offset.y, angularVelocity * offset.x};
	linearVelocity += mMechanics->GetVelocity();
	return linearVelocity;
}

void KEngine2D::PhysicalObject::ApplyImpulse( KEngine2D::Point const & impulse, KEngine2D::Point const & offset /*= KEngine2D::Point::Origin()*/ )
{
	//Decompose the impulse vector into the component parallel to the offset (which will be applied directly to velocity)
	//and the component perpendicular to the offset (which will be applied to angular velocity)
	KEngine2D::Point deltaVelocity = impulse;
	double deltaAngularVelocity = 0.3f * ((offset.x*impulse.y) - (offset.y*impulse.x)); //.3 is a complete hack, and doesn't really work
	
	if (offset.x != 0.0f || offset.y != 0.0f)
	{
		deltaVelocity = KEngine2D::Project(offset, impulse);


/*
		//Very confused what this math is
		
	
		KEngine2D::Point deltaTangentialVelocity = impulse;
		deltaTangentialVelocity -= deltaVelocity;
		//Again, make sure not to divide by zero
		if (offset.x != 0.0f)
		{
			deltaAngularVelocity += deltaTangentialVelocity.y / offset.x; 
		}
		if (offset.y != 0.0f)
		{
			deltaAngularVelocity -= deltaTangentialVelocity.x / offset.y;
		} */
	}

	double invertedMass = 1.0f / mMass; //Safe because of assert in Init
	double invertedMomentOfInertia = 1 / GetMomentOfInertia();

	deltaVelocity *= invertedMass;
	deltaAngularVelocity *= invertedMomentOfInertia;

	KEngine2D::Point velocity = mMechanics->GetVelocity();
	double angularVelocity = mMechanics->GetAngularVelocity();

	velocity += deltaVelocity;
	angularVelocity += deltaAngularVelocity;

	mMechanics->SetVelocity(velocity);
	mMechanics->SetAngularVelocity(angularVelocity);	
}

bool KEngine2D::PhysicalObject::CheckAndResolveCollision( PhysicalObject & other )
{
	CollisionInfo possibleCollision = mCollisionVolume->Collides(*other.mCollisionVolume);
	if (possibleCollision.collides) {
		Point offset = possibleCollision.collisionPoint;
		offset -= mMechanics->GetTranslation();
		Point otherOffset = possibleCollision.collisionPoint;
		otherOffset -= other.mMechanics->GetTranslation();
		Point collisionNormal = possibleCollision.collisionNormal;
		double mass = GetMass();
		double otherMass = other.GetMass();
		double momentOfInertia = GetMomentOfInertia();
		double otherMomentOfInertia = other.GetMomentOfInertia();
		
		KEngine2D::Point velocity = KEngine2D::Project(collisionNormal, GetVelocity(offset));
		KEngine2D::Point otherVelocity = KEngine2D::Project(collisionNormal, other.GetVelocity(otherOffset));

		double oldImpulseCoefficient = (2 * mass * otherMass) / (mass + otherMass); //masses asserted positive, total can't be zero
		double offsetCrossNormal = (offset.x * collisionNormal.y) - (offset.y * collisionNormal.x);
		double otherOffsetCrossNormal = (otherOffset.x * collisionNormal.y) - (otherOffset.y * collisionNormal.x);

		double impulseCoefficient = 2 / ((1 / mass) + (1 / otherMass) + (offsetCrossNormal / momentOfInertia) + (otherOffsetCrossNormal / otherMomentOfInertia));
		KEngine2D::Point impulse = otherVelocity;
		impulse -= velocity;
		impulse *= impulseCoefficient;
		KEngine2D::Point otherImpulse = -impulse;
		float kinetic1 = GetEnergy() + other.GetEnergy();
		ApplyImpulse(impulse, offset);
		other.ApplyImpulse(otherImpulse, otherOffset);
		float kinetic2 = GetEnergy() + other.GetEnergy();
		//assert(kinetic2 < 1.1 * kinetic1 && kinetic1 < 1.1 * kinetic2);
		return true;
	}
	return false;
}

bool KEngine2D::PhysicalObject::CheckAndResolveCollision( KEngine2D::BoundaryLine const & other )
{
	CollisionInfo possibleCollision = mCollisionVolume->Collides(other);
	if (possibleCollision.collides)
	{
		Point offset = possibleCollision.collisionPoint;
		offset -= mMechanics->GetTranslation();
		Point collisionNormal = possibleCollision.collisionNormal;
		Point deltaVelocity = -GetVelocity(offset);
		Point impulse = KEngine2D::Project(collisionNormal, deltaVelocity, true);
		impulse *= (2.0f * GetMass());

		float kinetic1 = GetEnergy();
		ApplyImpulse(impulse, offset);
		float kinetic2 = GetEnergy();
		//assert(kinetic2 < 1.5 * kinetic1 && kinetic1 < 1.5 * kinetic2);
		return true;
	}
	return false;
}

KEngine2D::PhysicsSystem::PhysicsSystem()
{
	
}

KEngine2D::PhysicsSystem::~PhysicsSystem()
{
	Deinit();
}

void KEngine2D::PhysicsSystem::Init()
{
	
}

void KEngine2D::PhysicsSystem::Deinit()
{
	mBoundaries.clear();
	mPhysicalObjects.clear();
}

void KEngine2D::PhysicsSystem::Update( double fTime )
{
	for (auto it = mPhysicalObjects.begin(); it != mPhysicalObjects.end(); it++)
	{
		PhysicalObject * physicalObject = *it;
		bool foundCollision = false;
		for (auto boundaryIt = mBoundaries.begin(); boundaryIt != mBoundaries.end() && !foundCollision; boundaryIt++)
		{
			KEngine2D::BoundaryLine * boundaryLine = *boundaryIt;
			foundCollision = physicalObject->CheckAndResolveCollision(*boundaryLine);
		}
		auto otherIt = it;
		for ( otherIt++; otherIt != mPhysicalObjects.end() && !foundCollision; otherIt++)
		{
			PhysicalObject * otherPhysicalObject = *otherIt;
			foundCollision = physicalObject->CheckAndResolveCollision(*otherPhysicalObject);
		}
	}
}

void KEngine2D::PhysicsSystem::AddPhysicalObject( PhysicalObject * physicalObject )
{
	mPhysicalObjects.push_back(physicalObject);
}

void KEngine2D::PhysicsSystem::RemovePhysicalObject( PhysicalObject * physicalObject )
{
	mPhysicalObjects.erase(remove(mPhysicalObjects.begin(), mPhysicalObjects.end(), physicalObject));
}

void KEngine2D::PhysicsSystem::AddBoundary( KEngine2D::BoundaryLine * boundary )
{
	mBoundaries.push_back(boundary);
}

void KEngine2D::PhysicsSystem::RemoveBoundary( KEngine2D::BoundaryLine * boundary )
{
    mBoundaries.erase(remove(mBoundaries.begin(), mBoundaries.end(), boundary));
}
