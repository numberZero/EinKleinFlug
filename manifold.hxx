#pragma once
#include <Eigen/Core>

struct PointState
{
	Eigen::Vector2d pos;
	Eigen::Vector2d vel;
};

struct BodyState: PointState
{
	double rpos;
	double rvel;
	bool mirror;
};

class SquareKleinBottle
{
private:
	bool shift_x(PointState &state, int dir) const;
	bool shift_y(PointState &state, int dir) const;
	bool stepState_base(PointState &state, double dt) const;
	bool remap_base(BodyState const &base, PointState &state) const;

public:
	double const radius;
	void stepState(PointState &state, double dt) const;
	void stepState(BodyState &state, double dt) const;
	void remap(BodyState const &base, PointState &state) const;
	void remap(BodyState const &base, BodyState &state) const;
	void relativize(BodyState const &base, PointState &state) const;
	void relativize(BodyState const &base, BodyState &state) const;
};
