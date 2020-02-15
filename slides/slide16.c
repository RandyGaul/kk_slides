@color yellow
#include <stdio.h>
#include <stdlib.h>
#include <cute_coroutine.h>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif
@color normal

void print(coroutine_t* co, float dt, const char* character_name, const char* string)
{
	static int index; // Not thread safe, but can easily be changed to a ptr as input param.
	int milliseconds = rand() % 10 + 20;
	char c = 0;

	COROUTINE_START(co);

	index = 0;
	printf("%-8s", character_name);
	COROUTINE_WAIT(co, 750, dt);

	COROUTINE_CASE(co, print_char);
	if (string[index]) {
		c = string[index++];
		printf("%c", c);
		if (c == '.' || c == ',' || c == '?') COROUTINE_WAIT(co, 250, dt);
		else COROUTINE_WAIT(co, milliseconds, dt);
		goto print_char;
	}

	COROUTINE_END(co);
}

int do_coroutine(coroutine_t* co, float dt)
{
	int keep_going = 1;
	int milliseconds = 1000;

	COROUTINE_START(co);
	COROUTINE_WAIT(co, milliseconds, dt);
	COROUTINE_CALL(co, print(co, dt, "Bob", "Yo Alice. I heard you like mudkips.\n"));
	COROUTINE_WAIT(co, milliseconds, dt);
	COROUTINE_CALL(co, print(co, dt, "Alice", "No Bob. Not me. Who told you such a thing?\n"));
	COROUTINE_WAIT(co, milliseconds, dt);
	COROUTINE_CALL(co, print(co, dt, "Bob", "Alice please, don't lie to me. We've known each other a long time.\n"));
	COROUTINE_WAIT(co, milliseconds, dt);
	COROUTINE_CALL(co, print(co, dt, "Alice", "We have grown apart. I barely know myself.\n"));
	COROUTINE_WAIT(co, milliseconds, dt);
	COROUTINE_CALL(co, print(co, dt, "Bob", "OK.\n"));
	COROUTINE_WAIT(co, milliseconds, dt);
	COROUTINE_CALL(co, print(co, dt, "Alice", "Good bye Bob. I wish you the best.\n"));
	COROUTINE_WAIT(co, milliseconds, dt);
	COROUTINE_CALL(co, print(co, dt, "Bob", "But do you like mudkips?\n"));
	COROUTINE_WAIT(co, milliseconds, dt);
	COROUTINE_CALL(co, print(co, dt, "Alice", "<has left>\n"));
	COROUTINE_WAIT(co, milliseconds, dt);
	COROUTINE_CALL(co, print(co, dt, "Bob", "Well, I like mudkips :)\n"));
	COROUTINE_WAIT(co, milliseconds, dt);
	keep_going = 0;
	COROUTINE_END(co);

	return keep_going;
}

int main()
{
	coroutine_t co;
	coroutine_init(&co);

	while (do_coroutine(&co, 1))
	{
		// Sleep for a second.
		#ifdef _WIN32
			Sleep(1);
		#else
			usleep(1000);
		#endif
		}

	system("pause");

	return 0;
}

@open assets\\coroutine_demo.gif