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

bool SquareKleinBottle::stepState_base(PointState &state, Float dt) const
{
	bool result = false;
	state.pos += dt * state.vel;
	if(state.pos[0] <= -radius)	result ^= shift_x(state, +1);
	if(state.pos[0] >= radius)	result ^= shift_x(state, -1);
	if(state.pos[1] <= -radius)	result ^= shift_y(state, +1);
	if(state.pos[1] >= radius)	result ^= shift_y(state, -1);
	return result;
}

void SquareKleinBottle::stepState(PointState &state, Float dt) const
{
	stepState_base(state, dt);
}

void SquareKleinBottle::stepState(BodyState &state, Float dt) const
{
	state.mirror ^= stepState_base(state, dt);
	state.rpos += dt * state.rvel;
}

bool SquareKleinBottle::remap_base(PointState const &base, PointState &state) const
{
	bool f = false;
	if(std::abs(state.pos[1] - base.pos[1]) > radius)
		f ^= shift_y(state, state.pos[1] < base.pos[1] ? +1 : -1);
	if(std::abs(state.pos[0] - base.pos[0]) > radius)
		f ^= shift_x(state, state.pos[0] < base.pos[0] ? +1 : -1);
	return f;
}

void SquareKleinBottle::remap(PointState const &base, PointState &state) const
{
	remap_base(base, state);
}

void SquareKleinBottle::remap(PointState const &base, BodyState &state) const
{
	state.mirror ^= remap_base(base, state);
}

void SquareKleinBottle::relativize_base(BodyState const &base, PointState &state) const
{
	Eigen::Matrix2d rot = relativizationMatrix(base);
	state.pos = rot * (state.pos - base.pos);
	state.vel = rot * (state.vel - base.vel);
}

void SquareKleinBottle::relativize(BodyState const &base, PointState &state) const
{
	remap(base, state);
	relativize_base(base, state);
}

void SquareKleinBottle::relativize(BodyState const &base, BodyState &state) const
{
	remap(base, state);
	relativize_base(base, state);
	state.mirror ^= base.mirror;
	if(state.mirror)
	{
		state.rpos = -state.rpos;
		state.rvel = -state.rvel;
	}
	state.rpos -= base.rpos;
	state.rvel -= base.rvel;
}

Eigen::Matrix2d SquareKleinBottle::relativizationMatrix(Float rpos, bool mirror) const
{
	Float c = std::cos(rpos);
	Float s = std::sin(rpos);
	Eigen::Matrix2d rot;
	if(mirror)
		rot << -c, s, s, c;
	else
		rot << c, s, -s, c;
	return rot;
}

Eigen::Matrix2d SquareKleinBottle::absolutizationMatrix(Float rpos, bool mirror) const
{
	Float c = std::cos(rpos);
	Float s = std::sin(rpos);
	Eigen::Matrix2d rot;
	if(mirror)
		rot << -c, s, s, c; // gives correct result
	else
		rot << c, -s, s, c;
	return rot;
}

Eigen::Matrix2d SquareKleinBottle::relativizationMatrix(const BodyState& base) const
{
	return relativizationMatrix(base.rpos, base.mirror);
}

Eigen::Matrix2d SquareKleinBottle::absolutizationMatrix(const BodyState& base) const
{
	return absolutizationMatrix(base.rpos, base.mirror);
}

void SquareKleinBottle::absolutize(BodyState const &base, PointState &state) const
{
	Eigen::Matrix2d rot = absolutizationMatrix(base);
	state.pos = base.pos + rot * state.pos;
	state.vel = base.vel + rot * state.vel;
}

Float SquareKleinBottle::distance(PointState const &a, PointState const &b) const
{
#ifdef HARD_OPTIMIZE
	Vector2 d = b.pos - a.pos;
	if(d[1] > radius)
	{
		d[0] = b.pos[0] + a.pos[0];
		d[1] -= 2 * radius;
	}
	else if(d[1] < -radius)
	{
		d[0] = b.pos[0] + a.pos[0];
		d[1] += 2 * radius;
	}
	if(d[0] > radius)
		d[0] -= 2 * radius;
	else if(d[0] < -radius)
		d[0] += 2 * radius;
	return d.norm();
#else
	PointState bb(b);
	remap(a, bb);
	return (bb.pos - a.pos).norm();
#endif
}
