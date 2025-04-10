#pragma once
#ifndef SNAKE_H
#define SNAKE_H

#include "Vec2.h"

typedef struct _Snake
{
	Vec2* points;
	int size;
	int maxsize;
} Snake;

typedef enum _SnakeState
{
	NONE = 0,
	APPLE = 1,
	GAMEOVER = 2,
	WINNER = 3
} SnakeState;

typedef enum _SnakeDir
{
	NONE_DIR = -1,
	LEFT_DIR = 0,
	UP_DIR = 1,
	RIGHT_DIR = 2,
	DOWN_DIR = 3
} SnakeDir;

int snake_init(Snake* psnake, int max_size, Vec2 initial_pos);
SnakeState snake_step(Snake* psnake, Vec2 dir, Vec2 screensize, Vec2 apple_pos);
int is_part_of_snake(Snake* psnake, Vec2 part);
void snake_free(Snake* psnake);

#endif // SNAKE_H