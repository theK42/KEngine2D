#pragma  once

#include "Transform2D.h"
#include "StaticTransform2D.h"
#include "Updater.h"

namespace KEngine2D
{
	class MechanicalTransform : public Transform
	{
	public:
		MechanicalTransform();
		~MechanicalTransform();
		void Init(StaticTransform const & currentTransform = StaticTransform::Identity(), Point const & velocity = Point::Origin(), double angularVelocity = 0.0f);
		void Deinit();

		void Update(double fTime);

		virtual Point GetTranslation() const;
		virtual double GetRotation() const;
		virtual double GetScale() const;

		void SetCurrentTransform(StaticTransform const & currentTransform);
		void SetVelocity(Point const & velocity);
		void SetAngularVelocity(double angularVelocity);

		Point const & GetVelocity() const;
		double GetAngularVelocity() const;

	private:
		StaticTransform mCurrentTransform;
		Point			mVelocity;
		double			mAngularVelocity;
		
	};

	class UpdatingMechanicalTransform : public KEngineCore::Updating<MechanicalTransform>
	{
	public:
		void Init(KEngineCore::Updater<MechanicalTransform> * updater, StaticTransform const & currentTransform = KEngine2D::StaticTransform::Identity(), KEngine2D::Point const & velocity = KEngine2D::Point::Origin(), double angularVelocity = 0.0f);
	};

	class MechanicsUpdater : public  KEngineCore::Updater<MechanicalTransform> {};
}