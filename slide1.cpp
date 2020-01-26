               ~ Key Point ~
                 ---------
>> Coroutines are great for state machines.
@pause

This talk is about state machines.
@pause

:)
@pause

For example...
@pause
@clear
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
