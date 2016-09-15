#pragma once
#include "types.hxx"

struct PointState
{
	Vector2 pos;
	Vector2 vel;
};

struct BodyState: PointState
{
	Float rpos;
	Float rvel;
	bool mirror;
};

class SquareKleinBottle
{
private:
	bool shift_x(PointState &state, int dir) const;
	bool shift_y(PointState &state, int dir) const;
	bool stepState_base(PointState &state, Float dt) const;
	bool remap_base(PointState const &base, PointState &state) const;
	void relativize_base(BodyState const &base, PointState &state) const;

public:
	Float const radius;

	SquareKleinBottle(Float radius);

	Matrix2 relativizationMatrix(Float rpos, bool mirror) const;
	Matrix2 absolutizationMatrix(Float rpos, bool mirror) const;

	Matrix2 relativizationMatrix(BodyState const &base) const;
	Matrix2 absolutizationMatrix(BodyState const &base) const;

	void stepState(PointState &state, Float dt) const;
	void stepState(BodyState &state, Float dt) const;
	void remap(PointState const &base, PointState &state) const;
	void remap(PointState const &base, BodyState &state) const;
	void relativize(BodyState const &base, PointState &state) const;
	void relativize(BodyState const &base, BodyState &state) const;
	void absolutize(BodyState const &base, PointState &state) const;
	Float distance(PointState const &a, PointState const &b) const;
};
