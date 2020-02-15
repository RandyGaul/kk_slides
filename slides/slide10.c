#include "main.h"

int run()
{
@color red
	static int state;
@color normal
	int keep_going = 1;

	switch (state)
	{
	case 0:
		printf("State 0\n");
		++state;
		break;

	case 1:
		printf("State 1\n");
		++state;
		break;

	case 2:
		printf("State 2\n");
@color red
		state = 0;
		keep_going = 0;
@color normal
		break;
	}

	return keep_going;
}

@compile