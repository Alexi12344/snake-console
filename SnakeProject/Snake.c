#include <stdio.h>
#include <stdlib.h>
#include "Snake.h"

int snake_init(Snake* psnake, int max_size, Vec2 initial_pos)
{
	psnake->points = calloc(max_size, sizeof(Vec2));

	if(!psnake->points)
	{
		return 0;
	}

	psnake->points[0] = initial_pos;
	psnake->points[1] = vec_add(initial_pos, (Vec2) { -1, 0 });
	psnake->size = 2;
	psnake->maxsize = max_size;
	return 1;
}

SnakeState snake_step(Snake* psnake, Vec2 dir, Vec2 screensize, Vec2 apple_pos)
{
	// apple, winner, gameover, ...
	Vec2 head = psnake->points[0];
	Vec2 next_pos = vec_add(head, dir);
	
	// if next_pos is apple; move all, and add new head; return APPLE
	if (vec_equal(next_pos, apple_pos))
	{
		psnake->size++;
		for (int i = psnake->size - 1; i > 0; --i)
		{
			psnake->points[i].x = psnake->points[i - 1].x;
			psnake->points[i].y = psnake->points[i - 1].y;
		}
		psnake->points[0] = next_pos;
		return APPLE;
	}

	// check snake size for WINNER; and return
	if (psnake->size == psnake->maxsize)
	{
		return WINNER;
	}
	
	// - move snake
	for (int i = psnake->size - 1; i > 0; --i)
	{
		psnake->points[i].x = psnake->points[i - 1].x;
		psnake->points[i].y = psnake->points[i - 1].y;
	}
	psnake->points[0] = next_pos;

	// if next_pos is outside or invalid(snake break) return game_over
	if (next_pos.x < 0 || next_pos.y < 0 || 
		next_pos.x >= screensize.x || next_pos.y >= screensize.y || 
		is_part_of_snake(psnake, next_pos))
	{
		return GAMEOVER;
	}

	// success
	return NONE;
}

int is_part_of_snake(Snake* psnake, Vec2 part)
{
	for (int i = 1; i < psnake->size; ++i)
	{
		if (vec_equal(psnake->points[i], part))
		{
			return 1;
		}
	}
	return 0;
}

void snake_free(Snake* psnake)
{
	if (psnake->points)
	{
		free(psnake->points);
	}
	psnake->maxsize = 0;
	psnake->size = 0;
}