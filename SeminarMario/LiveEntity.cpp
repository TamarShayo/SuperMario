#include "LiveEntity.h"
#include <filesystem>
#include "Config.h"


using namespace cv;
using namespace std;
namespace fs = std::filesystem;

LiveEntity:: LiveEntity() : Entity(nullptr)
{
	
	IGraphicsComponentPtr graphicsPtr(
		new LivesGraphics(Frame(R"(../Animations/heart.png)")));

	IPhysicsComponentPtr physicsPtr =make_shared<NonCollidingPhysicsDecorator>
						(NonCollidingPhysicsDecorator(make_shared<FixedWidgetPhysics>()));
	_state = make_shared<EntityState>(graphicsPtr, physicsPtr);
}

void LiveEntity::onNotify(Event const& e)
{
	if (e.sender == EventSenders::SENDER_ENTITY_STATE
		&&
		e.type == EventTypes::EVENT_PHYSICS
		&&
		e.code == EventCodes::COLLISION_WITH_ENEMY)
	{
		_state->getGraphics()->reset(_state->getGraphics().get()->getCode()-1);
	}
	else 
		if (e.sender == EventSenders::SENDER_ENTITY_STATE
			&&
			e.type == EventTypes::EVENT_PHYSICS
			&&
			e.code == EventCodes::COLLISION_WITH_FOOD_SCORE)
		{
			_state->getGraphics()->reset(_state->getGraphics().get()->getCode() +1);
		}

	else if (e.sender == EventSenders::SENDER_ENTITY_STATE
		&&
		e.type == EventTypes::EVENT_PHYSICS
		&&
		e.code == EventCodes::KILL_ENEMY) {
		_state->getGraphics()->reset(_state->getGraphics().get()->getCode() + 1);
	}

}

