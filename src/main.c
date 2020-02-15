#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include "cute_coroutine.h"

#ifdef _WIN32
	#include <windows.h>
	#include <conio.h>
	#define popen _popen
	#define pclose _pclose
	#define getch _getch
	#define kbhit _kbhit

	// At the time of writing, this define requires fairly recent windows version, so it's
	// safest to just define it ourselves... Should be harmless!
	#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
	#   define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
	#endif

	// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
	void turn_on_console_color()
	{
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD flags = 0;
		GetConsoleMode(h, &flags);
		flags |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(h, flags);
	}
#else
	#include <unistd.h>
	void turn_on_console_color() {}
	#define Sleep(ms) usleep((ms)*1000)
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

static char s_buffer[8*1024];
static char s_command[1024];
static int slide_reset;
static coroutine_t s_flush_co;
static coroutine_t s_parse_co;

void clear()
{
	s_buffer[0] = '\0';
	#ifdef _WIN32
	system("cls");
	#else
	system("clear");
	#endif
}

void pause(bool show_message)
{
	if (show_message) {
		printf("\033[36m\nPress any key to continue...\033[0m");
	}
	while (kbhit()) {
		getch();
	}
	int c = getch();
	if (c == 0 || c == 0xE0) c = getch();
	if (c == 75) {
		slide_reset = 1;
		return;
	}
	if (show_message) {
		printf("\n\n");
	}
}

void exec(const char* command, bool compiling)
{
	FILE* fp = popen(command, "r");
	char text[1024], *ptr = text;
	int c;
	while (c = fgetc(fp), c != EOF) {
		*ptr++ = (char)c;
	}
	*ptr = '\0';
	printf("%s", text);
	pclose(fp);

	if (compiling) {
		if ((ptr - text) == 0) {
			printf("\033[36mCompilation finished!\033[0m\n\n");
		}
	}
}

void compile()
{
	FILE* fp = fopen("temp.c", "w");
	fprintf(fp, "%s", s_buffer);
	fclose(fp);
	remove("temp.exe");
#ifdef _WIN32
	exec("tcc\\tcc temp.c -I slides -I src -o temp.exe 1> nul 2> nul", true); // tcc is already bundled in repo
#else
	exec("g++ temp.c -I slides -I src -o temp.exe", true); // this should work on osx/linux
#endif
	printf("\t\033[36m---\033[0m\n\n");
	exec("temp.exe", false);
	printf("\n\t\033[36m---\033[0m\n");
}

void do_command()
{
	for(int i = 0; s_command[i]; ++i) if(s_command[i]=='\r') s_command[i] = '\0'; // remove linefeeds
	const char* command = s_command;
	if (!strcmp(command, "@pause")) {
		pause(false);
	} else if (!strcmp(command, "@compile")) {
		compile();
	} else if (!strcmp(command, "@clear")) {
		clear();
	} else if (strstr(command, "@open")) {
		command += 6;
		FILE* fp = popen(command, "r");
		pclose(fp);
	} else if (!strcmp(command, "@color teal")) {
		printf("\033[36m");
	} else if (!strcmp(command, "@color normal")) {
		printf("\033[0m");
	} else if (!strcmp(command, "@color red")) {
		printf("\033[31m");
	} else if (!strcmp(command, "@color yellow")) {
		printf("\033[33m");
	}
	s_command[0] = '\0';
}

void parse(coroutine_t* co, char c)
{
	COROUTINE_START(co);

	COROUTINE_CASE(co, is_at_symbol);
	if (c == '@') goto read_command;
	else goto print_char;

	COROUTINE_CASE(co, read_command);
	if (c == '\n' || c == '\0') {
		do_command();
		COROUTINE_YIELD(co);
		goto is_at_symbol;
	} else {
		char s[2] = { c, '\0' };
		strcat(s_command, s);
		COROUTINE_YIELD(co);
		goto read_command;
	}

	COROUTINE_CASE(co, print_char);
	printf("%c", c);
	char s[2] = {c, '\0'};
	strcat(s_buffer, s);
	COROUTINE_YIELD(co);
	goto is_at_symbol;

	COROUTINE_END(co);
}

void print(coroutine_t* co, float dt, int char_frequency, const char* string)
{
	static int index;
	static char c;
	int milliseconds = rand() % 5 + char_frequency;

	COROUTINE_START(co);
	index = 0;

	COROUTINE_CASE(co, print_char);
	c = string[index++];
	COROUTINE_CALL(co, parse(&s_parse_co, c));
	COROUTINE_WAIT(co, milliseconds, dt);
	if (c) goto print_char;

	COROUTINE_END(co);
}

int flush_slide(float dt, int frequency_milliseconds, const char* string)
{
	coroutine_t* co = &s_flush_co;
	int keep_going = 1;
	int ms = frequency_milliseconds;

	COROUTINE_START(co);
	s_buffer[0] = '\0';
	COROUTINE_WAIT(co, ms, dt);
	COROUTINE_CALL(co, print(co, dt, ms, string));
	keep_going = 0;
	COROUTINE_END(co);

	return keep_going;
}

const char* read_file_to_memory_and_null_terminate(const char* path, size_t* size)
{
	char* data = 0;
	FILE* fp = fopen(path, "rb");
	int sz = 0;

	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		data = (char*)malloc(sz + 1);
		fread(data, sz, 1, fp);
		data[sz] = 0;
		fclose(fp);
	}

	if (size) *size = sz;
	return data;
}

int do_slides()
{
	static coroutine_t s_co;
	coroutine_t* co = &s_co;
	static char slide_path[256];
	static int slide_index = 0;
	static const char* buffer;
	int keep_going = 1;

	reset_slide_label:

	COROUTINE_START(co);

	COROUTINE_CASE(co, load_slide);
	sprintf(slide_path, "slides/slide%02d.c", slide_index++);
	free((void*)buffer);
	buffer = read_file_to_memory_and_null_terminate(slide_path, NULL);
	if (!buffer) keep_going = 0;
	else goto flush_slide_label;
	COROUTINE_EXIT(co);

	COROUTINE_CASE(co, flush_slide_label);
	while (1)
	{
		int keep_going = flush_slide(1, 5, buffer);
		if (!keep_going) break;
		Sleep(1);

		if (slide_reset) {
			clear();
			coroutine_init(co);
			coroutine_init(&s_flush_co);
			coroutine_init(&s_parse_co);
			--slide_index;
			slide_reset = 0;
			goto reset_slide_label;
		}
	}
	pause(true);
	clear();
	s_buffer[0] = '\0';
	COROUTINE_YIELD(co);
	goto load_slide;

	COROUTINE_END(co);

	return keep_going;
}

int main(int argc, const char* argv)
{
	turn_on_console_color();

	while (1)
	{
		int keep_going = do_slides();
		if (!keep_going) break;
	}

	remove("temp.c");
	remove("temp.exe");

	printf("End of slide show!\n");
	pause(true);

	return 0;
}
