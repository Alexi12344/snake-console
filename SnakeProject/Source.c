#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <string.h>

#include "ConsoleUtils.h"
#include "Vec2.h"
#include "Snake.h"
#include "Apple.h"

#define FPS 10
#define FRAMETIME (1000/FPS)

typedef enum _Screen
{
	SNAKE_SCR = 0,
	GAMEOVER_SCR = 1,
	WINNER_SCR = 2
} Screen;

BOOL WINAPI ConsoleCtrlHandler(DWORD CtrlType);
void snake_draw(Snake* psnake);
void string_rotate(WCHAR* text, size_t length);

int isRunning = 0;

int main()
{
	if (console_init())
	{
		SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
		srand((unsigned int)time(NULL));
		isRunning = 1;
	}
	else {
		printf("[ERROR] Failed to setup console\n");
		return -1;
	}

	int snake_first_time = 1;
	SHORT snakescr_w = get_width(), snakescr_h = get_height()-2;
	int max_snake_size = snakescr_w * snakescr_h;
	Vec2 screen_size = (Vec2){ snakescr_w, snakescr_h };
	Vec2 initial_pos = { 4, snakescr_h / 2 };
	Snake snake_obj;
	if (!snake_init(&snake_obj, max_snake_size, initial_pos))
	{
		console_free();
		printf("[ERROR] Failed to allocate snake buffer\n");
		return -1;
	}

	Vec2 vdir = { 0,0 }; SnakeDir edir = NONE_DIR;
	int dir_input[4] = { 0 };

	Vec2 apple_pos = generate_apple_pos(snakescr_w, snakescr_h);

	int score = 0;
	WCHAR* score_format = L"SCORE: %d\0";
	WCHAR score_text[100] = {0};

	int highest_score = 0;
	WCHAR* highest_score_format = L"HIGHEST SCORE: %d\0";
	WCHAR highest_score_text[100] = { 0 };

	Screen current_screen = SNAKE_SCR;

	// buffer used for gameover/winner screen
	size_t gowbuffer_size = get_width();
	WCHAR* gowbuffer = calloc(gowbuffer_size, sizeof(WCHAR));
	if (!gowbuffer)
	{
		snake_free(&snake_obj);
		console_free();
		printf("[ERROR]: failed to allocate game over / winner buffer\n");
		return -1;
	}

	SetConsoleTitle(L"SNAKE GAME");

	ULONGLONG prev = GetTickCount64(), curr = 0, elapsed = 0;
	while (isRunning)
	{
		curr = GetTickCount64();
		elapsed = curr - prev;

		read_events();
		if (is_key_pressed(VK_ESCAPE))
		{
			isRunning = 0;
			break;
		}

		if (is_key_pressed(VK_UP))
		{
			dir_input[UP_DIR] = 1;
		}
		if (is_key_pressed(VK_LEFT) && !snake_first_time)
		{
			dir_input[LEFT_DIR] = 1;
		}
		if (is_key_pressed(VK_RIGHT))
		{
			dir_input[RIGHT_DIR] = 1;
		}
		if (is_key_pressed(VK_DOWN))
		{
			dir_input[DOWN_DIR] = 1;
		}

		if (is_key_pressed(0x52)) // R
		{
			if (current_screen == GAMEOVER_SCR || current_screen == WINNER_SCR)
			{
				current_screen = SNAKE_SCR;
				snake_first_time = 1;
				snake_free(&snake_obj);
				if (!snake_init(&snake_obj, max_snake_size, initial_pos))
				{
					free(gowbuffer);
					console_free();
					printf("[ERROR] Failed to allocate snake buffer\n");
					return -1;
				}
				
				vdir = (Vec2){0,0};
				edir = NONE_DIR;
				score = 0;
			}
		}

		if (elapsed < FRAMETIME) continue; // skip render

		if (current_screen == SNAKE_SCR)
		{
			// snake input validation
			if (dir_input[LEFT_DIR] && !dir_input[RIGHT_DIR] && edir != RIGHT_DIR)
			{
				vdir.x = -1;
				vdir.y = 0;
				edir = LEFT_DIR;
				snake_first_time = 0;
			}
			else if (dir_input[UP_DIR] && !dir_input[DOWN_DIR] && edir != DOWN_DIR)
			{
				vdir.x = 0;
				vdir.y = -1;
				edir = UP_DIR;
				snake_first_time = 0;
			}
			else if (dir_input[RIGHT_DIR] && !dir_input[LEFT_DIR] && edir != LEFT_DIR)
			{
				vdir.x = 1;
				vdir.y = 0;
				edir = RIGHT_DIR;
				snake_first_time = 0;
			}
			else if (dir_input[DOWN_DIR] && !dir_input[UP_DIR] && edir != UP_DIR)
			{
				vdir.x = 0;
				vdir.y = 1;
				edir = DOWN_DIR;
				snake_first_time = 0;
			}
		}
		
		fill_pixels(L' ', 0);

		if (current_screen == SNAKE_SCR)
		{
			// snake drawing
			fill_rect(0, 0, snakescr_w, snakescr_h, L' ', BACKGROUND_GREEN);
			snake_draw(&snake_obj);
			draw_pixel(apple_pos.x, apple_pos.y, L' ', BACKGROUND_RED);

			memset(score_text, 0, sizeof(WCHAR) * 100);
			wsprintf(score_text, score_format, score);
			draw_text(0, get_height() - 1, score_text, FOREGROUND_INTENSITY);

			memset(highest_score_text, 0, sizeof(WCHAR) * 100);
			wsprintf(highest_score_text, highest_score_format, highest_score);
			draw_text(0, get_height() - 2, highest_score_text, FOREGROUND_INTENSITY);
		}
		else if (current_screen == GAMEOVER_SCR)
		{
			draw_text(0, get_height() - 1, score_text, FOREGROUND_INTENSITY);
			draw_text(0, get_height() - 2, highest_score_text, FOREGROUND_INTENSITY);
			draw_text2(0, get_height()/2, gowbuffer, gowbuffer_size, FOREGROUND_INTENSITY);
			string_rotate(gowbuffer, gowbuffer_size);
		}
		else if (current_screen == WINNER_SCR)
		{
			draw_text(0, get_height() - 1, score_text, FOREGROUND_INTENSITY);
			draw_text(0, get_height() - 2, highest_score_text, FOREGROUND_INTENSITY);
			draw_text2(0, get_height() / 2, gowbuffer, gowbuffer_size, FOREGROUND_INTENSITY);
			string_rotate(gowbuffer, gowbuffer_size);
		}
		
		render_present();
		
		// state validation
		if (current_screen == SNAKE_SCR && edir != NONE_DIR)
		{
			SnakeState step_result = snake_step(&snake_obj, vdir, screen_size, apple_pos);
			switch (step_result)
			{
				case APPLE:
				{
					// generate new apple pos
					apple_pos = generate_apple_pos(snakescr_w, snakescr_h);
					score++;
					if (score > highest_score)
					{
						highest_score = score;
					}
					break;
				}
				case GAMEOVER:
				{
					current_screen = GAMEOVER_SCR;
					memset(gowbuffer, 0, sizeof(WCHAR)* gowbuffer_size);
					wsprintf(gowbuffer, L"GAME OVER!");
					break;
				}
				case WINNER:
				{
					current_screen = WINNER_SCR;
					memset(gowbuffer, 0, sizeof(WCHAR) * gowbuffer_size);
					wsprintf(gowbuffer, L"YOU WIN!");
					break;
				}
				case NONE: { break; }
				default:
				{
					SetConsoleTitle(L"Error: step_result");
					break;
				}
			}
		}

		prev = GetTickCount64(); // get actual time NOW
		for (int i = 0; i < 4; ++i) dir_input[i] = 0;
		//Sleep(FRAMETIME);
	}

	free(gowbuffer);
	snake_free(&snake_obj);
	console_free();

	return 0;
}

BOOL WINAPI ConsoleCtrlHandler(DWORD CtrlType)
{
	if (CtrlType == CTRL_CLOSE_EVENT)
	{
		isRunning = 0;
		return TRUE;
	}
	return FALSE;
}

void snake_draw(Snake* psnake)
{
	WCHAR symbol = L' ';
	int color = BACKGROUND_BLUE;
	for (int i = 0; i < psnake->size; ++i)
	{
		Vec2 curr_pos = psnake->points[i];
		if (i == 0)
		{
			symbol = L'#';
		}
		else {
			symbol = L' ';
		}
		draw_pixel(curr_pos.x, curr_pos.y, symbol, color);
	}
}

void string_rotate(WCHAR* text, size_t length)
{
	if (length == 0) return;
	WCHAR first = text[0];
	for (size_t i = 1; i < length; ++i)
	{
		text[i - 1] = text[i];
	}
	text[length - 1] = first;
}