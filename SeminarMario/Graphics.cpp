#include "Graphics.h"
#include <algorithm>

using namespace cv;
using namespace std;

//////////////////////////////////////////////////

SingleAnimationGraphics::SingleAnimationGraphics(AnimationPtr animPtr, bool isCyclic)
    :_animation(animPtr), _currFrameIdx(0)
    , _isCyclic(isCyclic)
{    
}


cv::Mat SingleAnimationGraphics::getCollisionMask()
{
    return _animation->getFrame(_currFrameIdx).mask;
}

void SingleAnimationGraphics::draw(cv::Mat& canvas, cv::Point const& topLeft)
{
    drawFrame(_animation->getFrame(_currFrameIdx), canvas, topLeft);
}

void SingleAnimationGraphics::reset()
{
    reset(0);
}

void SingleAnimationGraphics::reset(int code)
{
    _currFrameIdx = 0;
}

bool SingleAnimationGraphics::update()
{
    bool hasFinished = (_currFrameIdx == _animation->numFrames() - 1);
    if (_isCyclic)
        _currFrameIdx = (_currFrameIdx + 1) % _animation->numFrames();
    else 
        _currFrameIdx = min<int>(_currFrameIdx + 1, (int)_animation->numFrames() - 1);

    return hasFinished;
}

int SingleAnimationGraphics::getCode() const
{
    return 0;
}




//////////////////////////////////////////

LivesGraphics::LivesGraphics(Frame singleLife)
    : _singleLife(singleLife), _livesCount(4)
{
}

cv::Mat LivesGraphics::getCollisionMask()
{
    return _singleLife.mask;
}

void LivesGraphics::draw(cv::Mat& canvas, cv::Point const& topLeft)
{
    Point temp = topLeft;
    for (int i = 0; i < _livesCount; i++) 
    {
        drawFrame(_singleLife, canvas, temp);
        temp.x -= 100;
    }
    
}

void LivesGraphics::reset()
{
    reset(4);
}

void LivesGraphics::reset(int code)
{
    _livesCount = code;
}

bool LivesGraphics::update()
{
    return false;
}

int LivesGraphics::getCode() const
{
    return _livesCount;
}



//////////////////////////////////////////////////////////////////////////

ScoresGraphics::ScoresGraphics(float fontScale, int score, int fontFace)
    :_fontScale(fontScale) , _score(score), _fontFace(fontFace)
{
}

cv::Mat ScoresGraphics::getCollisionMask()
{
    return cv::Mat();
}

void ScoresGraphics::draw(cv::Mat& canvas, cv::Point const& topLeft)
{
    putText(canvas, "scores: "+ to_string(_score), topLeft, _fontFace, _fontScale, Scalar(0,0,255), 2, FILLED);
}

void ScoresGraphics::reset()
{
    reset(200);
}

void ScoresGraphics::reset(int code)
{
    _score = code;
}

bool ScoresGraphics::update()
{
    return false;
}

int ScoresGraphics::getCode() const
{
    return _score;
}



//////////////////////////////////////////////////

