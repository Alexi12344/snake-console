#include <stdlib.h>
#include "Apple.h"

float random_float()
{
	return (float)rand() / RAND_MAX;
}

Vec2 generate_apple_pos(int maxwidth, int maxheight)
{
	Vec2 pos;
	pos.x = (int)(random_float() * (maxwidth - 1));
	pos.y = (int)(random_float() * (maxheight - 1));
	return pos;
}