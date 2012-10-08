#pragma  once

#include "Transform2D.h"
#include "StaticTransform2D.h"
#include "Updater.h"

namespace KEngine2D
{
	class HierarchicalTransform : public Transform
	{
	public:
		HierarchicalTransform();
		~HierarchicalTransform();
		void Init(Transform * parent, StaticTransform const & localTransform = StaticTransform::Identity());
		void Deinit();

		void Update(double fTime);

		virtual Point GetTranslation() const;
		virtual double GetRotation() const;
		virtual double GetScale() const;

		StaticTransform const & GetLocalTransform() const;
		void SetLocalTransform(StaticTransform const & localTransform);

	private:
		Transform const * mParent;
		StaticTransform mLocalTransform;
		StaticTransform mGlobalTransform;
	};


	class UpdatingHierarchicalTransform : public KEngineCore::Updating<HierarchicalTransform>
	{
	public:
		void Init(KEngineCore::Updater<HierarchicalTransform> * updater, Transform * parent, StaticTransform const & localTransform = StaticTransform::Identity());
	};

	class HierarchyUpdater : public KEngineCore::Updater<HierarchicalTransform> {};
}