//
//  BallSprite.h
//  TumTum_Base
//
//  Created by ohtaisao on 2014/12/11.
//
//

#ifndef __TumTum_Base__BallSprite__
#define __TumTum_Base__BallSprite__

#define PTM_RATIO 32.0

#include "cocos2d.h"
//#import "cocos-ext.h"
#include "Box2D.h"
#include "extensions/physics-nodes/CCPhysicsSprite.h"

using namespace cocos2d;

class BallSprite : public cocos2d::extension::PhysicsSprite
{
public:
    virtual bool init();
    
    CREATE_FUNC(BallSprite);
    static BallSprite *createBallSprite(Node *parent ,b2World *world, const std::string& filename);
    static BallSprite *createParticleSprite(Node *parent ,b2World *world, b2ParticleSystem *particleSystem, b2ParticleGroup *particleGroup,const std::string& filename);
    static void setPhysics2Sprite(b2World *world, cocos2d::extension::PhysicsSprite *sptite, b2BodyType type, b2Shape shape);
    static float getBallRadius();
    void setParticlePpos(b2ParticleSystem *particleSystem, b2ParticleGroup *particleGroup, float posx, float posy, float vx, float vy);
    void deleteParticle(b2ParticleSystem *particleSystem, b2ParticleGroup *particleGroup);
    
    // 衝突状態の設定
    enum ballContactState {
        kNone         = 0,
        kFloorContact,
        kSecondContact
    };
    ballContactState getBallContState();
    void setBallContState(ballContactState);
    
    // タッチ判定
   // void setCenter(Vec2 pos);
    bool isInCircle(Vec2 pos, Node *parent);
    bool isInCircle(Touch touch);
    bool getIsTouchBegan();
    void setIsTouchBegan();
    bool getIsTouchMoved();
    void setIsTouchMoved();
    bool getIsTouchEnd();
    void setIsTouchEnd();
private:
    bool _isTouchBegan;
    bool _isTouchMoved;
    bool _isTouchEnd;
    
public:
    enum deleteState {
        kNoDelete = 0,
        kPreDelete,
        kDelete
    };
    deleteState getDeleteState();
    void setDeleteState(deleteState state);
    
    //ボール種類判定
    enum ballType {
        kRed = 0,
        kBrue,
        kPurple,
        kYellow
    };
    ballType getBallType();
    void setBallType(ballType state);
    
    //ボールハイライト判定
    enum ballHilightType {
        kNoTouch = 0,
        kFirstTouch,
        kTouch,
        kAdjacent, //touchしたボールに隣接するボール
        kChain//ハイライトされるボール全部
    };
    ballHilightType getBallHilightType();
    void setBallHilightType(ballHilightType state);
    
protected:
    ballContactState _ballContactState;
    deleteState      _deleteState;
    ballType         _ballType;
    ballHilightType  _ballHilightType;
    Vec2             _centerPos;
    
private:
    void setNormalTexture();
    void setHilightTexture();
};

#endif /* defined(__TumTum_Base__BallSprite__) */
