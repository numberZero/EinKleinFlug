#pragma once

class CEntity
{
public:
	virtual ~CEntity() = default;
	virtual void step() {}
};
