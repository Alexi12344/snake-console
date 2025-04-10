#include "Vec2.h"

Vec2 vec_add(Vec2 v1, Vec2 v2)
{
	Vec2 v;
	v.x = v1.x + v2.x;
	v.y = v1.y + v2.y;
	return v;
}

int vec_equal(Vec2 v1, Vec2 v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y);
}