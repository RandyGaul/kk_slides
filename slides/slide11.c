#include "main.h"

typedef struct coroutine_t
{
	float elapsed;
	int index;
} coroutine_t;

@color yellow
#define COROUTINE_START(co) do { switch (co->index) { default:
#define COROUTINE_CASE(co, name) case __LINE__: name: co->index = __LINE__;
#define COROUTINE_EXIT(co) do { goto __co_end; } while (0)
#define COROUTINE_YIELD(co) do { co->index = __LINE__; COROUTINE_EXIT(co); case __LINE__:; } while (0)
#define COROUTINE_WAIT(co, time, dt) do { case __LINE__: co->index = __LINE__; co->elapsed += dt; do { if (co->elapsed < time) { goto __co_end; } else { co->elapsed = 0; } } while (0); } while (0)
#define COROUTINE_END(co) } co->index = 0; __co_end:; } while (0)
@color normal
@pause

int run()
{
@color red
	static coroutine_t s_co;
	coroutine_t* co = &s_co;
@color normal
	static int loop_count = 0;
	int keep_going = 1;

	COROUTINE_START(co);

	COROUTINE_CASE(co, loop_start);
	printf("State 0\n");
	COROUTINE_YIELD(co);
	printf("State 1\n");
	COROUTINE_YIELD(co);
	printf("State 2\n");
	COROUTINE_YIELD(co);
@color red
	++loop_count;
	if (loop_count == 3) {
		keep_going = 0;
	}
@color normal
	goto loop_start;

	COROUTINE_END(co);

	return keep_going;
}

@pause
@compile