//
//  TGestureRecognizer.cpp
//
//  Created by Tien Dao Cong on 12/20/14.
//  Copyright (c) 2014 TienDC. All rights reserved.
//


#include "TGestureRecognizer.h"


USING_NS_CC;


#define GESTURE_RECOGNIZER_TAP_THRESHOLD                                1.0f
#define GESTURE_RECOGNIZER_DOUBLE_TAP_INTERVAL                          0.3f
#define GESTURE_RECOGNIZER_LONGPRESS_THRESHOLD                          1.0f
#define GESTURE_RECOGNIZER_ROTATION_FINGERS_DISTANCE_CHANGE_TOLERANCE   0.5f
#define GESTURE_RECOGNIZER_PINCH_FINGERS_DISTANCE_CHANGE_TOLERANCE      0.1f
#define GESTURE_RECOGNIZER_SWIPE_THRESHOLD                              0.3f


//
//
// TGestureRecognizer
//
//

TGestureRecognizer::TGestureRecognizer() : Ref()
{
    _gestureHandler = NULL;
}

TGestureRecognizer::~TGestureRecognizer()
{
    _gestureHandler = NULL;
}

bool TGestureRecognizer::init()
{
    _gestureHandler = NULL;
    
    _tapEnabled = true;
    _doubleTapEnabled = true;
    _longPressEnabled = true;
    _panEnabled = true;
    _pinchEnabled = true;
    _rotationEnabled = true;
    _swipeEnabled = true;
    
    _tapThreshold = GESTURE_RECOGNIZER_TAP_THRESHOLD;
    _doubleTapInterval = GESTURE_RECOGNIZER_DOUBLE_TAP_INTERVAL;
    _longPressThreshold = GESTURE_RECOGNIZER_LONGPRESS_THRESHOLD;
    _tapRequiresDoubleTapRecognitionToFail = true;
    _rotationFingersDistanceChangeTolerance = GESTURE_RECOGNIZER_ROTATION_FINGERS_DISTANCE_CHANGE_TOLERANCE;
    _pinchFingersDistanceChangeTolerance = GESTURE_RECOGNIZER_PINCH_FINGERS_DISTANCE_CHANGE_TOLERANCE;
    _swipeThreshold = GESTURE_RECOGNIZER_SWIPE_THRESHOLD;
    
    _lastTapId = -1;
    _lastLongPressId = -1;
    _lastPanId = -1;
    _lastPinchId = -1;
    _lastRotationId = -1;
    _lastSwipeId = -1;
    
    return true;
}

void TGestureRecognizer::fireTapEvent(TGestureTap* gesture)
{
    _lastTapId = gesture->getID();
    if (_gestureHandler) _gestureHandler->onGestureTap(gesture);
}

void TGestureRecognizer::fireLongPressEvent(TGestureLongPress* gesture)
{
    _lastLongPressId = gesture->getID();
    if (_gestureHandler) _gestureHandler->onGestureLongPress(gesture);
}

void TGestureRecognizer::firePanEvent(TGesturePan* gesture)
{
    _lastPanId = gesture->getID();
    if (_gestureHandler) _gestureHandler->onGesturePan(gesture);
}

void TGestureRecognizer::firePinchEvent(TGesturePinch* gesture)
{
    _lastPinchId = gesture->getID();
    if (_gestureHandler) _gestureHandler->onGesturePinch(gesture);
}

void TGestureRecognizer::fireRotationEvent(TGestureRotation* gesture)
{
    _lastRotationId = gesture->getID();
    if (_gestureHandler) _gestureHandler->onGestureRotation(gesture);
}

void TGestureRecognizer::fireSwipeEvent(TGestureSwipe* gesture)
{
    _lastSwipeId = gesture->getID();
    if (_gestureHandler) _gestureHandler->onGestureSwipe(gesture);
}


//
//
// TSimpleGestureRecognizer
//
//

TSimpleGestureRecognizer::TSimpleGestureRecognizer() : TGestureRecognizer()
{
    _finger0 = NULL;
    _finger1 = NULL;
    
    _lastTap = NULL;
    _lastPan = NULL;
    _lastPinch = NULL;
    _lastRotation = NULL;
}

TSimpleGestureRecognizer::~TSimpleGestureRecognizer()
{
    CC_SAFE_RELEASE_NULL(_finger0);
    CC_SAFE_RELEASE_NULL(_finger1);
    
    CC_SAFE_RELEASE_NULL(_lastTap);
    CC_SAFE_RELEASE_NULL(_lastPan);
    CC_SAFE_RELEASE_NULL(_lastPinch);
    CC_SAFE_RELEASE_NULL(_lastRotation);
}

bool TSimpleGestureRecognizer::init()
{
    TGestureRecognizer::init();
    
    CC_SAFE_RELEASE_NULL(_finger0);
    CC_SAFE_RELEASE_NULL(_finger1);
    
    CC_SAFE_RELEASE_NULL(_lastTap);
    CC_SAFE_RELEASE_NULL(_lastPan);
    CC_SAFE_RELEASE_NULL(_lastPinch);
    CC_SAFE_RELEASE_NULL(_lastRotation);
    
    return true;
}

bool TSimpleGestureRecognizer::onTouchBegan(Touch* touch, Event* event)
{
    if (_finger0 == NULL)
    {
        _finger0 = touch;
        _finger0->retain();
        
        _finger0StartPt = _finger0->getLocation();
        _finger0PrevPt = _finger0StartPt;
        _finger0Pt = _finger0StartPt;
        _finger0Moved = false;
        
        _finger0StartTime = getCurrentTime();
        _finger0Time = _finger0StartTime;
        
        // Long press recognition is applied on 1st finger only
        _longPressActivatingScheduled = false;
        if (_longPressEnabled && !_finger1)
        {
            Scheduler* scheduler = Director::getInstance()->getScheduler();
            scheduler->unschedule(schedule_selector(TSimpleGestureRecognizer::onLongPressActivate), this);
            scheduler->schedule(schedule_selector(TSimpleGestureRecognizer::onLongPressActivate), this, 0, 0, _longPressThreshold, false);
            
            _longPressActivatingScheduled = true;
        }
    }
    else if (_finger1 == NULL)
    {
        _finger1 = touch;
        _finger1->retain();
        
        _finger1StartPt = _finger1->getLocation();
        _finger1PrevPt = _finger1StartPt;
        _finger1Pt = _finger1StartPt;
        _finger1Moved = false;
    }
    else
    {
        CCLOG("TouchBegan: More than 2 fingers are used - Ignored");
    }
    
    return false;
}

void TSimpleGestureRecognizer::onTouchMoved(Touch* touch, Event* event)
{
    if (_finger0 && touch->getID() == _finger0->getID())
    {
        _finger0Moved = true;
        
        _finger0PrevPt = _finger0Pt;
        _finger0Pt = touch->getLocation();
        
        // Pan recognition is applied on 1st finger only
        if (_panEnabled && !_finger1)
        {
            if (!_lastPan)
            {
                _lastPan = new TGesturePan();
                _lastPan->setID(getNextPanID());
            }
            _lastPan->setLocation(_finger0Pt);
            _lastPan->setTranslation(_finger0Pt - _finger0PrevPt);
            
            firePanEvent(_lastPan);
        }
    }
    else if (_finger1 && touch->getID() == _finger1->getID())
    {
        _finger1Moved = true;
        
        _finger1PrevPt = _finger1Pt;
        _finger1Pt = touch->getLocation();
    }
    else
    {
        CCLOG("TouchMoved: More than 2 fingers are used - Ignored");
    }
    
    // Pinch/Rotation recognition
    if (_finger0 && _finger1 && _finger0Moved && _finger1Moved)
    {
        float fingersStartDistanceSq = _finger0StartPt.getDistanceSq(_finger1StartPt);
        float fingersDistanceSq = _finger0Pt.getDistanceSq(_finger1Pt);
        float fingersDistanceChange = fabsf(sqrtf(fingersStartDistanceSq/fingersDistanceSq) - 1.0f);
        
        // Pinch recognition
        if (_pinchEnabled && fingersDistanceChange > _pinchFingersDistanceChangeTolerance)
        {
            float scale = sqrtf(fingersDistanceSq/fingersStartDistanceSq);
            Vec2 pinchLocation = _lastPinch ? _lastPinch->getLocation() : _finger0StartPt.getMidpoint(_finger1StartPt);
            
            if (!_lastPinch)
            {
                _lastPinch = new TGesturePinch();
                _lastPinch->setID(getNextPinchID());
            }
            _lastPinch->setLocation(pinchLocation);
            _lastPinch->setScale(scale);
            
            firePinchEvent(_lastPinch);
        }
        
        // Rotation recognition
        if (_rotationEnabled && fingersDistanceChange < _rotationFingersDistanceChangeTolerance)
        {
            Vec2 rotationLocation = _lastRotation ? _lastRotation->getLocation() : _finger0StartPt.getMidpoint(_finger1StartPt);
            
            Vec2 rotationArc0Start = _finger0StartPt - rotationLocation;
            Vec2 rotationArc0End = _finger0Pt - rotationLocation;
            float rotationAngle0 = rotationArc0End.getAngle(rotationArc0Start);
            
            Vec2 rotationArc1Start = _finger1StartPt - rotationLocation;
            Vec2 rotationArc1End = _finger1Pt - rotationLocation;
            float rotationAngle1 = rotationArc1End.getAngle(rotationArc1Start);
            
            float rotationAngle = fabsf(rotationAngle0) > fabsf(rotationAngle1) ? rotationAngle0 : rotationAngle1;
            
            if (!_lastRotation)
            {
                _lastRotation = new TGestureRotation();
                _lastRotation->setID(getNextRotationID());
            }
            _lastRotation->setLocation(rotationLocation);
            _lastRotation->setRotation(CC_RADIANS_TO_DEGREES(rotationAngle));
            
            fireRotationEvent(_lastRotation);
        }
    }
}

void TSimpleGestureRecognizer::onTouchEnded(Touch* touch, Event* event)
{
    if (_finger0 && touch->getID() == _finger0->getID())
    {
        // Tap/Double tap/Swipe recognition is applied on 1st finger only
        if (!_finger1)
        {
            // Deactivate long press schedule
            _longPressActivatingScheduled = false;
            
            _finger0Pt = touch->getLocation();
            _finger0Time = getCurrentTime();
            
            if (!_finger0Moved)
            {
                // Prev tap
                TGestureTap* prevTap = _lastTap;
                double prevTapTime = _lastTapTime;
                _lastTap = NULL;
                
                double threshold = _finger0Time - _finger0StartTime;
                if (threshold < _tapThreshold)
                {
                    // Current tap (also last tap)
                    TGestureTap* lastTap = new TGestureTap();
                    lastTap->setID(getNextTapID());
                    lastTap->setLocation(_finger0StartPt);
                    lastTap->setNumClicks(1);
                    double lastTapTime = _finger0Time;
                    
                    // Event activation flags
                    bool activateTap = false;
                    bool activateDoubleTap = false;
                    
                    if (_doubleTapEnabled && prevTap)
                    {
                        double tapsInterval = _finger0StartTime - prevTapTime;
                        float tapsDistance = prevTap->getLocation().getDistanceSq(_finger0StartPt);
                        
                        // The two taps must not be too far from each other
                        activateDoubleTap = tapsInterval < _doubleTapInterval && tapsDistance < (20 * 20); // Hard coded!
                    }
                    
                    if (_tapEnabled)
                    {
                        if (activateDoubleTap)
                        {
                            activateTap = !_tapRequiresDoubleTapRecognitionToFail;
                        }
                        else if (_doubleTapEnabled && _tapRequiresDoubleTapRecognitionToFail)
                        {
                            // Wait for the double tap recognition to fail before activating a tap
                            Scheduler* scheduler = Director::getInstance()->getScheduler();
                            scheduler->unschedule(schedule_selector(TSimpleGestureRecognizer::onTapActivate), this);
                            scheduler->schedule(schedule_selector(TSimpleGestureRecognizer::onTapActivate), this, 0, 0, _doubleTapInterval, false);
                            
                            _tapActivatingScheduled = true;
                        }
                        else
                        {
                            activateTap = true;
                        }
                    }
                    
                    if (activateTap)
                    {
                        // Deactivate tap schedule
                        _tapActivatingScheduled = false;
                        fireTapEvent(lastTap);
                    }
                    
                    if (activateDoubleTap)
                    {
                        // Deactivate tap schedule
                        _tapActivatingScheduled = false;
                        
                        TGestureTap* gesture = new TGestureTap();
                        gesture->setID(getNextTapID());
                        gesture->setLocation(lastTap->getLocation());
                        gesture->setNumClicks(2);
                        
                        fireTapEvent(gesture);
                        CC_SAFE_RELEASE(gesture);
                    }
                    
                    // Remember the last tap
                    if (_doubleTapEnabled && !activateDoubleTap)
                    {
                        _lastTap = lastTap;
                        _lastTapTime = lastTapTime;
                    }
                    else
                    {
                        CC_SAFE_RELEASE(lastTap);
                    }
                }
                
                CC_SAFE_RELEASE(prevTap);
            }
            else /* _finger0Moved == true */
            {
                // Swipe recognition
                if (_swipeEnabled)
                {
                    float threshold = (float) (_finger0Time - _finger0StartTime);
                    if (threshold < _swipeThreshold)
                    {
                        TGestureSwipe* gesture = new TGestureSwipe();
                        gesture->setID(getNextSwipeID());
                        gesture->setLocation(_finger0StartPt);
                        gesture->setEndLocation(_finger0Pt);
                        gesture->setDuration(threshold);
                        
                        fireSwipeEvent(gesture);
                        CC_SAFE_RELEASE(gesture);
                    }
                }
            }
        }
        
        CC_SAFE_RELEASE_NULL(_finger0);
        CC_SAFE_RELEASE_NULL(_lastPan);
        CC_SAFE_RELEASE_NULL(_lastPinch);
        CC_SAFE_RELEASE_NULL(_lastRotation);
    }
    else if (_finger1 && touch->getID() == _finger1->getID())
    {
        CC_SAFE_RELEASE_NULL(_finger1);
        CC_SAFE_RELEASE_NULL(_lastPinch);
        CC_SAFE_RELEASE_NULL(_lastRotation);
    }
    else
    {
        CCLOG("TouchEnded: More than 2 fingers are used - Ignored");
    }
}

void TSimpleGestureRecognizer::onTouchCancelled(Touch* touch, Event* event)
{
    if (_finger0 && touch->getID() == _finger0->getID())
    {
        CC_SAFE_RELEASE_NULL(_finger0);
    }
    else if (_finger1 && touch->getID() == _finger1->getID())
    {
        CC_SAFE_RELEASE_NULL(_finger1);
    }
    else
    {
        CCLOG("TouchCancelled: More than 2 fingers are used - Ignored");
    }
}

void TSimpleGestureRecognizer::onLongPressActivate(float time)
{
    // Long press recognition is applied on 1st finger only
    if (_longPressActivatingScheduled && _finger0 && !_finger0Moved)
    {
        TGestureLongPress* gesture = new TGestureLongPress();
        gesture->setID(getNextLongPressID());
        gesture->setLocation(_finger0StartPt);
        gesture->setThreshold(_longPressThreshold);
        
        fireLongPressEvent(gesture);
        CC_SAFE_RELEASE(gesture);
    }
}

void TSimpleGestureRecognizer::onTapActivate(float time)
{
    TGestureTap* lastTap = _lastTap;
    
    // Tap recognition is applied on 1st finger only
    if (_tapActivatingScheduled && lastTap)
    {
        fireTapEvent(lastTap);
    }
}

// Get current time
double TSimpleGestureRecognizer::getCurrentTime()
{
    timeval val;
    while (gettimeofday(&val, NULL) != 0);
    
    return (double) val.tv_sec + val.tv_usec / 1000000.0;
}
