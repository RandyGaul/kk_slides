        ~ Implementing Coroutines in C ~
           --------------------------

>> There are a myriad of ways, but I like Duff's Device
@pause
>> https://en.wikipedia.org/wiki/Duff%27s_device

Key functionality:
 * Pause/resume
@pause

Some psuedo-code:

	Do Step A
@color red
	Wait for X seconds
@color normal
	Do Step B
@color red
	Wait for Y seconds
@color normal
	Do Step C
@color red
	Start over
@color normal
@pause

A decent implementation in C:
>> https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
