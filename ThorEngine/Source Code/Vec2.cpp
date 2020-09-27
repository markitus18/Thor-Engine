#include "Vec2.h"

// #include "TMath.h" TODO: Add Math functions library for Clamp

Vec2 Vec2::FitInRect(const Vec2& rect)
{
	Vec2 ret(*this);
	if (ret.x > rect.x)
	{
		float ratio = ret.x / rect.x;
		ret /= ratio;
	}
	if (ret.y > rect.y)
	{
		float ratio = ret.y / rect.y;
		ret /= ratio;
	}
	return ret;
}

void Vec2::Rotate(Vec2 pivot, float angle)
{
	Vec2 rot_vec = *this - pivot;

	float new_angle = rot_vec.Angle() + angle;
	float lenght = rot_vec.Lenght();

	x = pivot.x + cos(new_angle) * lenght;
	y = pivot.y + sin(new_angle) * lenght;
}

/*
void Vec2::Transform(const Mat3x3& mat)
{
	int v_x = x;
	x = x * mat[0][0] + y * mat[0][1] + mat[0][2];
	y = v_x * mat[1][0] + y * mat[1][1] + mat[1][2];
}
*/

Vec2 Vec2::Lerp(Vec2 begin, Vec2 end, float ratio)
{
	//ratio = Math::Clamp(ratio, 0, 1); TODO: Add Math functions library for Clamp
	return begin + (end - begin) * ratio;
}