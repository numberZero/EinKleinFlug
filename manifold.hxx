#pragma once
#include <Eigen/Core>

struct PointState
{
	Eigen::Vector2d pos;
	Eigen::Vector2d vel;
	bool mirror;
};

struct BodyState: PointState
{
	double rpos;
	double rvel;
};

struct SquareKleinBottle
{
	void shift_x(PointState &state, int dir);
	void shift_y(PointState &state, int dir);

	double radius;
	void stepState(PointState &state, double dt);
	void stepState(BodyState &state, double dt);
	void remap(BodyState const &base, BodyState &state);
	void relativize(BodyState const &base, BodyState &state);
};
