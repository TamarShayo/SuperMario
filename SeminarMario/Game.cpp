#include "Game.h"
#include "Config.h"
#include <algorithm>
#include <iostream>
//#include <windows.h>
//ctor
Game::Game(int width, int height)
	:timer(100), slimesInUse(new std::vector<EntityPtr>()),
	handGrenadeInUse(new std::vector<EntityPtr>()),
	slimePool(CreateSlimeEnemy, R"(../Animations/SlimeOrange)"),
	handGrenadePool(createHandGrenade, R"(../Animations/HandGrenade)"),
	shelfs(new std::vector<EntityPtr>()),
	coins(new std::vector<EntityPtr>()),
	gamePtr(this)
{
	background = cv::imread(R"(../Animations/background.png)", cv::IMREAD_UNCHANGED);
	resize(background, background, cv::Size(width, height));

	EntityPtr shelf = CreateShelf(R"(../Animations/wall)");
	shelf->reset(cv::Point(width / 8 + 20, height * 3 / 4));
	shelfs->push_back(shelf);
	shelf = CreateShelf(R"(../Animations/wall)");
	shelf->reset(cv::Point(width * 2 / 8 + 100, height * 5 / 8));
	shelfs->push_back(shelf);
	shelf = CreateShelf(R"(../Animations/wall)");
	shelf->reset(cv::Point(width * 3 / 8 + 100, height * 7 / 16));
	shelfs->push_back(shelf);
	shelf = CreateShelf(R"(../Animations/wall)");
	shelf->reset(cv::Point(width * 4 / 5, height * 3 / 4));
	shelfs->push_back(shelf);

	EntityPtr coin = CreateShelf(R"(../Animations/coin)");
	coin->reset(cv::Point(width / 8 + 20 +50, height * 3 / 4 - 70));
	coins->push_back(coin);
	coin = CreateCoin(R"(../Animations/coin)");
	coin->reset(cv::Point(width * 2 / 8 + 100 + 50, height * 5 / 8 - 70));
	coins->push_back(coin);
	coin = CreateCoin(R"(../Animations/coin)");
	coin->reset(cv::Point(width * 3 / 8 + 100 + 50, height * 7 / 16 - 70));
	coins->push_back(coin);
	coin = CreateCoin(R"(../Animations/coin)");
	coin->reset(cv::Point(width * 4 / 5 + 50, height * 3 / 4 - 70));
	coins->push_back(coin);

	hero = createHero(R"(../Animations/Hero)", width, height);
	hero->reset(cv::Point(background.size().width / 2, height * 2 / 3));

	lives = std::make_shared<LiveEntity>();
	lives->reset(cv::Point(background.size().width - 100, 20));

	scores = std::make_shared<ScoresEntity>(1.5, 200, cv::HersheyFonts::FONT_HERSHEY_SIMPLEX);
	scores->reset(cv::Point(40, 70));
}
void Game::run()
{
	timer.Register(gamePtr);
	slimesInUse->push_back(slimePool.getNext());
	(*slimesInUse)[0]->reset(cv::Point(background.size().width * 2 / 3, background.size().height * 4 / 5));
	
	//register the entutues to timer.
	for (auto it = slimesInUse->begin(); it != slimesInUse->end(); it++)
		timer.Register(*it);

	for (auto it = shelfs->begin(); it != shelfs->end(); it++)
		timer.Register(*it);

	for (auto it = coins->begin(); it != coins->end(); it++)
		timer.Register(*it);
	
	//register
	timer.Register(hero);
	hero->Register(lives);
	hero->Register(scores);
	hero->Register(hero);
	hero->Register(gamePtr);

	bool isToExit = false;
	while (false == isToExit)
	{
		cv::Mat canvas = background.clone();
		timer.tick();
		for (EntityPtr slime : (*slimesInUse)) {
			if (hero->checkCollision(slime))
			{
				hero->Notify(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::COLLISION_WITH_ENEMY });
			}
			slime->draw(canvas);
		}

		for (int i = 0; i < coins->size(); i++)
		{
			auto currCoin = coins->at(i);
			if (hero->checkCollision(currCoin))
			{
				hero->Notify(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::COLLISION_WITH_FOOD_SCORE });
			}
			currCoin->draw(canvas);
		}

	
		bool oneOnShelf = false;
		for (EntityPtr shelf : (*shelfs)) {
			if (hero->getState()->getPhysics()->checkCollisionButtom(shelf->getState()->getPhysics()))
			{
				hero->Notify(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::JUMP_TO_SHELF});
				oneOnShelf = true;
			}
			shelf->draw(canvas);
		}

		if ((!oneOnShelf) && (hero->getState()->getPhysics()->getTL().y < background.size().height * 2 / 3))
			hero->Notify(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::FALLING_FROM_SHELF });





		for (EntityPtr handGrenade : (*handGrenadeInUse)) {
			handGrenade->draw(canvas);
			handGrenade->Register(gamePtr);
			for (EntityPtr slime : (*slimesInUse)) {
				if (handGrenade->checkCollision(slime)) {
					handGrenade->Notify(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::KILL_ENEMY });
				
				}
			}
		}


		hero->draw(canvas);
		lives->draw(canvas);
		scores->draw(canvas);


		cv::imshow("Game", canvas);
	}
}

void Game::onNotify(Event const& e) 
{
	if (e.sender == EventSenders::SENDER_KEYBOARD
		&&
		e.type == EventTypes::EVENT_KEY_PRESSED
		&&
		e.code == EventCodes::KEY_SPACE)
	{
		EntityPtr currentHandGrenade = handGrenadePool.getNext();
		handGrenadeInUse->push_back(currentHandGrenade);
		(currentHandGrenade)->reset(hero->getState()->getPhysics()->getTL() + cv::Point(30, 40));
		timer.Register(currentHandGrenade);
		for (EntityPtr slime : (*slimesInUse)) {
			(currentHandGrenade)->Register(slime);
		}
		(currentHandGrenade)->Register(lives);
		(currentHandGrenade)->Register(scores);
		(currentHandGrenade)->getState()->Register(gamePtr);

	}
	if (e.sender == EventSenders::SENDER_ENTITY_STATE
		&&
		e.type == EventTypes::EVENT_PHYSICS
		&&
		e.code == EventCodes::ENTITY_PHYSICS_FINISHED)
	{
		auto handGrenadeFinishedIt = std::find_if(handGrenadeInUse->begin(), handGrenadeInUse->end(),
			[=](EntityPtr handGrenade) {return handGrenade->getState()->getPhysics()->getTL().y >= background.size().height; });
	//	handGrenadePool.returnEntity(EntityPtr(*handGrenadeFinishedIt));
		
		timer.unRegister(*(handGrenadeFinishedIt));
		(*handGrenadeFinishedIt)->getState()->unRegister(gamePtr);
		handGrenadeInUse->erase(handGrenadeFinishedIt) ;
	}
	if (e.sender == EventSenders::SENDER_ENTITY_STATE
		&&
		e.type == EventTypes::EVENT_PHYSICS
		&&
		e.code == EventCodes::KILL_ENEMY)
	{
		for (auto slimeIt = slimesInUse->begin(); slimeIt != slimesInUse->end(); slimeIt++) {
			for (auto handGrenadeIt = handGrenadeInUse->begin(); handGrenadeIt != handGrenadeInUse->end(); handGrenadeIt++) {
				if ((*slimeIt)->checkCollision(*handGrenadeIt))
				{
					//handGrenadePool.returnEntity(*handGrenadeIt);
					slimePool.returnEntity(*slimeIt);
					//timer.UnRegister(*handGrenadeIt);
					timer.unRegister(*slimeIt);
					//(*handGrenadeIt)->getState()->UnRegister(gamePtr);
					slimesInUse->erase(slimeIt);
					//handGrenadeInUse->erase(handGrenadeIt);
					return;
				}
			}
		}
	}
	if (e.sender == EventSenders::SENDER_ENTITY_STATE
		&&
		e.type == EventTypes::EVENT_PHYSICS
		&&
		e.code == EventCodes::COLLISION_WITH_FOOD_SCORE)
	{
		for (auto coin = coins->begin(); coin != coins->end(); coin++) {
				if (hero->checkCollision(*coin))
				{
					//handGrenadePool.returnEntity(*handGrenadeIt);
				//	coins.returnEntity(*coin);
					//timer.UnRegister(*handGrenadeIt);
					timer.unRegister(*coin);
					//(*handGrenadeIt)->getState()->UnRegister(gamePtr);
					coins->erase(coin);
					//handGrenadeInUse->erase(handGrenadeIt);
					return;
				}
			}
		}
	}


