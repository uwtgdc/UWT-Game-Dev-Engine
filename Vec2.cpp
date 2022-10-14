#include "Vec2.h"
#include <math.h>

c_Vec2::c_Vec2()
{

}

c_Vec2::c_Vec2(float xin, float yin)
	: x(xin), y(yin) {}


c_Vec2 c_Vec2::operator + (const c_Vec2 &rhs) const
{
	return c_Vec2(x + rhs.x, y + rhs.y);
}

c_Vec2 c_Vec2::operator - (const c_Vec2 &rhs) const
{
	return c_Vec2(x - rhs.x, y - rhs.y);
}

c_Vec2 c_Vec2::operator / (const float val) const
{
	return c_Vec2(x/val, y/val);
}

c_Vec2 c_Vec2::operator * (const float val) const
{
	return c_Vec2(x*val, y*val);
}

bool c_Vec2::operator == (const c_Vec2 &rhs) const
{
	return x == rhs.x && y == rhs.y;
}
bool c_Vec2::operator != (const c_Vec2 &rhs) const
{
	return x != rhs.x || y != rhs.y;
}

void c_Vec2::operator = (const c_Vec2 &rhs) 
{
	x= rhs.x;
	y= rhs.y;
}

void c_Vec2::operator += (const c_Vec2 &rhs) 
{
	x+= rhs.x;
	y+= rhs.y;
}

void c_Vec2::operator -= (const c_Vec2 &rhs) 
{
	x-= rhs.x;
	y-= rhs.y;
}

void c_Vec2::operator *= (const float val) 
{
	x*= val;
	y*= val;
}

void c_Vec2::operator /= (const float val) 
{
	x/= val;
	y/= val;
}

float c_Vec2::compute_distance(const c_Vec2 &rhs) const
{
	return sqrt((rhs.x - x)*(rhs.x - x) + (rhs.y - y) * (rhs.y - y));
}

float c_Vec2::get_distance_squared(const c_Vec2 &rhs) const
{
	return ((rhs.x - x) * (rhs.x - x)) + ((rhs.y - y) * (rhs.y - y));
}

float c_Vec2::get_magnitude_squared() const
{
	return (x * x) + (y * y);
}

// adapted from quick inverse from Quake 3 source code
float c_Vec2::quick_inverse_magnitude() const
{
	float magnitude_squared= this->get_magnitude_squared();
	long i;
	float x2, y;
	const float three_halves= 1.5f;

	x2= magnitude_squared * 0.5f;
	y= magnitude_squared;
	i= *(long *)&y;
	i= 0x5f3759df - (i >> 1);
	y= *(float *)&i;
	y= y * (three_halves - (x2 * y * y));
	y= y * (three_halves - (x2 * y * y));

	return y;
}

float c_Vec2::cross_product(const c_Vec2 &rhs) const
{
	return x * rhs.y - y * rhs.x;
}