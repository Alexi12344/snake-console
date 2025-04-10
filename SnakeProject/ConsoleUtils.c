#include <stdio.h>
#include <stdlib.h>
#include "ConsoleUtils.h"

#define INPUT_SIZE 256

HANDLE hConsoleInput;
HANDLE hConsoleOutput;
DWORD oldConsoleMode;

HANDLE hConsole;
COORD pixels_size;
COORD buffer_coord;
SMALL_RECT buffer_region;
CHAR_INFO* pixel_buffer;

INPUT_RECORD input_buffer[INPUT_SIZE];
BOOL key_input_buffer[INPUT_SIZE];
BOOL console_freed = FALSE;

int setup_console_handles()
{
	hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsole = CreateConsoleScreenBuffer(GENERIC_WRITE | GENERIC_READ, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (hConsole == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	SetConsoleActiveScreenBuffer(hConsole);
	return 1;
}

int setup_console_mode()
{
	/* DISABLE MOUSE CLICK IN CONSOLE */
	GetConsoleMode(hConsoleInput, &oldConsoleMode);
	if (!SetConsoleMode(hConsoleInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT))
	{
		return 0;
	}
	return 1;
}

int setup_console_fontinfo(SHORT fw, SHORT fh)
{
	CONSOLE_FONT_INFOEX fontInfoEx;
	fontInfoEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	GetCurrentConsoleFontEx(hConsole, FALSE, &fontInfoEx);
	fontInfoEx.FontFamily = FF_DONTCARE;
	fontInfoEx.FontWeight = FW_NORMAL;
	fontInfoEx.dwFontSize.X = fw;
	fontInfoEx.dwFontSize.Y = fh;
	if (!SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfoEx))
	{
		return 0;
	}
	return 1;
}

int setup_console_bufferinfo(SHORT* sw, SHORT* sh)
{
	CONSOLE_SCREEN_BUFFER_INFOEX bufferInfoEx;
	bufferInfoEx.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	GetConsoleScreenBufferInfoEx(hConsole, &bufferInfoEx);
	bufferInfoEx.srWindow.Right = bufferInfoEx.srWindow.Bottom;
	bufferInfoEx.dwSize.X = bufferInfoEx.srWindow.Bottom;
	bufferInfoEx.dwSize.Y = bufferInfoEx.srWindow.Bottom;
	*sw = bufferInfoEx.dwSize.X;
	*sh = bufferInfoEx.dwSize.Y;
	if (!SetConsoleScreenBufferInfoEx(hConsole, &bufferInfoEx))
	{
		return 0;
	}
	return 1;
}

int setup_console_cursor()
{
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = FALSE;
	if (!SetConsoleCursorInfo(hConsole, &cursorInfo))
	{
		return 0;
	}
	return 1;
}

int console_init()
{
	if (!setup_console_handles()) return 0;
	
	if (!setup_console_mode()) return 0;
	
	if (!setup_console_fontinfo(16, 16)) return 0;
	
	SHORT scrw, scrh;
	if (!setup_console_bufferinfo(&scrw, &scrh)) return 0;

	if (!setup_console_cursor()) return 0;

	console_setup_window();

	if (!setup_renderer(scrw, scrh)) return 0;
	
	return 1;
}

void console_setup_window()
{
	HWND consoleWindow = GetConsoleWindow();
	LONG state = GetWindowLong(consoleWindow, GWL_STYLE);
	SetWindowLong(consoleWindow, GWL_STYLE, state & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX & ~WS_MINIMIZEBOX);
}

void console_free()
{
	if (console_freed) return;
	console_freed = TRUE;

	if (pixel_buffer)
	{
		free(pixel_buffer);
	}

	// unstyle console window
	HWND consoleWindow = GetConsoleWindow();
	LONG state = GetWindowLong(consoleWindow, GWL_STYLE);
	SetWindowLong(consoleWindow, GWL_STYLE, state | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_MINIMIZEBOX);

	SetConsoleMode(hConsoleInput, oldConsoleMode);
	SetConsoleActiveScreenBuffer(hConsoleOutput);
	CloseHandle(hConsole);
}

int setup_renderer(SHORT width, SHORT height)
{
	pixels_size.X = width;
	pixels_size.Y = height;
	pixel_buffer = calloc(pixels_size.X * pixels_size.Y, sizeof(CHAR_INFO));

	if (!pixel_buffer)
	{
		return 0;
	}

	buffer_coord = (COORD){ 0,0 };
	buffer_region = (SMALL_RECT){ 0,0,pixels_size.X - 1, pixels_size.Y - 1 };
	return 1;
}

void read_events()
{
	memset(key_input_buffer, 0, INPUT_SIZE * sizeof(BOOL));

	DWORD numEvents;
	GetNumberOfConsoleInputEvents(hConsoleInput, &numEvents);
	if (numEvents > 0)
	{
		ReadConsoleInput(hConsoleInput, input_buffer, INPUT_SIZE, &numEvents);
	}

	for (DWORD i = 0; i < numEvents; ++i)
	{
		INPUT_RECORD current_ir = input_buffer[i];
		switch (current_ir.EventType)
		{
			case KEY_EVENT:
			{
				KEY_EVENT_RECORD key_ir = current_ir.Event.KeyEvent;
				key_input_buffer[key_ir.wVirtualKeyCode] |= key_ir.bKeyDown;
				break;
			}
		}
	}
}

BOOL is_key_pressed(int virtualKey)
{
	return key_input_buffer[virtualKey];
}

SHORT get_width()
{
	return pixels_size.X;
}

SHORT get_height()
{
	return pixels_size.Y;
}

void fill_pixels(WCHAR symbol, int color)
{
	for (SHORT y = 0; y < pixels_size.Y; ++y)
	{
		for (SHORT x = 0; x < pixels_size.X; ++x)
		{
			pixel_buffer[y * pixels_size.Y + x].Char.UnicodeChar = symbol;
			pixel_buffer[y * pixels_size.Y + x].Attributes = color;
		}
	}
}

void draw_pixel(SHORT x, SHORT y, WCHAR symbol, int color)
{
	if (x < 0 || x > pixels_size.X || y < 0 || y > pixels_size.Y) return;
	pixel_buffer[y * pixels_size.Y + x].Char.UnicodeChar = symbol;
	pixel_buffer[y * pixels_size.Y + x].Attributes = color;
}

void render_present()
{
	WriteConsoleOutput(hConsole, pixel_buffer, pixels_size, buffer_coord, &buffer_region);
}

void fill_rect(SHORT x, SHORT y, SHORT w, SHORT h, WCHAR symbol, int color)
{
	for (SHORT yy = y; yy < (y + h); ++yy)
	{
		for (SHORT xx = x; xx < (x + w); ++xx)
		{
			draw_pixel(xx, yy, symbol, color);
		}
	}
}

void draw_text(SHORT x, SHORT y, const WCHAR* text, int color)
{
	int length = wcslen(text);
	for (int i = 0; i<length; ++i)
	{
		draw_pixel(x + i, y, text[i], color);
	}
}

void draw_text2(SHORT x, SHORT y, const WCHAR* text, size_t textlen, int color)
{
	for (int i = 0; i < textlen; ++i)
	{
		draw_pixel(x + i, y, text[i], color);
	}
}