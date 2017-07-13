#pragma once
class FPSCounter
{
	long frames = 0;
	Float time = 0.0;
	Float rate = 0.0;

public:
	void advance(Float dt);
	Float value() const;
};