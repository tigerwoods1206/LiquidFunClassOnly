//
//  TGestureRecognizer.h
//
//  Created by Tien Dao Cong on 12/20/14.
//  Copyright (c) 2014 TienDC. All rights reserved.
//


#ifndef __TGESTURE_RECOGNIZER_H__
#define __TGESTURE_RECOGNIZER_H__

#include "cocos2d.h"


USING_NS_CC;


//
//
// TGestureHandler
//
//

class TGesture;
class TGestureTap;
class TGestureLongPress;
class TGesturePan;
class TGesturePinch;
class TGestureRotation;
class TGestureSwipe;

class TGestureHandler
{
public:
    /* Return true to swallow the event */
    virtual bool onGestureTap(TGestureTap* gesture) { return false; };
    virtual bool onGestureLongPress(TGestureLongPress* gesture) { return false; };
    virtual bool onGesturePan(TGesturePan* gesture) { return false; };
    virtual bool onGesturePinch(TGesturePinch* gesture) { return false; };
    virtual bool onGestureRotation(TGestureRotation* gesture) { return false; };
    virtual bool onGestureSwipe(TGestureSwipe* gesture) { return false; };
};


//
//
// Base Gesture class.
//
//

class TGesture : public Ref
{
    CC_SYNTHESIZE(int, _id, ID);
    CC_SYNTHESIZE_PASS_BY_REF(Vec2, _location, Location);
    
public:
    inline Vec2 getLocationInView()
    {
        return Director::getInstance()->convertToUI(_location);
    }
};


//
//
// Tap Gesture class.
//
//

class TGestureTap : public TGesture
{
    CC_SYNTHESIZE(int, _numClicks, NumClicks);
};


//
//
// Long press Gesture class.
//
//

class TGestureLongPress : public TGesture
{
    CC_SYNTHESIZE(float, _threshold, Threshold);
};


//
//
// Pan Gesture class.
//
//

class TGesturePan : public TGesture
{
    CC_SYNTHESIZE_PASS_BY_REF(Vec2, _translation, Translation);
};


//
//
// Pinch Gesture class.
//
//

class TGesturePinch : public TGesture
{
    CC_SYNTHESIZE(float, _scale, Scale);
};


//
//
// Roatation Gesture class.
//
//

class TGestureRotation : public TGesture
{
    CC_SYNTHESIZE(float, _rotation, Rotation); // Rotation in Degrees
    
public:
    inline float getRotationInRadians()
    {
        return CC_DEGREES_TO_RADIANS(_rotation);
    }
};


typedef enum {
    SwipeDirectionUnkown,
    SwipeDirectionNorth,
    SwipeDirectionNorthWest,
    SwipeDirectionNorthEast,
    SwipeDirectionSouth,
    SwipeDirectionSouthWest,
    SwipeDirectionSouthEast,
    SwipeDirectionEast,
    SwipeDirectionWest,
}
SwipeDirection;

//
//
// Swipe Gesture class.
//
//

class TGestureSwipe : public TGesture
{
    CC_SYNTHESIZE_PASS_BY_REF(Vec2, _endLocation, EndLocation);
    CC_SYNTHESIZE(float, _duration, Duration);
    
public:
    inline Vec2 getDirectionVec()
    {
        return _endLocation - _location;
    }
    
    inline SwipeDirection getDirection()
    {
        Vec2 origin(0, 1); // With swipe North, this gives an angle of 0 degree
        float angle = getDirectionVec().getAngle(origin);
        angle = CC_RADIANS_TO_DEGREES(angle);
        
        SwipeDirection direction = SwipeDirectionUnkown;
        
        if (-90.0f*1/4 <= angle && angle < 90.0f*1/4)
        {
            direction = SwipeDirectionNorth;
        }
        else if (90.0f*1/4 <= angle && angle < 90.0f*3/4)
        {
            direction = SwipeDirectionNorthEast;
        }
        else if (90.0f*3/4 <= angle && angle < 90.0f*5/4)
        {
            direction = SwipeDirectionEast;
        }
        else if (90.0f*5/4 <= angle && angle < 90.0f*7/4)
        {
            direction = SwipeDirectionSouthEast;
        }
        else if ((90.0f*7/4 <= angle && angle <= 90.0f*8/4) || (-90.0f*8/4 <= angle && angle < -90.0f*7/4))
        {
            direction = SwipeDirectionSouth;
        }
        else if (-90.0f*7/4 <= angle && angle < -90.0f*5/4)
        {
            direction = SwipeDirectionSouthWest;
        }
        else if (-90.0f*5/4 <= angle && angle < -90.0f*3/4)
        {
            direction = SwipeDirectionWest;
        }
        else if (-90.0f*3/4 <= angle && angle < -90.0f*1/4)
        {
            direction = SwipeDirectionNorthWest;
        }
        else
        {
            CCLOG("TGestureSwipe::getDirection() - where is the error?");
        }
        
        return direction;
    }
    
    inline float getVelocity()
    {
        return _endLocation.getDistance(_location) / _duration;
    }
    
    inline float getLength()
    {
        return _endLocation.getDistance(_location);
    }
};


//
//
// TGestureRecognizer
//
//

class TGestureRecognizer : public Ref
{
    CC_SYNTHESIZE(bool, _tapEnabled, TapEnabled);
    CC_SYNTHESIZE(bool, _doubleTapEnabled, DoubleTapEnabled);
    CC_SYNTHESIZE(bool, _longPressEnabled, LongPressEnabled);
    CC_SYNTHESIZE(bool, _panEnabled, PanEnabled);
    CC_SYNTHESIZE(bool, _pinchEnabled, PinchEnabled);
    CC_SYNTHESIZE(bool, _rotationEnabled, RotationEnabled);
    CC_SYNTHESIZE(bool, _swipeEnabled, SwipeEnabled);
    
    CC_SYNTHESIZE(float, _tapThreshold, TapThreshold);              // Max threshold to activate a tap
    CC_SYNTHESIZE(float, _doubleTapInterval, DoubleTapInterval);    // Max interval between two taps to activate a double tap
    CC_SYNTHESIZE(float, _longPressThreshold, LongPressThreshold);  // Min threshold to activate a long press
    CC_SYNTHESIZE(bool, _tapRequiresDoubleTapRecognitionToFail, TapRequiresDoubleTapRecognitionToFail);
    CC_SYNTHESIZE(float, _rotationFingersDistanceChangeTolerance, RotationFingersDistanceChangeTolerance);  // Max rotation fingers' distance change
    CC_SYNTHESIZE(float, _pinchFingersDistanceChangeTolerance, PinchFingersDistanceChangeTolerance);        // Min pinch fingers' distance change
    CC_SYNTHESIZE(float, _swipeThreshold, SwipeThreshold);          // Max threshold to activate a swipe
    
    // IDs of last fired events for each kind of gesture
    CC_SYNTHESIZE_READONLY(int, _lastTapId, LastTapID);   // Return -1 when no event of this kind has been fired
    CC_SYNTHESIZE_READONLY(int, _lastLongPressId, LastLongPressID);
    CC_SYNTHESIZE_READONLY(int, _lastPanId, LastPanID);
    CC_SYNTHESIZE_READONLY(int, _lastPinchId, LastPinchID);
    CC_SYNTHESIZE_READONLY(int, _lastRotationId, LastRotationID);
    CC_SYNTHESIZE_READONLY(int, _lastSwipeId, LastSwipeID);
    
    // Client code should use this to register a gesture handler
    CC_SYNTHESIZE(TGestureHandler*, _gestureHandler, GestureHandler);
    
public:
    TGestureRecognizer();
    virtual ~TGestureRecognizer();
    virtual bool init();
    
public:
    virtual bool onTouchBegan(Touch* touch, Event* event) = 0;
    virtual void onTouchMoved(Touch* touch, Event* event) = 0;
    virtual void onTouchEnded(Touch* touch, Event* event) = 0;
    virtual void onTouchCancelled(Touch* touch, Event* event) = 0;
    
protected:
    virtual void fireTapEvent(TGestureTap* gesture);
    virtual void fireLongPressEvent(TGestureLongPress* gesture);
    virtual void firePanEvent(TGesturePan* gesture);
    virtual void firePinchEvent(TGesturePinch* gesture);
    virtual void fireRotationEvent(TGestureRotation* gesture);
    virtual void fireSwipeEvent(TGestureSwipe* gesture);
    
    inline int getNextTapID() { int val = _lastTapId + 1; return val < 0 ? 0 : val; }
    inline int getNextLongPressID() { int val = _lastLongPressId + 1; return val < 0 ? 0 : val; }
    inline int getNextPanID() { int val = _lastPanId + 1; return val < 0 ? 0 : val; }
    inline int getNextPinchID() { int val = _lastPinchId + 1; return val < 0 ? 0 : val; }
    inline int getNextRotationID() { int val = _lastRotationId + 1; return val < 0 ? 0 : val; }
    inline int getNextSwipeID() { int val = _lastSwipeId + 1; return val < 0 ? 0 : val; }
};


//
//
// TSimpleGestureRecognizer
//
//

class TSimpleGestureRecognizer : public TGestureRecognizer
{
protected:
    Touch* _finger0;
    Vec2 _finger0StartPt, _finger0PrevPt, _finger0Pt;
    bool _finger0Moved;
    double _finger0StartTime, _finger0Time;
    
    Touch* _finger1;
    Vec2 _finger1StartPt, _finger1PrevPt, _finger1Pt;
    bool _finger1Moved;
    
    // Tap/Double tap
    TGestureTap* _lastTap;
    double _lastTapTime;
    bool _tapActivatingScheduled;
    
    // Long press
    bool _longPressActivatingScheduled;
    
    // Pan
    TGesturePan* _lastPan;
    
    // Pinch
    TGesturePinch* _lastPinch;
    
    // Rotation
    TGestureRotation* _lastRotation;
    
public:
    TSimpleGestureRecognizer();
    virtual ~TSimpleGestureRecognizer();
    virtual bool init();
    
public:
    virtual bool onTouchBegan(Touch* touch, Event* event);
    virtual void onTouchMoved(Touch* touch, Event* event);
    virtual void onTouchEnded(Touch* touch, Event* event);
    virtual void onTouchCancelled(Touch* touch, Event* event);
    
protected:
    void onLongPressActivate(float time);
    void onTapActivate(float time);
    
private:
    // Get current time
    double getCurrentTime();
};


#endif /* __TGESTURE_RECOGNIZER_H__ */
