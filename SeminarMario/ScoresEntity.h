#pragma once
#include "Entities.h"

class ScoresEntity : public Entity {

public:
	ScoresEntity(float fontScale, int score, int fontFace);
	virtual void onNotify(Event const& e) override;
};

typedef std::shared_ptr<ScoresEntity> ScoresEntityPtr;