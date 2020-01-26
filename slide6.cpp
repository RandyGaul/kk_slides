New Concept
@pause

	CODE FLOW
@pause

>> The path of execution through a program. Like tracing a line
   from start to finish.
@pause

The simpler the code flow the better!
@pause
@clear
CODE FLOW is FRAGILE
And breaking code flow is...
@pause
@color red
EVIL
@color normal
@pause

Some examples that break code flow:
* goto statements
* switch statements
* calling into another process
* forking/spawning threads
* longjmp
* exceptions
@pause
@clear
State machines break code flow ;(
@pause

However, a coroutine can help.
@pause

Let use reimplement the Grass Enemy with a coroutine in C.
