#include "Boundaries2D.h"
#include <cassert>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

KEngine2D::BoundaryLine::BoundaryLine()
{
	mXCoefficient = 0.0f;
	mYCoefficient = 0.0f;
	mConstantCoefficient = 0.0f;
}

KEngine2D::BoundaryLine::~BoundaryLine()
{
	Deinit();
}

void KEngine2D::BoundaryLine::Init( double xCoefficient, double yCoefficient, double constantCoefficient )
{
	assert(xCoefficient != 0.0f || yCoefficient != 0.0f);
	mXCoefficient = xCoefficient;
	mYCoefficient = yCoefficient;
	mConstantCoefficient = constantCoefficient;
}

void KEngine2D::BoundaryLine::Deinit()
{
	mXCoefficient = 0.0f;
	mYCoefficient = 0.0f;
	mConstantCoefficient = 0.0f;
}

double KEngine2D::BoundaryLine::GetSignedDistance( Point const & point ) const
{
	assert(mXCoefficient != 0.0f || mYCoefficient != 0.0f);
	double fDistance = mXCoefficient * point.x + mYCoefficient * point.y + mConstantCoefficient;
	return fDistance;
}

KEngine2D::Point KEngine2D::BoundaryLine::GetNormal() const
{
	Point normal = {mXCoefficient, mYCoefficient};
	return normal;
}

KEngine2D::BoundingCircle::BoundingCircle()
{
	mTransform = 0;
	mRadius = 0.0f;
}

KEngine2D::BoundingCircle::~BoundingCircle()
{
	Deinit();
}

void KEngine2D::BoundingCircle::Init( Transform * transform, double radius )
{
	assert(transform != 0);
	assert(radius >= 0.0f);
	mTransform = transform;
	mRadius = radius;
}

void KEngine2D::BoundingCircle::Deinit()
{
	mTransform = 0;
	mRadius = 0.0f;
}

double KEngine2D::BoundingCircle::GetRadius() const
{	
	assert(mTransform != 0);
	assert(mTransform->GetScale().x == mTransform->GetScale().y); // Currently only supporting uniform scale, will need to revisit
	return mRadius * mTransform->GetScale().x;
}

KEngine2D::Point KEngine2D::BoundingCircle::GetCenter() const
{
	assert(mTransform != 0);
	return mTransform->GetTranslation();
}

double KEngine2D::BoundingCircle::GetArea() const
{
	return M_PI * pow(GetRadius(), 2);
}

double KEngine2D::BoundingCircle::GetAreaMomentOfInertia() const
{
	return M_PI_4 * pow(GetRadius(), 4); //M_PI_4 is pi/4
}

std::pair<KEngine2D::Point, KEngine2D::Point> KEngine2D::BoundingCircle::GetAxisAlignedBoundingBox() const
{
	Point center = GetCenter();
	double radius = GetRadius();
	Point diff = { radius, radius };
	return { center - diff, center + diff };
}

KEngine2D::CollisionInfo KEngine2D::BoundingCircle::Collides( BoundingCircle const & other ) const
{
	CollisionInfo retVal;
	Point center = GetCenter();
	Point otherCenter = other.GetCenter();
	retVal.collisionNormal = otherCenter;
	retVal.collisionNormal -= center;
	double distance2 = DotProduct(retVal.collisionNormal, retVal.collisionNormal);
	float radius = GetRadius();
	float otherRadius = other.GetRadius();
	double minDistance = radius + otherRadius;
	double minDistance2 = minDistance * minDistance;  // Cheaper than sqrt
	retVal.collides = distance2 <= minDistance2;
	retVal.collisionPoint = retVal.collisionNormal;
	retVal.collisionPoint *= radius / minDistance;
	retVal.collisionPoint += center;
	return retVal;
}

KEngine2D::CollisionInfo KEngine2D::BoundingCircle::Collides( BoundaryLine const & boundary ) const
{
	CollisionInfo retVal;
	Point center = GetCenter();
	double distance = boundary.GetSignedDistance(center);
	double minDistance = GetRadius();
	retVal.collides = (distance <= minDistance);
	retVal.collisionNormal = boundary.GetNormal();
	retVal.collisionPoint = center;
	retVal.collisionPoint -= retVal.collisionNormal;
	return retVal;
}

KEngine2D::BoundingBox::BoundingBox()
{
	mTransform = nullptr;
	mWidth = 0.0f;
	mHeight = 0.0f;
}

KEngine2D::BoundingBox::~BoundingBox()
{
	Deinit();
}

void KEngine2D::BoundingBox::Init(Transform * transform, double width, double height)
{
	assert(transform != 0);
	assert(width >= 0.0f);
	assert(height >= 0.0f);
	mTransform = transform;
	mWidth = width;
	mHeight = height;
}

void KEngine2D::BoundingBox::Deinit()
{
	mTransform = 0;
	mWidth = 0.0f;
	mHeight = 0.0f;
}

double KEngine2D::BoundingBox::GetWidth() const
{
	assert(mTransform != 0);
	assert(mTransform->GetScale().x == mTransform->GetScale().y); // Currently only supporting uniform scale, will need to revisit
	return mWidth * mTransform->GetScale().x;  
}

double KEngine2D::BoundingBox::GetHeight() const
{
	assert(mTransform != 0);
	assert(mTransform->GetScale().x == mTransform->GetScale().y); // Currently only supporting uniform scale, will need to revisit
	return mHeight * mTransform->GetScale().y;
}

KEngine2D::Point KEngine2D::BoundingBox::GetCenter() const
{
	assert(mTransform != 0);
	return mTransform->GetTranslation();
}

double KEngine2D::BoundingBox::GetArea() const
{
	return GetWidth() * GetHeight();
}

double KEngine2D::BoundingBox::GetAreaMomentOfInertia() const
{
	return (pow(GetHeight(), 2.0f) + pow(GetWidth(), 2.0f)) / 12.0f;
}

std::pair<KEngine2D::Point, KEngine2D::Point> KEngine2D::BoundingBox::GetAxisAlignedBoundingBox() const
{
	constexpr Corner firstCorner = (Corner)0;

	double minX = std::numeric_limits<double>::max();
	double minY = std::numeric_limits<double>::max();
	double maxX = std::numeric_limits<double>::lowest();
	double maxY = std::numeric_limits<double>::lowest();
	for (int c = firstCorner; c < Corner::CornerCount; c++) {
		Point p = GetCorner((Corner)c);
		minX = std::min(p.x, minX);
		minY = std::min(p.y, minY);
		maxX = std::max(p.x, maxX);
		maxY = std::max(p.y, maxY);
	}
	return { {minX, minY}, {maxX, maxY} };
}

KEngine2D::Point KEngine2D::BoundingBox::GetCorner(Corner corner) const
{
	assert(corner >= 0 && corner < Corner::CornerCount);
	constexpr Point corners[4] = {
		{-.5f, -.5f}, //Upper Left
		{ .5f, -.5f}, //Upper Right
		{ .5f,  .5f}, //Lower Right
		{-.5f,  .5f}  //Lower Left
	};

	const Point &cornerVec = corners[corner];

	return mTransform->LocalToGlobal({cornerVec.x * mWidth, cornerVec.y * mHeight});
}

KEngine2D::Point KEngine2D::BoundingBox::GetAxis(Axis axis) const
{
	assert(axis >= 0 && axis < Axis::AxisCount);
	return GetCorner((Corner)((axis * 2) + 1)) - GetCorner((Corner)0);
}

KEngine2D::CollisionInfo KEngine2D::BoundingBox::Collides(BoundaryLine const & boundary) const
{
	CollisionInfo retVal;
	retVal.collides = false;
	retVal.collisionNormal = boundary.GetNormal();
	retVal.collisionPoint = Point::Origin();
	int numPenetrating = 0;
	for (int i = 0; i < Corner::CornerCount; i++) {
		Point corner = GetCorner((Corner)i);
		float distance = boundary.GetSignedDistance(corner);
		if (distance < 0) {
			numPenetrating++;;
			retVal.collisionPoint += corner;
		}
	}
	retVal.collides = numPenetrating > 0;
	if (retVal.collides) {  //If multiple corners are beyond the boundary, scales the influence of that impact point by depth
		retVal.collisionPoint /= numPenetrating;
	}
	return retVal;
}

KEngine2D::CollisionInfo KEngine2D::BoundingBox::Collides(BoundingCircle const & other) const
{
	CollisionInfo retVal;
	retVal.collides = false;
	retVal.collisionNormal = Point::Origin();
	retVal.collisionPoint = Point::Origin();
	Point center = GetCenter();
	Point otherCenter = other.GetCenter();
	float radius = other.GetRadius();
	float radius2 = radius * radius;
	float halfWidth = GetWidth() / 2.0f;
	float halfHeight = GetHeight() / 2.0f;
	Point otherCenterLocal = mTransform->GlobalToLocal(other.GetCenter());

	if (otherCenterLocal.x > -halfWidth && otherCenterLocal.x < halfWidth && otherCenterLocal.y > -halfHeight && otherCenterLocal.y < halfHeight) //Deep penetration
	{
		retVal.collides = true;
		retVal.collisionNormal = otherCenter;
		retVal.collisionNormal -= center;
		retVal.collisionPoint = otherCenter;
	}
	else if (otherCenterLocal.x > -halfWidth && otherCenterLocal.x < halfWidth) { //Vertical
		if (otherCenterLocal.y > halfHeight && otherCenterLocal.y < halfHeight + radius) //Top
		{
			retVal.collides = true;
			retVal.collisionNormal = mTransform->LocalToGlobal({ 0, 1 }, true);
			retVal.collisionPoint = mTransform->LocalToGlobal({ otherCenterLocal.x, halfHeight }, false);
		}
		else if (otherCenterLocal.y > -(halfHeight + radius) && otherCenterLocal.y < halfHeight) //Bottom
		{
			retVal.collides = true;
			retVal.collisionNormal = mTransform->LocalToGlobal({ 0, -1 }, true);
			retVal.collisionPoint = mTransform->LocalToGlobal({ otherCenterLocal.x, -halfHeight }, false);
		}
	}
	else if (otherCenterLocal.y > -halfHeight && otherCenterLocal.y < halfHeight) //Horizontal
	{
		if (otherCenterLocal.x > halfWidth && otherCenterLocal.x < halfWidth + radius) //Right
		{
			retVal.collides = true;
			retVal.collisionNormal = mTransform->LocalToGlobal({ 1, 0 }, true);
			retVal.collisionPoint = mTransform->LocalToGlobal({ halfWidth, otherCenterLocal.y }, false);
		}
		else if (otherCenterLocal.x > -(halfWidth + radius) && otherCenterLocal.x < halfWidth) //Left
		{
			retVal.collides = true;
			retVal.collisionNormal = mTransform->LocalToGlobal({ -1, 0 }, true);
			retVal.collisionPoint = mTransform->LocalToGlobal({ -halfWidth, otherCenterLocal.y }, false);
		}
	} 
	else // Check for corner penetration
	{
		for (int i = 0; i < Corner::CornerCount; i++) {
			Point corner = GetCorner((Corner)i);
			Point axis = otherCenter - corner;
			float dist2 = DotProduct(axis, axis);
			if (dist2 < radius2)
			{
				retVal.collides = true;
				retVal.collisionNormal = axis;
				retVal.collisionPoint = corner;
				break;
			}
		}
	}
	return retVal;
}


KEngine2D::CollisionInfo KEngine2D::BoundingBox::Collides(BoundingBox const & other) const
{
	CollisionInfo retVal;
	retVal.collides = MayCollide(other) && other.MayCollide(*this);
	retVal.collisionNormal = Point::Origin();
	retVal.collisionPoint = Point::Origin();
	if (retVal.collides) {
		std::vector<std::pair<Point, Point>> cornerPenetrations;
		//Does our corners penetrate?
		for (int i = 0; i < Corner::CornerCount; i++) {
			CollisionInfo possibleCollision = other.Collides(GetCorner((Corner)i));
			if (possibleCollision.collides)
			{
				cornerPenetrations.push_back({ possibleCollision.collisionPoint, -possibleCollision.collisionNormal });// Invert the normal
			}
		}
		//Okay, does one of their corners penetrate?
		for (int i = 0; i < Corner::CornerCount; i++) {
			CollisionInfo possibleCollision = Collides(other.GetCorner((Corner)i));
			if (possibleCollision.collides)
			{
				cornerPenetrations.push_back({ possibleCollision.collisionPoint, possibleCollision.collisionNormal });
			}
		}

		if (cornerPenetrations.size() > 0) {
			for (auto penetration : cornerPenetrations) 
			{
				retVal.collisionPoint += penetration.first;
				retVal.collisionNormal = penetration.second;
			}
			retVal.collisionPoint /= cornerPenetrations.size();
			//retVal.collisionNormal /= cornerPenetrations.size();
			
		}
		
		if (retVal.collisionNormal.x == 0 && retVal.collisionNormal.y == 0) 
		{
			retVal.collisionNormal = other.GetCenter();
			retVal.collisionNormal -= GetCenter();
			retVal.collisionPoint = other.GetCenter();
		}
	}
	return retVal;
}

KEngine2D::CollisionInfo KEngine2D::BoundingBox::Collides(Point const & other) const
{
	CollisionInfo retVal;
	retVal.collisionPoint = other;
	Point otherLocal = mTransform->GlobalToLocal(other);
	float halfWidth = GetWidth() / 2.0f;
	float halfHeight = GetHeight() / 2.0f;
	float slope = halfHeight / halfWidth;
	retVal.collides = otherLocal.x > -halfWidth && otherLocal.x < halfWidth && otherLocal.y > -halfHeight &&  otherLocal.y < halfHeight;
	if (otherLocal.y > (otherLocal.x * slope)) // Upper Right
	{
		if (otherLocal.y > (otherLocal.x * -slope)) //Upper quadrant
		{
			retVal.collisionNormal = mTransform->LocalToGlobal({ 0, 1 }, true);
		}
		else // Right quadrant
		{
			retVal.collisionNormal = mTransform->LocalToGlobal({ 1, 0 }, true);
		}
	}
	else // Lower Left
	{
		if (otherLocal.y > (otherLocal.x * -slope)) //Left quadrant
		{
			retVal.collisionNormal = mTransform->LocalToGlobal({ -1, 0 }, true);
		}
		else // Bottom quadrant
		{
			retVal.collisionNormal = mTransform->LocalToGlobal({ 0, -1 }, true);
		}
	}
	return retVal;
}

bool KEngine2D::BoundingBox::MayCollide(BoundingBox const & other) const
{
	constexpr Corner firstCorner = (Corner)0;
	
	for (int a = 0; a < Axis::AxisCount; a++) {

		Corner firstCorner = (Corner)0;

		Point axis = GetAxis((Axis)a);
		axis /= DotProduct(axis, axis);
		float origin = DotProduct(GetCorner(firstCorner), axis);

		double t = DotProduct(other.GetCorner(Corner::UpperLeft), axis);

		// Find the extent of box 2 on axis a
		double tMin = t;
		double tMax = t;

		for (int c = firstCorner + 1; c < Corner::CornerCount; ++c) {
			t = DotProduct(other.GetCorner((Corner)c), axis);

			if (t < tMin) {
				tMin = t;
			}
			else if (t > tMax) {
				tMax = t;
			}
		}
		// See if [tMin, tMax] intersects [0, 1]
		if ((tMin > 1 + origin) || (tMax < origin)) {
			// There was no intersection along this dimension;
			// the boxes cannot possibly overlap.
			return false;
		}
	}
    return true;
}

void KEngine2D::BoundingArea::Init(Transform * transform)
{
	mTransform = transform;
	mBoundingBoxes.clear();
	mBoundingCircles.clear();
}

void KEngine2D::BoundingArea::Deinit()
{
	mBoundingBoxes.clear();
	mBoundingCircles.clear();
	mTransform = nullptr;
}

KEngine2D::Point KEngine2D::BoundingArea::GetCenter() const
{
	assert(mTransform != 0);
	return mTransform->GetTranslation();
}

void KEngine2D::BoundingArea::AddBoundingBox(const BoundingBox * box)
{
	mBoundingBoxes.push_back(box);
}

void KEngine2D::BoundingArea::AddBoundingCircle(const BoundingCircle * circle)
{
	mBoundingCircles.push_back(circle);
}

double KEngine2D::BoundingArea::GetAreaMomentOfInertia()
{
	double accumulator = 0.0f;
	for (const BoundingBox * box : mBoundingBoxes) {
		accumulator += box->GetAreaMomentOfInertia();
		Point offset = box->GetCenter() - GetCenter();
		accumulator += DotProduct(offset, offset) * box->GetArea();
	}


	for (const BoundingCircle * circle : mBoundingCircles) {
		accumulator += circle->GetAreaMomentOfInertia();
		Point offset = circle->GetCenter() - GetCenter();
		accumulator += DotProduct(offset, offset) * circle->GetArea();
	}
	return accumulator;
}

std::pair<KEngine2D::Point, KEngine2D::Point> KEngine2D::BoundingArea::GetAxisAlignedBoundingBox() const
{
	double minX = std::numeric_limits<double>::max();
	double minY = std::numeric_limits<double>::max();
	double maxX = std::numeric_limits<double>::lowest();
	double maxY = std::numeric_limits<double>::lowest();
	for (const BoundingBox* box : mBoundingBoxes) {
		auto bounds = box->GetAxisAlignedBoundingBox();
		minX = std::min(bounds.first.x, minX);
		minY = std::min(bounds.first.y, minY);
		maxX = std::max(bounds.second.x, maxX);
		maxY = std::max(bounds.second.y, maxY);
	}
	for (const BoundingCircle* circle : mBoundingCircles) {
		auto bounds = circle->GetAxisAlignedBoundingBox();
		minX = std::min(bounds.first.x, minX);
		minY = std::min(bounds.first.y, minY);
		maxX = std::max(bounds.second.x, maxX);
		maxY = std::max(bounds.second.y, maxY);
	}
	return {{minX, minY}, { maxX, maxY }};
}

//Doesn't get the complete collision manifold, sorry.
KEngine2D::CollisionInfo KEngine2D::BoundingArea::Collides(const BoundingArea & other) const
{
	for (const BoundingBox * box : mBoundingBoxes)
	{
		for (const BoundingBox * otherBox : other.mBoundingBoxes)
		{
			CollisionInfo possibleCollision = box->Collides(*otherBox);
			if (possibleCollision.collides)
			{
				return possibleCollision;
			}
		}

		for (const BoundingCircle * otherCircle : other.mBoundingCircles)
		{
			CollisionInfo possibleCollision = box->Collides(*otherCircle);
			if (possibleCollision.collides)
			{
				return possibleCollision;
			}
		}
	}

	
	for (const BoundingCircle * circle : mBoundingCircles)
	{
		for (const BoundingCircle * otherCircle : other.mBoundingCircles)
		{
			CollisionInfo possibleCollision = circle->Collides(*otherCircle);
			if (possibleCollision.collides) {
				return possibleCollision;
			}
		}
	}
	return { false, Point::Origin(), Point::Origin() };
}

KEngine2D::CollisionInfo KEngine2D::BoundingArea::Collides(BoundaryLine const & boundary) const
{
	for (const BoundingBox * box : mBoundingBoxes)
	{
		CollisionInfo possibleCollision = box->Collides(boundary);
		if (possibleCollision.collides) {
			return possibleCollision;
		}
	}
	for (const BoundingCircle * circle : mBoundingCircles)
	{
		CollisionInfo possibleCollision = circle->Collides(boundary);
		if (possibleCollision.collides) {
			return possibleCollision;
		}
	}
	return{ false, Point::Origin(), Point::Origin() };
}

const std::vector<const KEngine2D::BoundingBox*>& KEngine2D::BoundingArea::GetBoundingBoxes()
{
	return mBoundingBoxes;
}

const std::vector<const KEngine2D::BoundingCircle*>& KEngine2D::BoundingArea::GetBoundingCircles()
{
	return mBoundingCircles;
}
