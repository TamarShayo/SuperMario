#pragma once
#include <opencv2/opencv.hpp>
#include "Animation.h"

// Interface for various drawing behaviors.
class IGraphicsComponent
{
public:
	// Get mask of current frame (or just an empty cv::Mat() if you don't have)
	virtual cv::Mat getCollisionMask() = 0;
	// Draw current frame on canvas:
	virtual void draw(cv::Mat & canvas, cv::Point const & topLeft) = 0;
	// Reset the graphics (a generic function that has different meaning for different graphic types)
	virtual void reset() = 0;
	virtual void reset(int code) = 0;
	
	// return value is true if the graphics would like to be replaced.
	// (graphics states that "it has fulfilled its duty and has no reason to be here anymore").
	virtual bool update() = 0;

	virtual int getCode() const = 0;
};

typedef std::shared_ptr<IGraphicsComponent> IGraphicsComponentPtr;

// Example of implementation of Graphics of an animation:
class SingleAnimationGraphics : public IGraphicsComponent
{
protected:
	AnimationPtr _animation;
	int _currFrameIdx;
	bool _isCyclic;

public:
	SingleAnimationGraphics(AnimationPtr animPtr, bool isCyclic);

public:
	virtual cv::Mat getCollisionMask();
	virtual void draw(cv::Mat& canvas, cv::Point const& topLeft);
	virtual void reset() override;
	virtual void reset(int code) override;
	virtual bool update();
	virtual int getCode() const override;
};


class LivesGraphics : public IGraphicsComponent
{
protected:
	Frame _singleLife; // visualization of one life.
	int _livesCount; // how much lives we have. gets updated at reset(..).

public:
	LivesGraphics(Frame singleLife);
public:
	virtual cv::Mat getCollisionMask();
	virtual void draw(cv::Mat& canvas, cv::Point const& topLeft);
	virtual void reset() override;
	virtual void reset(int code) override;
	virtual bool update();
	virtual int getCode() const override;
};


class ScoresGraphics : public IGraphicsComponent
{
private:
	// parameters of putText that should be accepted in the c'tor and kept fixed.
	float _fontScale;
	int _score;
	int _fontFace;

public:
	ScoresGraphics(float fontScale, int score , int fontFace);

public:
	virtual cv::Mat getCollisionMask();
	virtual void draw(cv::Mat& canvas, cv::Point const& topLeft);
	virtual void reset() override;
	virtual void reset(int code) override;
	virtual bool update();
	virtual int getCode() const override;
};