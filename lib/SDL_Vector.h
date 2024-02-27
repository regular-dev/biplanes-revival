/*
	Original Vector class definition by Miguel Gomez, which was written for an article called "C++ Data Structures for Rigid-Body Physics", and can be found in http://www.gamasutra.com/view/feature/131761/c_data_structures_for_rigidbody_.php?page=2
*/

#include <math.h>

typedef float Scalar;

class SDL_Vector {
	public:
		Scalar x,y,z;
		SDL_Vector():
			x(0), y(0), z(0) {}
		SDL_Vector( const Scalar& a, const Scalar& b, const Scalar& c ):
			x(a), y(b), z(c) {}

		Scalar& operator[] (const long i) {
			return *((&x) + i);
		}

		const bool operator == ( const SDL_Vector& v ) const {
			return (v.x==x && v.y==y && v.z==z);
		}

		const bool operator != ( const SDL_Vector& v ) const {
			return !(v == *this);
		}

		const SDL_Vector operator - () const {
			return SDL_Vector( -x, -y, -z );
		}

		const SDL_Vector& operator = ( const SDL_Vector& v ) {
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}

		const SDL_Vector& operator += ( const SDL_Vector& v ) {
			x+=v.x;
			y+=v.y;
			z+=v.z;
			return *this;
		}

		const SDL_Vector& operator -= ( const SDL_Vector& v ) {
			x-=v.x;
			y-=v.y;
			z-=v.z;
			return *this;
		}

		const SDL_Vector& operator *= ( const Scalar& s ) {
			x*=s;
			y*=s;
			z*=s;
			return *this;
		}

		const SDL_Vector& operator /= ( const Scalar& s ) {
			const Scalar r = 1 / s;
			x *= r;
			y *= r;
			z *= r;
			return *this;
		}

		const SDL_Vector operator + ( const SDL_Vector& v ) const {
			return SDL_Vector(x + v.x, y + v.y, z + v.z);
		}

		//subtract

		const SDL_Vector operator - ( const SDL_Vector& v ) const {
			return SDL_Vector(x - v.x, y - v.y, z - v.z);
		}

		//post-multiply by a scalar

		const SDL_Vector operator * ( const Scalar& s ) const {
			return SDL_Vector( x*s, y*s, z*s );
		}

		//pre-multiply by a scalar

		friend inline const SDL_Vector operator * ( const Scalar& s, const SDL_Vector& v ) {
			return v * s;
		}

		//divide

		const SDL_Vector operator / (Scalar s) const {
			s = 1/s;
			return SDL_Vector( s*x, s*y, s*z );
		}

		//cross product

		const SDL_Vector cross( const SDL_Vector& v ) const {
			//Davis, Snider, "Introduction to Vector Analysis", p. 44
			return SDL_Vector( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x );
		}

		//scalar dot product

		const Scalar dot( const SDL_Vector& v ) const {
			return x*v.x + y*v.y + z*v.z;
		}

		//length

		const Scalar length() const {
			return (Scalar)sqrt( (double)this->dot(*this) );
		}

		//unit vector

		const SDL_Vector unit() const {
			return (*this) / length();
		}

		//make this a unit vector

		void normalize() {
			(*this) /= length();
		}

		const bool nearlyEquals( const SDL_Vector& v, const Scalar e ) const {
			return fabs(x-v.x);
		}
};
