Pros/Cons of the Macro Strategy
@pause

Pros
@pause
* Small dependency
@pause
* Easy to understand perf costs
@pause
* Pretty simple overall
@pause

Cons
@pause
* Uses macros, thus has an abstraction cost
@pause
* Local variables are difficult
@pause
* Cannot yield from nested or multiple levels of coroutines
@pause

Con Mitigations
@pause
* Possible to add a "stack" to the coroutine.
  Pop off local variables from a fixed-sized table.
@pause
* Possible to add explicit support for nested or multi-level coroutines.
  I did this one in my own coroutine header!
@pause

@color yellow
>> COROUTINE_CALL(co, call_this_func(co, any_params_you_like));
@color normal
@pause

Possible to yield from multiple functions, or nest entirely separate
coroutines. So traditional objections to macro-style sort of just fall
apart if extra features are added.
@pause

LOCAL VARIABLES
@pause

void do_work(coroutine_t* co)
{
@color yellow
	// Setup locals onto the CRT (C runtime) stack.
@color normal
	int* a_ptr = (int*)coroutine_local_var(co, sizeof(int));
	int* b_ptr = (int*)coroutine_local_var(co, sizeof(int));
	int a = *a_ptr;
	int b = *b_ptr;

	COROUTINE_START(co);
	// ...
	COROUTINE_END(co);

@color yellow
	// Push locals back onto the coroutine's stack.
@color normal
	*a_ptr = a;
	*b_ptr = b;
}
@pause

>> Fixed size buffer to allocate small local variables!
@pause

static inline void* coroutine_local_var(coroutine_t* co, size_t size)
{
	COROUTINE_ASSERT(co->stack_pointer + size < COROUTINE_STACK_SIZE);
	void* ptr = co->stack + co->stack_pointer;
	co->stack_pointer += size;
	return ptr;
}
@pause
@clear
#include <stdio.h>
#include "cute_coroutine.h"

void do_work(coroutine_t* co)
{
@color yellow
	// Setup locals onto the CRT (C runtime) stack.
@color normal
	int* a_ptr = (int*)coroutine_local_var(co, sizeof(int));
	int* b_ptr = (int*)coroutine_local_var(co, sizeof(int));
	int a = *a_ptr;
	int b = *b_ptr;

	COROUTINE_START(co);
	a = 2;
	b = 0;

	COROUTINE_YIELD(co);
	--a;
	++b;
	COROUTINE_YIELD(co);
	--a;
	++b;
	COROUTINE_YIELD(co);
	--a;
	++b;

	COROUTINE_END(co);

	printf("a : %d, b : %d\n", a, b);

@color yellow
	// Push locals back onto the coroutine's stack.
@color normal
	*a_ptr = a;
	*b_ptr = b;
}

int main()
{
	coroutine_t co;
	coroutine_init(&co);

	for (int i = 0; i < 3; ++i) {
		do_work(&co);
	}

	return 0;
}

@compile
@pause
@clear
#include <stdio.h>
#include "cute_coroutine.h"

void print_abc(coroutine_t* co)
{
	COROUTINE_START(co);
	printf("a\n");
@color red
	COROUTINE_YIELD(co);
@color normal
	printf("b\n");
@color red
	COROUTINE_YIELD(co);
@color normal
	printf("c\n");
	COROUTINE_END(co);
}

int do_work(coroutine_t* co)
{
	int* i_ptr = (int*)coroutine_local_var(co, sizeof(int));
	int i = *i_ptr;
	int keep_going = 1;

	COROUTINE_START(co);

	printf("---\n");
	for (i = 0; i < 6; ++i) {
@color red
		COROUTINE_CALL(co, print_abc(co));
@color normal
		COROUTINE_YIELD(co);
		printf("---\n");
	}
	keep_going = 0;

	COROUTINE_END(co);

	*i_ptr = i;
	return keep_going;
}

int main()
{
	coroutine_t co;
	coroutine_init(&co);

	while (1) {
		int keep_going = do_work(&co);
		if (!keep_going) break;
	}

	return 0;
}

@compile