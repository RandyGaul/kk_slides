#include "main.h"
#include "coroutine.h"

int run()
{
	static coroutine_t s_co;
	coroutine_t* co = &s_co;
	int keep_going = 1;

@color red
	float dt = 0.25f;
	static float time;
	time += dt;
	printf("Time: %f\n", time);
@color normal

	COROUTINE_START(co);

	printf("State 0\n");
	COROUTINE_WAIT(co, 1.0f, dt);
	printf("State 1\n");
	COROUTINE_WAIT(co, 2.0f, dt);
	printf("State 2\n");
	COROUTINE_WAIT(co, 3.0f, dt);
	keep_going = 0;

	COROUTINE_END(co);

	return keep_going;
}

@pause
@compile