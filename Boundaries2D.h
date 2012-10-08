#pragma once
#include "Transform2D.h"

namespace KEngine2D
{
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

		bool Collides(BoundingCircle const & other);
		bool Collides(BoundaryLine const & boundary);

	private:
		double		mRadius;
		Transform *	mTransform;
	};
}