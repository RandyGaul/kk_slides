#include <stdio.h>

int run();

int main(void)
{
	while (1) {
		int keep_going = run();
		if (!keep_going) break;
	}

	return 0;
}
@pause

int run()
{
@color red
	static int state;
@color normal
	int keep_going = 1;

	if (state == 0) {
		printf("State 0\n");
		++state;
	} else if (state == 1) {
		printf("State 1\n");
		++state;
	} else if (state == 2) {
		printf("State 2\n");
@color red
		state = 0;
		keep_going = 0;
@color normal
	}

	return keep_going;
}

@compile