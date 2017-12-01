#pragma  once

#include "Transform2D.h"

namespace KEngine2D
{
	class StaticTransform : public Transform
	{
	public:
		StaticTransform(Point const & translation = Point::Origin(), double radians = 0.0f, double scale = 1.0f);
	
		virtual Point GetTranslation() const;
		virtual double GetRotation() const;
		virtual double GetScale() const;
        virtual const Matrix & GetAsMatrix() const;
	
		void SetTranslation(Point const & translation);
		void SetRotation(double rotation);
		void SetScale(double scale);

		static StaticTransform const & Identity();

	private:
        void UpdateMatrix();
        
		Point mTranslation;
		double mRotation;
		double mScale;
        Matrix mMatrix;
	};
}
