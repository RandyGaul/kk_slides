struct GrassEnemy
{
	float time;
@color red
	coroutine_t co;
@color yellow
	// all other stuff here...
	// like animation data...
@color normal
};

void update_grass_enemy(GrassEnemy* ge, float dt)
{
	coroutine_t* co = &ge->co;
	COROUTINE_START(co);

	COROUTINE_CASE(co, hide_in_grass);
	play_animation(ge->grass_popout);
	COROUTINE_WAIT(co, 3.0f);
	int dir = which_way_to_face_link(ge->position);
	face_link(ge->animation_data, dir);
	COROUTINE_WAIT(co, 0.25f);
	throw_spear(ge);
	play_animation(ge->grass_post_throw);
	COROUTINE_WAIT(co, 1.0f);
	goto hide_in_grass;

	COROUTINE_END(co);
}
