#pragma once

namespace KEngine2D
{
	struct Point
	{
		double x;
		double y;
		static Point const & Origin();

		Point & operator+=(Point const & other);
		Point & operator-=(Point const & other);
		Point & operator*=(double const & scalar);
		Point operator-();
	};

	Point Project(Point const & axis, Point const & vec, bool positiveOnly = false);

	class Transform
	{
	public:
		virtual ~Transform() {}
		virtual Point GetTranslation() const = 0;
		virtual double GetRotation() const = 0;
		virtual double GetScale() const = 0;

		virtual Point Apply(Point const & point) const; 
	};
}