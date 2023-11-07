#include "ScoresEntity.h"
#include <filesystem>
#include "Config.h"


using namespace cv;
using namespace std;
namespace fs = std::filesystem;

ScoresEntity::ScoresEntity(float fontScale, int score, int fontFace) 
	: Entity(nullptr)
{

	IGraphicsComponentPtr graphicsPtr(
		new ScoresGraphics(fontScale, score, fontFace));

	IPhysicsComponentPtr physicsPtr = make_shared<NonCollidingPhysicsDecorator>
		(NonCollidingPhysicsDecorator(make_shared<FixedWidgetPhysics>()));

	_state = make_shared<EntityState>(graphicsPtr, physicsPtr);
}

void ScoresEntity::onNotify(Event const& e)
{
	if (e.sender == EventSenders::SENDER_ENTITY_STATE
		&&
		e.type == EventTypes::EVENT_PHYSICS
		&&
		e.code == EventCodes::COLLISION_WITH_ENEMY)
	{
		_state->getGraphics()->reset(_state->getGraphics().get() ->getCode() - 50);
	}
	else if (e.sender == EventSenders::SENDER_ENTITY_STATE
		&&
		e.type == EventTypes::EVENT_PHYSICS
		&&
		e.code == EventCodes::KILL_ENEMY) {
		_state->getGraphics()->reset(_state->getGraphics().get()->getCode() + 50);
	}
}
