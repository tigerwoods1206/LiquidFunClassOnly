//
//  GameScene_Box2D.h
//  TumTum_Base
//
//  Created by ohtaisao on 2014/12/19.
//
//

#ifndef __TumTum_Base__GameScene_Box2D__
#define __TumTum_Base__GameScene_Box2D__

//#define LIQUIDFUN_SIMD_NEON 1

#include "cocos2d.h"
#include "Box2D.h"
#include "GLESDebugDraw.h"
#include "BallSprite.h"

using namespace cocos2d;

class GameScene_Box2D : public Layer
{
public:
    ~GameScene_Box2D();
    virtual bool init();
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t transformFlags) override;
    static Scene *createGameScene_Box2D();
    CREATE_FUNC(GameScene_Box2D);
    void initPhysics();
    void update(float dt);
    void updateParticles(float dt);
    
    BallSprite *createSprite(Vec2 &pos);
    void addParticle();
    
    //touch
    void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    
private:
    
    //衝突検知
    enum spriteType {
        kNone  = 0,
        kFloor,
        kBall
    };
    bool onContactBegin(PhysicsContact& contact);
    //std::function<void(PhysicsContact& contact, const PhysicsContactPostSolve& solve)> onContactPostSolve;
    void onContactPostSolve(PhysicsContact& contact, const PhysicsContactPostSolve& solve);
    
    //ボール充填
private:
    //ボールが満杯になった事を検知
    bool isFullBoll(BallSprite *ball);
    //ボール満タン
    void fillBoll();
    //消された分のボールを補充
    void refillBoll();
    std::vector<Vec2> _delballPos;
    
    //ボールが壁に接触している事を検知
    void isContactWall(Rect box);
    
    // in isContactWall
    Rect _floor, _leftwall, _rightwall, _ceil;
    
    //隣接ボールハイライト
    void setHilightAllAdjacent();
    void srchAllAdjacent(BallSprite* cur_ball);
    void srchNextAdjacent(BallSprite* cur_ball);
    void setHilightTypeAll(BallSprite::ballHilightType type);
    bool isExistTouch();
    
    //ボール削除
private:
    void delTouchedBalls();
    std::vector<BallSprite*> _bollArray;

    
    
private:
    b2World *_world;
    GLESDebugDraw *_debugDraw;
    b2ParticleSystem* _particleSystem;
    b2ParticleGroup* _particleGroup;
    
protected:
    void onDraw(const cocos2d::Mat4 &transform, uint32_t transformFlags);
    
    cocos2d::EventListenerTouchOneByOne* _touchListener;
    cocos2d::EventListenerKeyboard* _keyboardListener;
    cocos2d::CustomCommand _customCmd;
};

#endif /* defined(__TumTum_Base__GameScene_Box2D__) */
