#pragma once
#include "Animation.h"
#include <opencv2/opencv.hpp>
#include "Timer.h"
#include "HeroEntity.h"
#include "EntitiesPool.h"
#include "Graphics.h"
#include "LiveEntity.h"
#include "ScoresEntity.h"
#include "HandGrenadeEntity.h"
#include "ShelfEntity.h"
#include "Coin.h"

class Game : public IObserver
{
private:
	cv::Mat background;
	EntityPtr hero;
	EntitiesPool slimePool;
	EntitiesPool handGrenadePool;
	std::shared_ptr<std::vector<EntityPtr>> shelfs;
	std::shared_ptr<std::vector<EntityPtr>> coins;
	std::shared_ptr<std::vector<EntityPtr>> slimesInUse;
	std::shared_ptr<std::vector<EntityPtr>> handGrenadeInUse;
	Timer timer;
	LiveEntityPtr lives;
	ScoresEntityPtr scores;
	IObserverPtr gamePtr;
public:
	Game(int width, int height);
	void run();
	virtual void onNotify(Event const& e) override;
};

