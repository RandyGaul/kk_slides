struct coroutine_t
{
	float elapsed;
	int index;
};

#define COROUTINE_START(co) do { switch (co->index) { default:
#define COROUTINE_CASE(co, name) case __LINE__: name: co->index = __LINE__;
#define COROUTINE_EXIT(co) do { goto __co_end; } while (0)
#define COROUTINE_YIELD(co) do { co->index = __LINE__; COROUTINE_EXIT(co); case __LINE__:; } while (0)
#define COROUTINE_WAIT(co, time, dt) do { case __LINE__: co->index = __LINE__; co->elapsed += dt; do { if (co->elapsed < time) { goto __co_end; } else { co->elapsed = 0; } } while (0); } while (0)
#define COROUTINE_END(co) } co->index = 0; __co_end:; } while (0)
