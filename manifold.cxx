#include "manifold.hxx"

void SquareKleinBottle::shift_x(PointState &state, int dir)
{
	state.pos[0] += 2 * dir * radius;
}

void SquareKleinBottle::shift_y(PointState &state, int dir)
{
	state.pos[1] += 2 * dir * radius;
	state.pos[0] = -state.pos[0];
	state.vel[0] = -state.vel[0];
	state.mirror = !state.mirror;
}

void SquareKleinBottle::stepState(PointState &state, double dt)
{
	state.pos += dt * state.vel;
	if(state.pos[0] <= -radius)	shift_x(state, +1);
	if(state.pos[0] >= radius)	shift_x(state, -1);
	if(state.pos[1] <= -radius)	shift_y(state, +1);
	if(state.pos[1] >= radius)	shift_y(state, -1);
}

void SquareKleinBottle::stepState(BodyState &state, double dt)
{
	stepState((PointState &)state, dt);
	state.rpos += dt * state.rvel;
}

void SquareKleinBottle::remap(BodyState const &base, BodyState &state)
{
	if(std::abs(state.pos[1] - base.pos[1]) > radius)
		shift_y(state, state.pos[1] < base.pos[1] ? +1 : -1);
	if(std::abs(state.pos[0] - base.pos[0]) > radius)
		shift_x(state, state.pos[0] < base.pos[0] ? +1 : -1);
}

void SquareKleinBottle::relativize(BodyState const &base, BodyState &state)
{
	remap(base, state);
	double c = std::cos(base.rpos);
	double s = std::sin(base.rpos);
	Eigen::Matrix2d rot;
	Eigen::Matrix2d rot2;
	state.mirror ^= base.mirror;

	state.pos -= base.pos;
	state.vel -= base.vel;
	state.rpos -= base.rpos;
	state.rvel -= base.rvel;

	if(base.mirror)
	{
		rot << -c, s, s, c;
	}
	else
	{
		rot << c, s, -s, c;
	}

	state.pos = rot * state.pos;
	state.vel = rot * state.vel;
	if(state.mirror)
	{
		state.rpos = -state.rpos;
		state.rvel = -state.rvel;
	}
}
