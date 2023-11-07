#pragma once
#include "Observer.h"
#include <vector>
#include "Entities.h"

class Timer : public Subject
{
private:
	size_t _frequency_ms;

public:
	Timer(size_t frequency_ms);

	void tick();
};

