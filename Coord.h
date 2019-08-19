#pragma once
#include <cmath>
#define pi 3.14159265359



struct Coord {
	float x, y;
	Coord ( float x, float y ) : x ( x ), y ( y ) {}

	Coord() : Coord ( 0, 0 ) {}
	bool operator== ( Coord c )
	{
		return this->x == c.x && this->y == c.y;
	}
	void setSpeed ( float speed )
	{
		float ang = angle();
		x = cos ( ang ) * speed;
		y = sin ( ang ) * speed;
	}
	bool operator != ( Coord c )
	{
		return ! ( ( *this ) == c );
	}
	Coord operator * ( float a )
	{
		return Coord ( x * a, y * a );
	}
	Coord operator- ( Coord b )
	{
		return Coord ( this->x - b.x, this->y - b.y );
	}
	Coord operator+ ( Coord b )
	{
		return Coord ( this->x + b.x, this->y + b.y );
	}
	float distance ( Coord b = {0, 0} )
	{
		float t = b.x - this->x;
		float u = b.y - this->y;
		t *= t;
		u *= u;
		return pow ( t + u, 0.5 );
	}
	double angle() const
	{
		auto alce = Coord ( 0, 0 );
		return alce.angle ( *this );
	}
	double angle ( Coord b ) const
	{
		float x = b.x - this->x;
		float y = b.y - this->y;
		float angle;
		angle = atan ( y / x );
		if ( angle < 0 ) { angle = angle * -1; }
		if ( x < 0 && y > 0 ) {
			angle = pi - angle;
		}
		if ( x < 0 && y < 0 ) {
			angle = pi + angle;
		}
		if ( x > 0 && y < 0 ) {
			angle = 2 * pi - angle;
		}
		return angle;
	}
	friend std::ostream& operator<< ( std::ostream& os, const Coord& p )
	{
		os << "X:" << p.x << "  Y:" << p.y;
		return os;
	}
};


