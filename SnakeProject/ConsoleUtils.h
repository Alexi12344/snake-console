#pragma once
#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

int setup_console_handles();
int setup_console_mode();
int setup_console_fontinfo(SHORT fw, SHORT fh);
int setup_console_bufferinfo(SHORT *sw, SHORT *sh);
int setup_console_cursor();
int console_init();
void console_setup_window();
void console_free();
int setup_renderer(SHORT width, SHORT height);
void read_events();
BOOL is_key_pressed(int virtualKey);

SHORT get_width();
SHORT get_height();
void fill_pixels(WCHAR symbol, int color);
void draw_pixel(SHORT x, SHORT y, WCHAR symbol, int color);
void fill_rect(SHORT x, SHORT y, SHORT w, SHORT h, WCHAR symbol, int color);
void draw_text(SHORT x, SHORT y, const WCHAR* text, int color);
void draw_text2(SHORT x, SHORT y, const WCHAR* text, size_t textlen, int color);
void render_present();

#endif // CONSOLE_UTILS_H