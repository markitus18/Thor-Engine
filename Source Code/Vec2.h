#ifndef __VEC2_H__
#define __VEC2_H__

#include <math.h>

class Mat3x3;

struct Vec2
{
	//Methods
	Vec2() {};
	Vec2(const Vec2& vec) { x = vec.x; y = vec.y; }
	Vec2(float x, float y) : x(x), y(y) {}

	void Set(float x, float y) { this->x = x; this->y = y; }
	float* Ptr() { return &x; }

	Vec2 FitInRect(const Vec2& rect);

	float* operator&() { return (float*)this; }

	//Operators
	Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
	Vec2 operator+=(const Vec2& other) { return *this = Vec2(x + other.x, y + other.y); }

	Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
	Vec2 operator-=(const Vec2& other) { return *this = Vec2(x - other.x, y - other.y); }
	Vec2 operator-() const { return Vec2(-x, -y); }

	Vec2 operator*(const Vec2& other) const { return Vec2(x * other.x, y * other.y); }
	Vec2 operator*=(const Vec2& other) { return *this = Vec2(x * other.x, y * other.y); }

	Vec2 operator*(const float& factor) const { return Vec2(x * factor, y * factor); }
	Vec2 operator*=(const float& factor) { return *this = Vec2(x * factor, y * factor); }

	Vec2 operator/(const Vec2& other) const { return Vec2(x / other.x, y / other.y); }
	Vec2 operator/=(const Vec2& other) { return *this = Vec2(x / other.x, y / other.y); }

	Vec2 operator/(const float& factor) const { return Vec2(x / factor, y / factor); }
	Vec2 operator/=(const float& factor) { return *this = Vec2(x / factor, y / factor); }

	bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }
	bool operator!=(const Vec2& other) const { return x != other.x || y != other.y; }

	static Vec2 zero() { return Vec2(0, 0); };
	static Vec2 one() { return Vec2(1, 1); };

	float Lenght() const { return sqrt(x * x + y * y); }
	float Angle() const { return atan2(y, x); }

	void Normalize() { *this /= Lenght(); }
	Vec2 Normalized() const { return (*this / Lenght()); }
	//*Perform a rotation of the current point
	//		*pivot - center of rotation
	//		*angle - angle of the rotation in radians
	void Rotate(Vec2 pivot, float angle);
	//void Transform(const Mat3x3& mat); //TODO: Add Mat3x3 class or use MathGeoLib
	static Vec2 Lerp(Vec2 begin, Vec2 end, float ratio);

	//Variables
	float x = 0, y = 0;
};

#endif