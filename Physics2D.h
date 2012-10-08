#pragma once
#include <vector>
#include "MechanicalTransform2D.h"
#include "Boundaries2D.h"

namespace KEngine2D
{
	class PhysicsSystem;
	
	class PhysicalObject
	{
	public:

		PhysicalObject();
		~PhysicalObject();

		void Init(PhysicsSystem * physicsSystem, MechanicalTransform * mechanics, std::vector<KEngine2D::BoundingCircle *> collisionVolumes, double mass);
		void Deinit();

		double GetMass() const;
		void SetMass(double mass);

		KEngine2D::Point GetVelocity(KEngine2D::Point const & offset = KEngine2D::Point::Origin()) const;
		void ApplyImpulse(KEngine2D::Point const & impulse, KEngine2D::Point const & offset = KEngine2D::Point::Origin());

		bool CheckAndResolveCollision(PhysicalObject & other);
		bool CheckAndResolveCollision(KEngine2D::BoundaryLine const & other);

	private:
		double mMass;
		MechanicalTransform * mMechanics;
		PhysicsSystem * mPhysicsSystem;
		std::vector<KEngine2D::BoundingCircle *> mCollisionVolumes;
	};


	class PhysicsSystem
	{
	public:
		PhysicsSystem();
		~PhysicsSystem();

		void Init();
		void Deinit();

		void Update(double fTime);

		void AddPhysicalObject(PhysicalObject * physicalObject);
		void RemovePhysicalObject(PhysicalObject * physicalObject);

		void AddBoundary(KEngine2D::BoundaryLine * boundary);
		void RemoveBoundary(KEngine2D::BoundaryLine * boundary);

	private:
		std::list<PhysicalObject *> mPhysicalObjects;
		std::list<KEngine2D::BoundaryLine *> mBoundaries;
	};

}
