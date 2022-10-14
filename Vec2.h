#pragma once

class c_Vec2
{
public:

	float x= 0;
	float y= 0;

	c_Vec2();
	c_Vec2(float xin, float yin);

	bool operator == (const c_Vec2 &rhs) const;
	bool operator != (const c_Vec2 &rhs) const;

	c_Vec2 operator + (const c_Vec2 &rhs) const;
	c_Vec2 operator - (const c_Vec2 &rhs) const;
	c_Vec2 operator / (const float val)   const;
	c_Vec2 operator * (const float val)   const;

	void operator =	 (const c_Vec2 &rhs);
	void operator += (const c_Vec2 &rhs);
	void operator -= (const c_Vec2 &rhs);
	void operator *= (const float val);
	void operator /= (const float val);

	float compute_distance(const c_Vec2 &rhs) const;
	float get_distance_squared(const c_Vec2 &rhs) const;
	float get_magnitude_squared() const;
	float quick_inverse_magnitude() const;
	float cross_product(const c_Vec2 &rhs) const;
};