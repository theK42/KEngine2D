#include <cassert>
#include "HierarchicalTransform2D.h"

KEngine2D::HierarchicalTransform::HierarchicalTransform()
{
	mParent = 0;

	mLocalTransform = StaticTransform::Identity();
	mGlobalTransform = StaticTransform::Identity();
}

KEngine2D::HierarchicalTransform::~HierarchicalTransform()
{
	Deinit();
}

void KEngine2D::HierarchicalTransform::Init(Transform * parent, StaticTransform const & localTransform /* = StaticTransform::Identity */)
{
	assert(parent != 0);
	mParent = parent;
	mLocalTransform = localTransform;
	Update(0.0f); ///Sets up mGlobalTransform
}

void KEngine2D::HierarchicalTransform::Deinit()
{
	mParent = 0;
}

void KEngine2D::HierarchicalTransform::Update( double fTime )
{
	assert(mParent != 0);
	Point globalTranslation = mParent->Apply(mLocalTransform.GetTranslation());
	double globalRotation = mParent->GetRotation() + mLocalTransform.GetRotation();
	double globalScale = mParent->GetScale() * mLocalTransform.GetScale();
	mGlobalTransform.SetTranslation(globalTranslation);
	mGlobalTransform.SetRotation(globalRotation);
	mGlobalTransform.SetScale(globalScale);
}

KEngine2D::Point KEngine2D::HierarchicalTransform::GetTranslation() const
{
	assert(mParent != 0);
	return mGlobalTransform.GetTranslation();
}

double KEngine2D::HierarchicalTransform::GetRotation() const
{
	assert(mParent != 0);
	return mGlobalTransform.GetRotation();
}

double KEngine2D::HierarchicalTransform::GetScale() const
{
	assert(mParent != 0);
	return mGlobalTransform.GetScale();
}

KEngine2D::StaticTransform const & KEngine2D::HierarchicalTransform::GetLocalTransform() const
{
	assert(mParent != 0);
	return mLocalTransform;
}

void KEngine2D::HierarchicalTransform::SetLocalTransform( StaticTransform const & localTransform )
{
	assert(mParent != 0);
	mLocalTransform = localTransform;
	Update(0.0f);
}

void KEngine2D::UpdatingHierarchicalTransform::Init( KEngineCore::Updater<HierarchicalTransform> * updater, Transform * parent, StaticTransform const & localTransform /*= StaticTransform::Identity()*/ )
{
	KEngineCore::Updating<HierarchicalTransform>::Init(updater);
	mUpdatable.Init(parent, localTransform);
	Start();
}
