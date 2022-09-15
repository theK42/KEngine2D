#pragma  once

#include "Transform2D.h"

namespace KEngine2D
{
	class StaticTransform : public Transform
	{
	public:
		StaticTransform(Point const & translation = Point::Origin(), double radians = 0.0f, Point scale = Point::Identity());
	
		virtual Point GetTranslation() const;
		virtual double GetRotation() const;
		virtual Point GetScale() const;
        virtual const Matrix & GetAsMatrix() const;
	
		void SetTranslation(Point const & translation);
		void SetRotation(double rotation);
		void SetScale(Point const& scale);

		static StaticTransform const & Identity();

		StaticTransform& operator=(Transform const& other);

	private:
        void UpdateMatrix();
        
		Point mTranslation;
		double mRotation;
		Point mScale;
        Matrix mMatrix;
	};
}
