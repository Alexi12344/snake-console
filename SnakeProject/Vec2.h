#pragma once
#ifndef VEC2_H
#define VEC2_H

typedef struct _Vec2
{
	int x;
	int y;
} Vec2;

Vec2 vec_add(Vec2 v1, Vec2 v2);
int vec_equal(Vec2 v1, Vec2 v2);

#endif // VEC2_H