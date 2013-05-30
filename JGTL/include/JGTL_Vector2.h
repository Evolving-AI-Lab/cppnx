#ifndef VECTOR2_H_INCLUDED
#define VECTOR2_H_INCLUDED

#include <string>
#include <iostream>
#include <cmath>

namespace JGTL
{
	/**
	* @class Vector2
	* @brief This class handles 2D Vectors
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class T>
	class Vector2
	{
	public:
		T x,y;

		Vector2()
			:
		x(0),
			y(0)
		{}

		Vector2(T _x,T _y)
			:
		x(_x),
			y(_y)
		{}

		/*
		* Creates a Vector from an angle and magnitude
		*/
		template <class TT, class TTT>
		static Vector2<T> fromMagnitudeAngle(TT magnitude,TTT angle)
		{
			return Vector2<T>((T)cos(angle)*magnitude,(T)sin(angle)*magnitude);
		}

		/*
		* Special constructor to convert types
		*/
		template<class TT>
		Vector2<T>(const Vector2<TT> &other)
		{
			x = (T)other.x;
			y = (T)other.y;
		}

		/*
		* Assignment operator which also converts type
		*/
		template <class TT>
		const Vector2 &operator =(const Vector2<TT> &other)
		{
			if (this != ((const Vector2<T>*)&other))
			{
				x = (T)other.x;
				y = (T)other.y;
			}

			return *this;
		}

		/*
		* Equality operator
		*/
		template <class TT>
		bool operator==(const Vector2<TT> &other) const
		{
			return (x==other.x && y==other.y);
		}

		/*
		* Inequality operator
		*/
		template <class TT>
		bool operator!=(const Vector2<TT> &other) const
		{
			return (x!=other.x || y!=other.y);
		}

		/*
		* Less-than operator
		*/
		template <class TT>
		bool operator<(const Vector2<TT> &other) const
		{
			return (
				y<other.y ||
				( y==other.y&&x<other.x )
				);
		}

		Vector2<T> operator/(T divisor) const
		{
			return Vector2<T>(x/divisor,y/divisor);
		}

		void operator/=(T divisor)
		{
			x/=divisor;
			y/=divisor;
		}

		Vector2<T> operator-() const
		{
			Vector2<T> newVec(-x,-y);
			return newVec;
		}

		template <class TT>
		Vector2<T> operator-(const Vector2<TT> &other) const
		{
			return Vector2<T>(this->x-other.x,this->y-other.y);
		}

		template <class TT>
		Vector2<T> operator+(const Vector2<TT> &other) const
		{
			return Vector2<T>(this->x+other.x,this->y+other.y);
		}

		Vector2<T> operator*(const T &coeff) const
		{
			return Vector2<T>(x*coeff,y*coeff);
		}

		template <class TT>
		void operator+=(const Vector2<TT> &other)
		{
			x+=(T)other.x;
			y+=(T)other.y;
		}

		template <class TT>
		void operator-=(const Vector2<TT> &other)
		{
			x-=(T)other.x;
			y-=(T)other.y;
		}

		void operator*=(T val)
		{
			x*=val;
			y*=val;
		}

		/*
		* Returns the dot product of the vector onto another
		*/
		template <class TT>
		T dot(const Vector2<TT> &other) const
		{
			return T(this->x*other.x+this->y*other.y);
		}

		/*
		* Returns the magnitude of the cross product of this
		* vector onto another.  This value actually represents
		* the z-component of the cross-product, so the actual
		* cross product is (0,0,z)
		*/
		template <class TT>
		T cross(const Vector2<TT> &other) const
		{
			return (x * other.y - y * other.x);
		}

		/*
		* Returns the Right Hand Normal of the vector
		*/
		Vector2<T> rightHandNormal() const
		{
			return Vector2<T>(-y,x);
		}

		T magnitudeSquared() const
		{
			return dot(*this);
		}

		T magnitude() const
		{
			return (T) sqrt( (double) dot(*this) );
		}

		void normalize()
		{
			T mag = sqrt(magnitudeSquared());

			if (mag>1e-6)
			{
				x/=mag;
				y/=mag;
			}
		}

		Vector2<T> normalizeCopy() const
		{
			Vector2<T> newVec(*this);

			newVec.normalize();

			return newVec;
		}

		template <class TT>
		Vector2<T> projectOn(const Vector2<TT> &other) const
		{
			T dp = this->dot(other);

			Vector2<T> proj = other*(dp/other.getMagnitude());

			return proj;
		}

		template <class TT>
		T distance ( const Vector2<TT> &other ) const
		{
			return (T)sqrt(
				( x-other.x ) * ( x-other.x ) +
				( y-other.y ) * ( y-other.y )
				);
		}

		template <class TT>
		inline T distance ( TT _x,TT _y ) const
		{
			return (T)sqrt(
				( x- ( T ) _x ) * ( x- ( T ) _x ) +
				( y- ( T ) _y ) * ( y- ( T ) _y )
				);
		}

		template <class TT>
		T distanceSquared(const Vector2<TT> &other) const
		{
			return (x-other.x)*(x-other.x)+(y-other.y)*(y-other.y);
		}

		template <class TT>
		T chessDistance(const Vector2<TT> &other) const
		{
			return max(abs(x-other.x),abs(y-other.y));
		}				

		template <class TT>
		T angleTo(const Vector2<TT> &other) const
		{
			Vector2<T> v1 = normalizeCopy();
			Vector2<TT> v2 = other.normalizeCopy();

			T angle = atan2(v2.y,v2.x) - atan2(v1.y,v1.x);

			if(angle>M_PI)
			{
				angle -= (T)(M_PI*2.0);
			}
			else if(angle<-M_PI)
			{
				angle += (T)(M_PI*2.0);
			}

			return angle;
		}

		template <class TT>
		void rotate(TT angleTheta)
		{
			T oldx=x;
			T oldy=y;

			x = (T)(cos(angleTheta)*oldx - sin(angleTheta)*oldy);
			y = (T)(sin(angleTheta)*oldx + cos(angleTheta)*oldy);
		}

		template <class TT>
		Vector2<T> rotateCopy(TT angleTheta) const
		{
			Vector2<T> other(*this);

			other.rotate(angleTheta);

			return other;
		}

		T getAngle() const
		{
			return (T)atan2(y,x);
		}

		bool isContainedIn(T left, T top, T right, T bottom) const
		{
			if(x>=left && y>=top && x<=right && y<=bottom)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		template <class TT>
		inline T manhatDistance ( const Vector2<TT> &other ) const
		{
			return abs ( x- ( T ) other.x ) +abs ( y- ( T ) other.y );
		}
	};

	template<class T>
	inline std::ostream& operator<<(std::ostream& stream, const Vector2<T>& d)
	{
		stream << d.x << " " << d.y;

		return stream;
	}

	template<class T>
	inline std::istream& operator>>(std::istream& stream, Vector2<T>& d)
	{
		stream >> d.x >> d.y;

		return stream;
	}

	/*
	 * This function allows conversion between 2-D Vector types
	 */
	template<class T, class TT>
	T convertVector2(const TT &other)
	{
		return T(other.x,other.y);
	}
}

#endif // VECTOR2_H_INCLUDED


