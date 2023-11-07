#include "Physics.h"
#include <algorithm>

using namespace cv;
using namespace std;
/// //////////////////////////////////////////

///check if 2 mask collision meet
bool checkPixelLevelCollision(IPhysicsComponent const* first, IPhysicsComponentPtr const& other)
{
	cv::Mat const& mask_First = first->getCollisionMask();
	cv::Mat const& mask_Other = other->getCollisionMask();
	if (mask_Other.empty() || mask_First.empty())
		return false;

	Point TL_first = first->getTL();
	Point TL_other = other->getTL();
	
	// ROI = Reagion of Interest
	Rect firstROI(TL_first, TL_first + Point(mask_First.size()));
	Rect othersROI(TL_other, TL_other + Point(mask_Other.size()));
	Rect intersection = firstROI & othersROI;
	if (intersection.empty())
		return false;

	Rect intersection_RelativeToMe(
		intersection.tl() - TL_first, intersection.br() - TL_first);

	Rect intersection_RelativeToOther(
		intersection.tl() - TL_other, intersection.br() - TL_other);

	Mat myMaskROI = mask_First(intersection_RelativeToMe);
	Mat othersMaskROI = mask_Other(intersection_RelativeToOther);
	Mat whereMeAndOtherIntersect = myMaskROI.mul(othersMaskROI);

	return countNonZero(whereMeAndOtherIntersect);
}
bool checkPixelLevelCollisionButtom(IPhysicsComponent const* first, IPhysicsComponentPtr const& other)
{
	cv::Mat const& mask_First = first->getCollisionMask();
	cv::Mat const& mask_Other = other->getCollisionMask();
	if (mask_Other.empty() || mask_First.empty())
		return false;

	Point BR_first = first->getTL() + Point(mask_First.size());
	Point BL_first = first->getTL() + Point(0,mask_First.size().height);

	Point TL_other = other->getTL();
	Point TR_other = TL_other + Point(mask_Other.size().width,0);



	return BL_first.x <= TR_other.x - 100 && BR_first.x >= TL_other.x + 100 && abs(BR_first.y - TR_other.y) <= 15;
}

bool checkPixelLevelCollisionInner(IPhysicsComponent const* first, IPhysicsComponentPtr const& other)
{
	cv::Mat const& mask_First = first->getCollisionMask();
	cv::Mat const& mask_Other = other->getCollisionMask();
	if (mask_Other.empty() || mask_First.empty())
		return false;

	Point BR_first = first->getTL() + Point(mask_First.size());
	Point BL_first = first->getTL() + Point(0, mask_First.size().height);

	Point TL_other = other->getTL();
	Point TR_other = TL_other + Point(mask_Other.size().width, 0);



	return BL_first.x <= TR_other.x - 100 && BR_first.x >= TL_other.x + 100 && abs(BR_first.y - TR_other.y) <= 15;
}

/// //////////////////////////////////////////

FixedWidgetPhysics::FixedWidgetPhysics()
	:_topLeft(0,0), _mask()
{
}

void FixedWidgetPhysics::reset(cv::Point const& tl)
{
	_topLeft = tl;
}

bool FixedWidgetPhysics::update(cv::Mat const& collisionMask)
{
	_mask = collisionMask;
	return false;
}

bool FixedWidgetPhysics::checkCollision(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollision(this, other);
}

bool FixedWidgetPhysics::checkCollisionButtom(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollisionButtom(this, other);
}

cv::Point const& FixedWidgetPhysics::getTL() const
{
	return _topLeft;
}

cv::Point const& FixedWidgetPhysics::getVelocity() const
{
	return cv::Point(0, 0);
}

cv::Mat const& FixedWidgetPhysics::getCollisionMask() const
{
	return _mask;
}

///////////////////////////////////////////////////////////////////

cv::Point const& ConstVelocityPhysics::getVelocity() const
{
	return _velocity;
}

ConstVelocityPhysics::ConstVelocityPhysics(cv::Point const& velocity)
	:_currTL(0,0) , _mask() , _velocity(velocity)
{
}

void ConstVelocityPhysics::reset(cv::Point const& tl)
{
	_currTL = tl;
}

bool ConstVelocityPhysics::update(cv::Mat const& collisionMask)
{
	_mask = collisionMask;
	_currTL += _velocity;
	return false;
}

cv::Mat const& ConstVelocityPhysics::getCollisionMask() const
{
	return _mask;
}

bool ConstVelocityPhysics::checkCollision(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollision(this, other);
}

bool ConstVelocityPhysics::checkCollisionButtom(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollisionButtom(this, other);
}

cv::Point const& ConstVelocityPhysics::getTL() const
{
	return _currTL;
}

//////////////////////////////////////////////////////////////////////////////////////////

JumpPhysics::JumpPhysics(int horizontalVelocity, int initialVerticalVelocity, int gravity)
	:_gravity(gravity), _initialJumpVelocity(horizontalVelocity,-initialVerticalVelocity) , _currTL(0,0),
		_jumpStartTL(_currTL),_currVelocity(_initialJumpVelocity) 
{
	if (gravity < 0 || initialVerticalVelocity < 0)
		throw exception("A positive number was expected");
		
}

void JumpPhysics::reset(cv::Point const& tl)
{
	_jumpStartTL = tl;
	_currTL = tl;
	_currVelocity = _initialJumpVelocity;
}

bool JumpPhysics::update(cv::Mat const& collisionMask)
{
	_mask = collisionMask;
	_currTL += _currVelocity; 
	_currVelocity.y += _gravity;
	if (_currTL.y > _jumpStartTL.y) {//if _currTL.y big than _jumpStartTL.y it will be equals
		_currTL.y = _jumpStartTL.y;
	}
	return _currTL.y == _jumpStartTL.y;//return true if the jump is over and false otherwise
}

cv::Mat const& JumpPhysics::getCollisionMask() const
{
	return _mask;
}

bool JumpPhysics::checkCollision(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollision(this, other);
}
bool JumpPhysics::checkCollisionButtom(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollisionButtom(this, other);
}
cv::Point const& JumpPhysics::getTL() const
{
	return _currTL;
}

cv::Point const& JumpPhysics::getVelocity() const
{
	return _currVelocity;
}
////////////////////////////////////////////////////////////////////////////////////////

ThrowPhysics::ThrowPhysics(int horizontalVelocity, int initialVerticalVelocity, int gravity, int bottom)
	:_gravity(gravity), _initialThrowVelocity(horizontalVelocity, -initialVerticalVelocity),
												_bottom(bottom), _currVelocity(_initialThrowVelocity)
{
	if (gravity < 0 || initialVerticalVelocity < 0)
		throw exception("A positive number was expected");
}

void ThrowPhysics::reset(cv::Point const& tl)
{
	_currTL = tl;
	_currVelocity = _initialThrowVelocity;
}

bool ThrowPhysics::update(cv::Mat const& collisionMask)
{
	_mask = collisionMask;
	_currTL += _currVelocity;
	_currVelocity.y += _gravity;
	if (_currTL.y > _bottom)
		_currTL.y = _bottom;
	return _currTL.y == _bottom;//return true if the throw is over and false otherwise
}

cv::Mat const& ThrowPhysics::getCollisionMask() const
{
	return _mask;
}

bool ThrowPhysics::checkCollision(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollision(this, other);
}

bool ThrowPhysics::checkCollisionButtom(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollisionButtom(this, other);
}

cv::Point const& ThrowPhysics::getTL() const
{
	return _currTL;
}

cv::Point const& ThrowPhysics::getVelocity() const
{
	return _currVelocity;
}


////////////////////////////////////////////////////////////////////////////////////////

NonCollidingPhysicsDecorator::NonCollidingPhysicsDecorator(IPhysicsComponentPtr base)
	:_base(base)
{
}

void NonCollidingPhysicsDecorator::reset(cv::Point const& tl)
{
	_base->reset(tl);
}

bool NonCollidingPhysicsDecorator::update(cv::Mat const& collisionMask)
{
	return _base->update(collisionMask);
}

cv::Mat const& NonCollidingPhysicsDecorator::getCollisionMask() const
{
	return Mat();
}

bool NonCollidingPhysicsDecorator::checkCollision(std::shared_ptr<IPhysicsComponent> const& other) const
{
	return false;
}
bool NonCollidingPhysicsDecorator::checkCollisionButtom(std::shared_ptr<IPhysicsComponent> const& other) const
{
	return false;
}
cv::Point const& NonCollidingPhysicsDecorator::getTL() const
{
	return _base->getTL();
}

cv::Point const& NonCollidingPhysicsDecorator::getVelocity() const
{
	return _base->getVelocity();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

BoundedPhysicsDecorator::BoundedPhysicsDecorator(IPhysicsComponentPtr base, int width, int height)
	:_base(base), _bounds(0,0,width,height)
{
}

void BoundedPhysicsDecorator::reset(cv::Point const& tl)
{
	_base->reset(tl);
	_currTL = tl;
}

bool BoundedPhysicsDecorator::update(cv::Mat const& collisionMask)
{
	bool isFinish = _base->update(collisionMask);
	_currTL = _base-> getTL();
	test();
	return isFinish;
}

cv::Mat const& BoundedPhysicsDecorator::getCollisionMask() const
{
	return _base-> getCollisionMask();
}

bool BoundedPhysicsDecorator::checkCollision(std::shared_ptr<IPhysicsComponent> const& other) const
{
	return checkPixelLevelCollision(this, other);
}


cv::Point const& BoundedPhysicsDecorator::getTL() const
{
	return _currTL;
}

cv::Point const& BoundedPhysicsDecorator::getVelocity() const
{
	return _base->getVelocity();
}

void BoundedPhysicsDecorator::test()
{
	if (_currTL.x < 0) {
		_currTL.x = _bounds.width + (_currTL.x % _bounds.width);
	}
	else {
		_currTL.x %= _bounds.width;
	}
}
bool BoundedPhysicsDecorator::checkCollisionButtom(IPhysicsComponentPtr const& other) const
{
	return checkPixelLevelCollisionButtom(this, other);
}

//cv::Point const& IPhysicsComponent::getVelocity() const
//{
//	return cv::Point(0,0);
//}

