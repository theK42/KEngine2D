#include "Physics2D.h"

KEngine2D::PhysicalObject::PhysicalObject()
{
	mMass = 0.0f;
	mMechanics = 0;
}

KEngine2D::PhysicalObject::~PhysicalObject()
{
	Deinit();
}

void KEngine2D::PhysicalObject::Init( PhysicsSystem * physicsSystem, MechanicalTransform * mechanics, std::vector<KEngine2D::BoundingCircle *> collisionVolumes, double mass )
{
	assert(physicsSystem != 0);
	assert(mechanics != 0);
	assert(mass >= 0.0f); //Not supporting zero mass
	mMechanics = mechanics;
	mCollisionVolumes = collisionVolumes;
	mMass = mass;
	mPhysicsSystem = physicsSystem;
	physicsSystem->AddPhysicalObject(this);
}

void KEngine2D::PhysicalObject::Deinit()
{
	if (mPhysicsSystem != 0)
	{
		mPhysicsSystem->RemovePhysicalObject(this);
	}
	mPhysicsSystem = 0;
	mMass = 0.0f;
	mMechanics = 0;
	mCollisionVolumes.clear();
}

double KEngine2D::PhysicalObject::GetMass() const
{
	return mMass;
}

void KEngine2D::PhysicalObject::SetMass( double mass )
{
	mMass = mass;
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
	double deltaAngularVelocity = 0.0f;

	if (offset.x != 0.0f || offset.y != 0.0f)
	{
		deltaVelocity = KEngine2D::Project(offset, impulse);

		KEngine2D::Point deltaTangentialVelocity = impulse;
		deltaTangentialVelocity -= deltaVelocity;
		//Again, make sure not to divide by zero
		if (offset.x != 0.0f)
		{
			deltaAngularVelocity = deltaTangentialVelocity.y / offset.x; 
		}
		else if (offset.y != 0.0f)
		{
			deltaAngularVelocity = - deltaTangentialVelocity.x / offset.y;
		} 
	}

	double invertedMass = 1.0f / mMass; //Safe because of assert in Init

	deltaVelocity *= invertedMass;
	deltaAngularVelocity *= invertedMass;

	KEngine2D::Point velocity = mMechanics->GetVelocity();
	double angularVelocity = mMechanics->GetAngularVelocity();

	velocity += deltaVelocity;
	angularVelocity += deltaAngularVelocity;

	mMechanics->SetVelocity(velocity);
	mMechanics->SetAngularVelocity(deltaAngularVelocity);	
}

bool KEngine2D::PhysicalObject::CheckAndResolveCollision( PhysicalObject & other )
{
	for (std::vector<KEngine2D::BoundingCircle *>::const_iterator it = mCollisionVolumes.begin(); it != mCollisionVolumes.end(); it++)
	{
		for (std::vector<KEngine2D::BoundingCircle *>::const_iterator otherIt = other.mCollisionVolumes.begin(); otherIt != other.mCollisionVolumes.end(); otherIt++)
		{
			KEngine2D::BoundingCircle * circle = *it;
			KEngine2D::BoundingCircle * otherCircle = *otherIt;

			if (circle->Collides(*otherCircle))	
			{
				KEngine2D::Point offset = circle->GetCenter();
				offset -= mMechanics->GetTranslation();

				KEngine2D::Point otherOffset = otherCircle->GetCenter();
				otherOffset -= other.mMechanics->GetTranslation();

				double mass = GetMass();
				double otherMass = other.GetMass();

				KEngine2D::Point collisionNormal = circle->GetCenter();
				collisionNormal -= otherCircle->GetCenter();

				KEngine2D::Point velocity = KEngine2D::Project(collisionNormal, GetVelocity(offset));
				KEngine2D::Point otherVelocity = KEngine2D::Project(collisionNormal, other.GetVelocity(otherOffset));

				double impulseCoefficient = (2 * mass * otherMass) / (mass + otherMass); //masses asserted positive, total can't be zero

				KEngine2D::Point impulse = otherVelocity;
				impulse -= velocity;
				impulse *= impulseCoefficient;

				KEngine2D::Point otherImpulse = -impulse;

				ApplyImpulse(impulse, offset);
				other.ApplyImpulse(otherImpulse, otherOffset);
				return true; //Only resolve the first collision detected between these rigid bodies.  With simplistic method used here, it's too dangerous to do more.
			}
		}
	}
	return false;
}

bool KEngine2D::PhysicalObject::CheckAndResolveCollision( KEngine2D::BoundaryLine const & other )
{
	for (std::vector<KEngine2D::BoundingCircle *>::const_iterator it = mCollisionVolumes.begin(); it != mCollisionVolumes.end(); it++)
	{
		KEngine2D::BoundingCircle * circle = *it;
		if (circle->Collides(other))
		{
			KEngine2D::Point offset = circle->GetCenter();
			offset -= mMechanics->GetTranslation();
			KEngine2D::Point collisionNormal = other.GetNormal();
			KEngine2D::Point deltaVelocity = -GetVelocity(offset);
			KEngine2D::Point impulse = KEngine2D::Project(collisionNormal, deltaVelocity, true);
			impulse *= (2 * GetMass());
			ApplyImpulse(impulse, offset);
			return true; ///Again, only resolve the first collision detected.  This will cause problems, but probably not as severe as could be caused by naively attempting to resolve all collisions.
		}
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
	for (std::list<PhysicalObject *>::iterator it = mPhysicalObjects.begin(); it != mPhysicalObjects.end(); it++)
	{
		PhysicalObject * physicalObject = *it;
		bool foundCollision = false;
		for (std::list<KEngine2D::BoundaryLine *>::const_iterator boundaryIt = mBoundaries.begin(); boundaryIt != mBoundaries.end() && !foundCollision; boundaryIt++)
		{
			KEngine2D::BoundaryLine * boundaryLine = *boundaryIt;
			foundCollision = physicalObject->CheckAndResolveCollision(*boundaryLine);
		}
		std::list<PhysicalObject *>::iterator otherIt = it;
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
	mPhysicalObjects.remove(physicalObject);
}

void KEngine2D::PhysicsSystem::AddBoundary( KEngine2D::BoundaryLine * boundary )
{
	mBoundaries.push_back(boundary);
}

void KEngine2D::PhysicsSystem::RemoveBoundary( KEngine2D::BoundaryLine * boundary )
{
	mBoundaries.remove(boundary);
}