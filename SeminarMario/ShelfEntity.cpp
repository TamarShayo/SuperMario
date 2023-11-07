#include "ShelfEntity.h"
#include <filesystem>


using namespace cv;
using namespace std;
namespace fs = std::filesystem;


EntityStatePtr createShelfState(fs::path const& animationFolder)
{
	AnimationPtr animation(new Animation(animationFolder.string()));
	bool isCyclic = true;

	IGraphicsComponentPtr graphicsPtr(
		new SingleAnimationGraphics(animation, isCyclic));
	IPhysicsComponentPtr physicsPtr = make_shared<FixedWidgetPhysics>();

	return EntityStatePtr(new EntityState(graphicsPtr, physicsPtr));
	//return make_shared<EntityState>(graphicsPtr, make_shared<BoundedPhysicsDecorator>(physicsPtr, 1280, 720));

}


EntityPtr CreateShelf(std::string const& animationFolder)
{
	return EntityPtr(new Entity(createShelfState(animationFolder)));
}

