#include "HandGrenadeEntity.h"
#include "opencv2/opencv.hpp"
#include "Config.h"
#include <memory>
#include <filesystem>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

EntityStatePtr createHandGrenadeState(std::string const& animationsPath)
{
	AnimationPtr animation(new Animation(animationsPath));
	bool isCyclic = true;

	IGraphicsComponentPtr graphicsPtr(
		new SingleAnimationGraphics(animation, isCyclic));

	IPhysicsComponentPtr physicsPtr = make_shared<ThrowPhysics>(20, 40 ,10, 720);

	return EntityStatePtr(new EntityState(graphicsPtr, physicsPtr));

}
EntityPtr createHandGrenade(std::string const& animationsPath)
{
    return EntityPtr(new Entity(createHandGrenadeState(animationsPath)));
}
