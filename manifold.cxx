#include "manifold.hxx"

bool SquareKleinBottle::shift_x(PointState &state, int dir) const
{
	state.pos[0] += 2 * dir * radius;
	return false;
}

bool SquareKleinBottle::shift_y(PointState &state, int dir) const
{
	state.pos[1] += 2 * dir * radius;
	state.pos[0] = -state.pos[0];
	state.vel[0] = -state.vel[0];
	return true;
}

bool SquareKleinBottle::stepState_base(PointState &state, double dt) const
{
	bool result = false;
	state.pos += dt * state.vel;
	if(state.pos[0] <= -radius)	result ^= shift_x(state, +1);
	if(state.pos[0] >= radius)	result ^= shift_x(state, -1);
	if(state.pos[1] <= -radius)	result ^= shift_y(state, +1);
	if(state.pos[1] >= radius)	result ^= shift_y(state, -1);
	return result;
}

void SquareKleinBottle::stepState(PointState &state, double dt) const
{
	stepState_base(state, dt);
}

void SquareKleinBottle::stepState(BodyState &state, double dt) const
{
	state.mirror ^= stepState_base(state, dt);
	state.rpos += dt * state.rvel;
}

bool SquareKleinBottle::remap_base(BodyState const &base, PointState &state) const
{
	bool f = false;
	if(std::abs(state.pos[1] - base.pos[1]) > radius)
		f ^= shift_y(state, state.pos[1] < base.pos[1] ? +1 : -1);
	if(std::abs(state.pos[0] - base.pos[0]) > radius)
		f ^= shift_x(state, state.pos[0] < base.pos[0] ? +1 : -1);
	return f;
}

void SquareKleinBottle::remap(BodyState const &base, PointState &state) const
{
	remap_base(base, state);
}

void SquareKleinBottle::remap(BodyState const &base, BodyState &state) const
{
	state.mirror ^= remap_base(base, state);
}

void SquareKleinBottle::relativize(BodyState const &base, PointState &state) const
{
	remap(base, state);
	double c = std::cos(base.rpos);
	double s = std::sin(base.rpos);
	Eigen::Matrix2d rot;

	state.pos -= base.pos;
	state.vel -= base.vel;

	if(base.mirror)
		rot << -c, s, s, c;
	else
		rot << c, s, -s, c;

	state.pos = rot * state.pos;
	state.vel = rot * state.vel;
}

void SquareKleinBottle::relativize(BodyState const &base, BodyState &state) const
{
	relativize(base, static_cast<PointState &>(state));
	state.mirror ^= base.mirror;
	state.rpos -= base.rpos;
	state.rvel -= base.rvel;
	if(state.mirror)
	{
		state.rpos = -state.rpos;
		state.rvel = -state.rvel;
	}
}
