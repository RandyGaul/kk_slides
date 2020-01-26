A Closer Look at the Macros
@pause

@color teal
>> __LINE__ is a preprocessor macro that expands to current
   line number in the source file, as an integer.
@color normal
@pause

Can capture the line number of a case statement.
@pause
Also capture the line number in a variable.
@pause

@color yellow
   #define CASE(goto_label) case __LINE__: goto_label: state = __LINE__
@color normal
@pause
@clear
@color yellow
#include <stdio.h>

#define CASE(goto_label) case __LINE__: goto_label: state = __LINE__
#define YIELD() state = __LINE__; goto end; case __LINE__:
@color normal

void cooperative_state_machine()
{
	static int state;

	switch (state)
	{
		default:

@color red
		// Code here will run *once*
		printf("~ A Tale of Two People ~\n");
@color normal

		CASE(start);
		printf("Person A: Good bye...\n");
		YIELD();
		goto no_wait;

		CASE(no_wait);
		printf("Person B: No wait, come back!\n");
		YIELD();
		goto well_ok;

		CASE(well_ok);
		printf("Person A: Well, idk. I think I'll still leave.\n");
		YIELD();
		goto start;
	}

	end:;
}

int main()
{
	for (int i = 0; i < 9; ++i)
		cooperative_state_machine();

	return 0;
}

@compile