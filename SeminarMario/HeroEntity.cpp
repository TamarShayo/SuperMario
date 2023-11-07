#include "HeroEntity.h"
#include "opencv2/opencv.hpp"
#include "Config.h"
#include <memory>
#include <filesystem>

using namespace cv;
using namespace std;

namespace fs = std::filesystem;

enum HeroStates {
	HERO_IDLE,
	HERO_RUN_RIGHT,
	HERO_RUN_LEFT,
	HERO_DUCK, 
	HERO_STAY_DUCK, 
	HERO_STAND_AFTER_DUCK, 
	HERO_JUMP,
	HERO_FALL,
	HERO_COMING_DOWN_ON_SHELF,
	HERO_STAND_ON_SHELF,
};

EntityStatePtr createHeroState(
	fs::path const& animationFolder,
	HeroStates state,
	int width,
	int height)
{
	AnimationPtr animation(new Animation(animationFolder.string()));
	bool isNotCyclic =
		state == HeroStates::HERO_DUCK ||
		state == HeroStates::HERO_STAND_AFTER_DUCK ||
		state == HeroStates::HERO_JUMP;
	bool isCyclic = !isNotCyclic;

	IGraphicsComponentPtr graphicsPtr(
		new SingleAnimationGraphics(animation, isCyclic));

	IPhysicsComponentPtr physicsPtr = nullptr;
	Size frameSize = graphicsPtr->getCollisionMask().size();
	switch (state)
	{
	case HERO_IDLE:
		physicsPtr.reset(new FixedWidgetPhysics());
		break;
	case HERO_RUN_RIGHT:
		physicsPtr.reset(new ConstVelocityPhysics(Point(frameSize.width / 10, 0)));
		break;
	case HERO_RUN_LEFT:
		physicsPtr.reset(new ConstVelocityPhysics(Point(-frameSize.width / 10, 0)));
		break;
	case HERO_DUCK:
		physicsPtr.reset(new FixedWidgetPhysics());
		break;
	case HERO_STAY_DUCK:
		physicsPtr.reset(new FixedWidgetPhysics());
		break;
	case HERO_STAND_AFTER_DUCK:
		physicsPtr.reset(new FixedWidgetPhysics());
		break;
	case HERO_JUMP:
		physicsPtr.reset(new ThrowPhysics(35, 60, 15, height * 2 / 3));
		break;
	case HERO_COMING_DOWN_ON_SHELF:
		physicsPtr.reset(new ThrowPhysics(35, 60, 15, height * 2/3 ));
		break;
	case HERO_STAND_ON_SHELF:
		physicsPtr.reset(new FixedWidgetPhysics());
		break;
	case HERO_FALL:
		physicsPtr.reset(new ThrowPhysics(0, 0, 15, height * 2 / 3));
		break;
	default:
		throw std::exception("Unknown physics state!");
	}

	return make_shared<EntityState>(graphicsPtr, make_shared<BoundedPhysicsDecorator>(physicsPtr , width , height));
}

EntityPtr createHero(std::string const & rootAnimationsFolder , int width , int height)
{
	fs::path root = rootAnimationsFolder;
	auto idleFolder = root / "idle";

	auto idle = createHeroState(root / "idle", HeroStates::HERO_IDLE , width , height);
	auto runRight = createHeroState(root / "runRight", HeroStates::HERO_RUN_RIGHT, width, height);
	auto runLeft = createHeroState(root / "runLeft", HeroStates::HERO_RUN_LEFT, width, height);
	auto jump = createHeroState(root / "jump", HeroStates::HERO_JUMP, width, height);
	auto duckDown = createHeroState(root / "duckDown", HeroStates::HERO_DUCK, width, height);
	auto duckStay = createHeroState(root / "duckStay", HeroStates::HERO_STAY_DUCK, width, height);
	auto standAfterDuck = createHeroState(root / "standAfterDuck", HeroStates::HERO_STAND_AFTER_DUCK, width, height);

	auto standOnShelf = createHeroState(root / "idle", HeroStates::HERO_STAND_ON_SHELF, width, height);
	auto comingDownOnShelf = createHeroState(root / "jump", HeroStates::HERO_COMING_DOWN_ON_SHELF, width, height);
	auto fallFromShelf = createHeroState(root / "jump", HeroStates::HERO_FALL, width, height);

	idle->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_RIGHT }, runRight);
	idle->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_LEFT }, runLeft);
	idle->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_DOWN }, duckDown);
	idle->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_UP }, jump);

	runRight->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_LEFT }, idle);
	runRight->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_UP }, jump);
	runRight->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_DOWN }, duckDown);
	runRight->addState(Event{ EventSenders::SENDER_ENTITY_STATE,EventTypes::EVENT_PHYSICS, EventCodes::COLLISION_WITH_ENEMY}, idle);
	runRight->addState(Event{ EventSenders::SENDER_ENTITY_STATE,EventTypes::EVENT_PHYSICS, EventCodes::FALLING_FROM_SHELF}, fallFromShelf);


	runLeft->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_RIGHT }, idle);
	runLeft->addState(Event{ EventSenders::SENDER_ENTITY_STATE,EventTypes::EVENT_PHYSICS, EventCodes::COLLISION_WITH_ENEMY }, idle);
	runLeft->addState(Event{ EventSenders::SENDER_ENTITY_STATE,EventTypes::EVENT_PHYSICS, EventCodes::FALLING_FROM_SHELF }, fallFromShelf);

	jump->addState(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::ENTITY_PHYSICS_FINISHED }, idle);
	jump->addState(Event{ EventSenders::SENDER_ENTITY_STATE,EventTypes::EVENT_PHYSICS, EventCodes::COLLISION_WITH_ENEMY }, idle);
	jump->addState(Event{ EventSenders::SENDER_ENTITY_STATE,EventTypes::EVENT_PHYSICS, EventCodes::JUMP_TO_SHELF }, standOnShelf);

	duckDown->addState(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_GRAPHICS, EventCodes::ENTITY_FINISHED_ANIMATION }, duckStay);
 
	duckStay->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_UP }, standAfterDuck);

	standAfterDuck->addState(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_GRAPHICS, EventCodes::ENTITY_FINISHED_ANIMATION }, idle);
	
	standOnShelf->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_DOWN }, duckDown);
	standOnShelf->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_UP }, comingDownOnShelf);
	standOnShelf->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_RIGHT }, runRight);
	standOnShelf->addState(Event{ EventSenders::SENDER_KEYBOARD, EventTypes::EVENT_KEY_PRESSED, EventCodes::KEY_LEFT }, runLeft);

	comingDownOnShelf->addState(Event{ EventSenders::SENDER_ENTITY_STATE,EventTypes::EVENT_PHYSICS, EventCodes::KEY_UP }, jump);
	comingDownOnShelf->addState(Event{ EventSenders::SENDER_ENTITY_STATE,EventTypes::EVENT_PHYSICS, EventCodes::ENTITY_PHYSICS_FINISHED }, idle);
	comingDownOnShelf->addState(Event{ EventSenders::SENDER_ENTITY_STATE,EventTypes::EVENT_PHYSICS, EventCodes::JUMP_TO_SHELF }, standOnShelf);

	fallFromShelf->addState(Event{ EventSenders::SENDER_ENTITY_STATE, EventTypes::EVENT_PHYSICS, EventCodes::ENTITY_PHYSICS_FINISHED }, idle);

	// ... 
	EntityPtr hero(new Entity(idle));

	idle->Register(hero);
	runRight->Register(hero);
	runLeft->Register(hero);
	jump->Register(hero);
	duckDown->Register(hero);
	duckStay->Register(hero);
	standAfterDuck->Register(hero);
	standOnShelf->Register(hero);
	comingDownOnShelf->Register(hero);
	fallFromShelf->Register(hero);

	return hero;
}