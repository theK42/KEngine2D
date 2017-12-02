#include <cassert>
#include "HierarchicalTransform2D.h"

KEngine2D::HierarchicalTransform::HierarchicalTransform()
{
	mParent = nullptr;

	mLocalTransform = StaticTransform::Identity();
	mGlobalTransform = StaticTransform::Identity();
}

KEngine2D::HierarchicalTransform::~HierarchicalTransform()
{
	Deinit();
}

void KEngine2D::HierarchicalTransform::Init(Transform * parent, StaticTransform const & localTransform /* = StaticTransform::Identity */)
{
	assert(parent != nullptr);
	mParent = parent;
	mLocalTransform = localTransform;
	Update(0.0f); ///Sets up mGlobalTransform
}

void KEngine2D::HierarchicalTransform::Deinit()
{
	mParent = nullptr;
}

void KEngine2D::HierarchicalTransform::Update( double fTime )
{
	assert(mParent != nullptr);
	Point globalTranslation = mParent->Apply(mLocalTransform.GetTranslation());
	double globalRotation = mParent->GetRotation() + mLocalTransform.GetRotation();
	double globalScale = mParent->GetScale() * mLocalTransform.GetScale();
	mGlobalTransform.SetTranslation(globalTranslation);
	mGlobalTransform.SetRotation(globalRotation);
	mGlobalTransform.SetScale(globalScale);
}

KEngine2D::Point KEngine2D::HierarchicalTransform::GetTranslation() const
{
	assert(mParent != nullptr);
	return mGlobalTransform.GetTranslation();
}

double KEngine2D::HierarchicalTransform::GetRotation() const
{
	assert(mParent != nullptr);
	return mGlobalTransform.GetRotation();
}

double KEngine2D::HierarchicalTransform::GetScale() const
{
	assert(mParent != nullptr);
	return mGlobalTransform.GetScale();
}

const KEngine2D::Matrix& KEngine2D::HierarchicalTransform::GetAsMatrix() const
{
    assert(mParent != nullptr);
    return mGlobalTransform.GetAsMatrix();
}

KEngine2D::StaticTransform const & KEngine2D::HierarchicalTransform::GetLocalTransform() const
{
	assert(mParent != nullptr);
	return mLocalTransform;
}

void KEngine2D::HierarchicalTransform::SetLocalTransform( StaticTransform const & localTransform )
{
	assert(mParent != nullptr);
	mLocalTransform = localTransform;
	Update(0.0f);
}

void KEngine2D::UpdatingHierarchicalTransform::Init( KEngineCore::Updater<HierarchicalTransform> * updater, Transform * parent, StaticTransform const & localTransform /*= StaticTransform::Identity()*/ )
{
	KEngineCore::Updating<HierarchicalTransform>::Init(updater);
    HierarchicalTransform::Init(parent, localTransform);
	Start();
}
