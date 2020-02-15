Canonical Implementation
@pause

---

struct GrassEnemy
{
	float time;
	int state;
@color yellow
	// all other stuff here...
	// like animation data...
@color normal
};
@pause

enum GRASS_ENEMY_STATE
{
	GRASS_ENEMY_STATE_HIDE,
	GRASS_ENEMY_STATE_THROWING,
	GRASS_ENEMY_STATE_POST_THROW,
};
@pause

@color yellow
#define GRASS_ENEMY_HIDE_TIME 3.0f
#define GRASS_ENEMY_COCK_SPEAR_TIME 0.25f
#define GRASS_ENEMY_POST_THROW_TIME 1.0f
@color normal
@pause

void goto_state(GrassEnemy* ge, int state)
{
	ge->state = state;
	ge->time = 0;
}

void update_grass_enemy(GrassEnemy* ge, float dt)
@pause
{
	ge->time += dt;

	switch (ge->state)
	{
	case GRASS_ENEMY_STATE_HIDE:
		if (ge->time == 0) play_animation(ge->grass_popout);
		else if (ge->time > GRASS_ENEMY_HIDE_TIME) {
			goto_state(ge, GRASS_ENEMY_STATE_THROWING);
		}
		break

	case GRASS_ENEMY_STATE_THROWING:
		if (ge->time == 0) {
			int dir = which_way_to_face_link(ge->position);
			face_link(ge->animation_data, dir);
		} else if (ge->time > GRASS_ENEMY_COCK_SPEAR_TIME) {
			throw_spear(ge);
			goto_state(ge, GRASS_ENEMY_STATE_POST_THROW);
		}
		break;

	case GRASS_ENEMY_STATE_POST_THROW:
		if (ge->time == 0) play_animation(ge->grass_post_throw);
		else if (ge->time > GRASS_ENEMY_POST_THROW_TIME) {
			goto_state(ge, GRASS_ENEMY_STATE_HIDE);
		}
		break;
	}
}
@pause
@clear
Basically a huge amount of work for one enemy.
@pause
.
@pause
.
@pause
.
@pause
Now do it for all of them...
@pause

There's also another problem.
@pause

   FRAME DELAY
@pause

The switch statement performs one "jump", so changing states requires
the state machine function to be called again next frame, introducing
@color red
one frame lag, or one tick lag.
@color normal
@pause

This is a non-problem for the coroutines shown later!

:)
