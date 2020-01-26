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
