#pragma once
#include "Entities.h"

class LiveEntity : public Entity {
	public:
		LiveEntity();
		virtual void onNotify(Event const& e) override;
};

typedef std::shared_ptr<LiveEntity> LiveEntityPtr;
