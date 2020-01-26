#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include "cute_coroutine.h"

#ifdef _MSC_VER
	#include <windows.h>
	#include <conio.h>
	#define popen _popen
	#define getch _getch

	// At the time of writing, this define requires fairly recent windows version, so it's
	// safest to just define it ourselves... Should be harmless!
	#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
	#	define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
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
#endif

#include <stdio.h>
#include <string>

static std::string s_buffer;
static std::string s_command;

void clear()
{
	s_buffer.clear();
	system("cls");
}

void pause(bool show_message = true)
{
	if (show_message) {
		printf("\033[36m\nPress any key to continue...\033[0m");
	}
	int c = getch();
	if (c == 0 || c == 0xE0) _getch();
	if (show_message) {
		printf("\n\n");
	}
}

void exec(const char* command, bool compiling = false)
{
	FILE* fp = popen(command, "r");
	std::string text;
	int c;
	while (c = fgetc(fp), c != EOF) {
		text.push_back((char)c);
	}
	printf("%s", text.c_str());
	fclose(fp);

	if (compiling) {
		if (text.length() == 0) {
			printf("\033[36mCompilation finished!\033[0m\n\n");
		}
	}
}

void compile()
{
	FILE* fp = fopen("slide.cpp", "w");
	fprintf(fp, "%s", s_buffer.c_str());
	fclose(fp);
	remove("slide.exe");
	exec("g++ slide.cpp -o slide.exe", true);
	printf("\t\033[36m---\033[0m\n\n");
	exec("slide.exe");
	printf("\n\t\033[36m---\033[0m\n");
}

void do_command()
{
	const char* command = s_command.c_str();
	if (!strcmp(command, "@pause")) {
		pause(false);
	} else if (!strcmp(command, "@compile")) {
		compile();
	} else if (!strcmp(command, "@clear")) {
		clear();
	} else if (strstr(command, "@open")) {
		command += 6;
		FILE* fp = popen(command, "r");
		fclose(fp);
	} else if (!strcmp(command, "@color teal")) {
		printf("\033[36m");
	} else if (!strcmp(command, "@color normal")) {
		printf("\033[0m");
	} else if (!strcmp(command, "@color red")) {
		printf("\033[31m");
	} else if (!strcmp(command, "@color yellow")) {
		printf("\033[33m");
	}
	s_command.clear();
}

void parse(coroutine_t* co, char c)
{
	const char* color_code = NULL;

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
		s_command.push_back(c);
		COROUTINE_YIELD(co);
		goto read_command;
	}

	COROUTINE_CASE(co, print_char);
	printf("%c", c);
	s_buffer.push_back(c);
	COROUTINE_YIELD(co);
	goto is_at_symbol;

	COROUTINE_END(co);
}

void print(coroutine_t* co, float dt, int char_frequency, const char* string)
{
	static int index;
	static coroutine_t parse_co;
	static char c;
	int milliseconds = rand() % 5 + char_frequency;

	COROUTINE_START(co);
	index = 0;

	COROUTINE_CASE(co, print_char);
	c = string[index++];
	COROUTINE_CALL(co, parse(&parse_co, c));
	COROUTINE_WAIT(co, milliseconds, dt);
	if (c) goto print_char;

	COROUTINE_END(co);
}

int flush_slide(float dt, int frequency_milliseconds, const char* string)
{
	static coroutine_t s_co;
	coroutine_t* co = &s_co;
	int keep_going = 1;
	int ms = frequency_milliseconds;

	COROUTINE_START(co);
	s_buffer.clear();
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
	static int slide_index = 0;
	static char slide_path[256];
	static const char* buffer;
	int keep_going = 1;

	COROUTINE_START(co);

	COROUTINE_CASE(co, load_slide);
	sprintf(slide_path, "slide%d.cpp", slide_index++);
	free((void*)buffer);
	buffer = read_file_to_memory_and_null_terminate(slide_path, NULL);
	if (!buffer) keep_going = 0;
	else goto flush_slide;
	COROUTINE_EXIT(co);

	COROUTINE_CASE(co, flush_slide);
	while (1)
	{
		int keep_going = flush_slide(1, 5, buffer);
		if (!keep_going) break;
		#ifdef _MSC_VER
			Sleep(1);
		#else
			usleep(1000);
		#endif
	}
	pause();
	clear();
	s_buffer.clear();
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

	remove("slide.cpp");
	remove("slide.exe");

	printf("End of slide show!\n");
	pause();

	return 0;
}
