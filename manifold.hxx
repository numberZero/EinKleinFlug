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
	bool remap_base(PointState const &base, PointState &state) const;
	void relativize_base(BodyState const &base, PointState &state) const;

public:
	double const radius;

	Eigen::Matrix2d relativizationMatrix(double rpos, bool mirror) const;
	Eigen::Matrix2d absolutizationMatrix(double rpos, bool mirror) const;

	Eigen::Matrix2d relativizationMatrix(BodyState const &base) const;
	Eigen::Matrix2d absolutizationMatrix(BodyState const &base) const;

	void stepState(PointState &state, double dt) const;
	void stepState(BodyState &state, double dt) const;
	void remap(PointState const &base, PointState &state) const;
	void remap(PointState const &base, BodyState &state) const;
	void relativize(BodyState const &base, PointState &state) const;
	void relativize(BodyState const &base, BodyState &state) const;
	void absolutize(BodyState const &base, PointState &state) const;
	double distance(PointState const &a, PointState const &b) const;
};
