#pragma once

namespace KEngine2D
{
    struct Matrix
    {
        float  data[4][4];
        static Matrix const & Identity();
    };
    
	struct Point
	{
		double x;
		double y;
		static Point const & Origin();

		Point & operator+=(Point const & other);
		Point & operator-=(Point const & other);
		Point & operator*=(double const & scalar);
		Point & operator/=(double const & scalar);
        Point & operator*=(Matrix const & transform);
		Point operator-();
		Point operator+(Point const & other);
		Point operator-(Point const & other);
	};

	float DotProduct(Point const & vec1, Point const & vec2);
	float PseudoCrossProduct(Point const & vec1, Point const & vec2);
	Point PseudoCrossProduct(Point const & vec1, float scalar);
	Point Project(Point const & axis, Point const & vec, bool positiveOnly = false);

	class Transform
	{
	public:
		virtual ~Transform() {}
		virtual Point GetTranslation() const = 0;
		virtual double GetRotation() const = 0;
		virtual double GetScale() const = 0;
        virtual const Matrix& GetAsMatrix() const = 0;

		virtual Point LocalToGlobal(Point const & point, bool asVector = false) const;
		virtual Point GlobalToLocal(Point const & point) const;
	};
}
