#pragma once
#include "Transform2D.h"
#include <vector>
#include <tuple>

namespace KEngine2D
{
	class BoundaryLine;
	class BoundingCircle;
	class BoundingBox;

	struct CollisionInfo
	{
		bool collides;
		Point collisionPoint;
		Point collisionNormal;
	};

	class BoundaryLine
	{
	public:
		BoundaryLine();
		~BoundaryLine();

		void Init(double xCoefficient, double yCoefficient, double constantCoefficient);
		void Deinit();

		double GetSignedDistance(Point const & point) const;
		Point GetNormal() const;
	private:
		double mXCoefficient;
		double mYCoefficient;
		double mConstantCoefficient;
	};

	class BoundingCircle
	{
	public:

		BoundingCircle();
		~BoundingCircle();

		void Init(Transform * transform, double radius);
		void Deinit();

		double GetRadius() const;
		Point GetCenter() const;
		double GetArea() const;
		double GetAreaMomentOfInertia() const;

		CollisionInfo Collides(BoundingCircle const & other) const;
		CollisionInfo Collides(BoundaryLine const & boundary) const;

	private:
		double		mRadius;
		Transform *	mTransform;
	};

	class BoundingBox
	{
	public:
		BoundingBox();
		~BoundingBox();

		void Init(Transform * transform, double width, double height);
		void Deinit();
		double GetWidth() const;
		double GetHeight() const;
		Point GetCenter() const;
		double GetArea() const;
		double GetAreaMomentOfInertia() const;

		CollisionInfo Collides(BoundingCircle const & other) const;
		CollisionInfo Collides(BoundaryLine const & boundary) const;
		CollisionInfo Collides(BoundingBox const & other) const;
		CollisionInfo Collides(Point const & other) const;

	private:
		enum Corner {
			UpperLeft,
			UpperRight,
			LowerRight,
			LowerLeft,
			CornerCount
		};

		enum Axis {
			Horizontal,
			Vertical,
			AxisCount
		};
		
		Point GetCorner(Corner corner) const;
		Point GetAxis(Axis axis) const;
		bool MayCollide(BoundingBox const & other) const;

		double		mWidth;
		double		mHeight;
		Transform *	mTransform;
	};

	class BoundingArea
	{
	public:
		void Init(Transform * transform);
		void Deinit();
		Point GetCenter() const;
		void AddBoundingBox(const BoundingBox * box);
		void AddBoundingCircle(const BoundingCircle * circle);
		double GetAreaMomentOfInertia();
		std::pair<Point,Point> GetAxisAlignedBoundingBox() const;

		CollisionInfo Collides(const BoundingArea &other) const;
		CollisionInfo Collides(BoundaryLine const & boundary) const;

		const std::vector<const BoundingBox *>& GetBoundingBoxes();
		const std::vector<const BoundingCircle *>& GetBoundingCircles();

	private:
		std::vector<const BoundingBox *> mBoundingBoxes;
		std::vector<const BoundingCircle *> mBoundingCircles;
		Transform * mTransform;
	};
}