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
