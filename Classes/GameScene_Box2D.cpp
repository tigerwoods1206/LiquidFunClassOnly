//
//  GameScene_Box2D.cpp
//  TumTum_Base
//
//  Created by ohtaisao on 2014/12/19.
//
//

#include "GameScene_Box2D.h"
#include "BallSprite.h"

Scene *GameScene_Box2D::createGameScene_Box2D()
{
    Scene *scene = Scene::create();
    auto layer = GameScene_Box2D::create();
    scene->addChild(layer);
    return scene;
}

bool GameScene_Box2D::init()
{
    if ( !Layer::init() ) {
        return false;
    }
    
    this->initPhysics();
    
    //event listener
    //イベントリスナー作成
    auto listener = EventListenerTouchAllAtOnce::create();
    
    //イベントを飲み込むかどうか
    //listener->setSwallowTouches(true);
    
    //タッチメソッド設定
    listener->onTouchesBegan = CC_CALLBACK_2(GameScene_Box2D::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(GameScene_Box2D::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(GameScene_Box2D::onTouchesEnded, this);
    
    //this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    //優先度100でディスパッチャーに登録
    this->getEventDispatcher()->addEventListenerWithFixedPriority(listener, 100);
    
    this->fillBoll();
    
    scheduleUpdate();

    this->scheduleUpdate();
    
    return true;
}

void GameScene_Box2D::initPhysics()
{
    b2Vec2 gravity = b2Vec2(0, -9.8);    //重力を作成
    _world = new b2World(gravity);       //Worldを作成
   // b2DestructionListener *listener;
    //_world->SetDestructionListener(listener);
   // _world->SetDestructionListener(this);
    
    _debugDraw = new GLESDebugDraw( PTM_RATIO );
    _world->SetDebugDraw(_debugDraw);
    /*
    const b2ParticleSystemDef particleSystemDef;
    //particleSystemDef->flags = b2_waterParticle;
    
    _particleSystem = _world->CreateParticleSystem(&particleSystemDef);
    //_particleSystem->SetRadius(50/PTM_RATIO);
    
    b2ParticleGroupDef groupDef;
    //groupDef.flags = b2_waterParticle;
    _particleGroup = _particleSystem->CreateParticleGroup(groupDef);
    */
    
    
    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    //flags += b2Draw::e_particleBit;
    //        flags += b2Draw::e_jointBit;
    //        flags += b2Draw::e_aabbBit;
    //        flags += b2Draw::e_pairBit;
    //        flags += b2Draw::e_centerOfMassBit;
    _debugDraw->SetFlags(flags);
    
    //画面サイズサイズを取得
    auto window_size = Director::getInstance()->getWinSize();
    
    //床
    auto floor = cocos2d::extension::PhysicsSprite::create();
    floor->setTextureRect(Rect(0,0,window_size.width,100));
    _floor = floor->getSpriteFrame()->getRect();
    auto size = floor->getContentSize();
   
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(0, -size.height/PTM_RATIO);
    b2Body *body = _world->CreateBody(&bodyDef);
    
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(size.width/PTM_RATIO, size.height/PTM_RATIO);
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    body->CreateFixture(&fixtureDef);
    floor->setColor(Color3B(255,255,255));
    floor->setTag(spriteType::kFloor);
    
   // addChild(floor);
    
    auto left_wall =  cocos2d::extension::PhysicsSprite::create();
    left_wall->setTextureRect(Rect(10,window_size.height/2,10,window_size.height*2));
    //left_wall->setPosition(Vec2(10, window_size.height/2));
    left_wall->setColor(Color3B(255,255,255));
    _leftwall = left_wall->getSpriteFrame()->getRect();
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(0, 0);
    b2Body *bodyleft = _world->CreateBody(&bodyDef);
    
    b2PolygonShape dynamicBoxleft;
    dynamicBoxleft.SetAsBox(10/PTM_RATIO, window_size.height*2/PTM_RATIO);
    
    b2FixtureDef fixtureDefleft;
    fixtureDefleft.shape = &dynamicBoxleft;
    fixtureDefleft.density = 1.0f;
    bodyleft->CreateFixture(&fixtureDefleft);

    auto right_wall =  cocos2d::extension::PhysicsSprite::create();
    right_wall->setTextureRect(Rect(window_size.width,window_size.height/2,10,window_size.height*2));
    size = right_wall->getContentSize();
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(window_size.width/PTM_RATIO, 0);
    b2Body *bodyright_wall = _world->CreateBody(&bodyDef);
    
    b2PolygonShape dynamicBoxright_wall;
    dynamicBoxright_wall.SetAsBox(10/PTM_RATIO, window_size.height*2/PTM_RATIO);
    
    b2FixtureDef fixtureDefright;
    fixtureDefright.shape = &dynamicBoxright_wall;
    fixtureDefright.density = 1.0f;
    bodyright_wall->CreateFixture(&fixtureDefright);

    right_wall->setColor(Color3B(255,255,255));
    _rightwall = right_wall->getSpriteFrame()->getRect();
    //addChild(right_wall);
    
    _ceil = Rect(window_size.width/2, window_size.height - 200, window_size.width, 100);
    

}

void GameScene_Box2D::draw(Renderer *renderer, const Mat4 &transform, uint32_t transformFlags)
{
    Layer::draw(renderer, transform, transformFlags);
    Director* director = Director::getInstance();
    GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION );
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    _world->DrawDebugData();
    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}
#pragma mark -
#pragma mark update

void GameScene_Box2D::update(float dt)
{
    int velocityIterations = 10;
    int positionIterations = 10;
    this->setHilightAllAdjacent();
    this->delTouchedBalls();
    this->refillBoll();
    _world->Step(dt, velocityIterations, positionIterations);
    //this->updateParticles(dt);
}

void GameScene_Box2D::updateParticles(float dt)
{
    //ユーザーデータ取得
    void ** userData = _particleSystem->GetUserDataBuffer() + _particleGroup->GetBufferIndex();
    
    //色リスト取得
    b2ParticleColor * colorList = _particleSystem->GetColorBuffer();
    
    //座標リスト取得
    b2Vec2* posList = _particleSystem->GetPositionBuffer();
    
    //グループが管理しているパーティクルのデータ更新
    for(int i = 0; i != _particleGroup->GetParticleCount();i++,colorList++,posList++,userData++){
        ((Sprite*)(*userData))->setPosition((*posList).x*PTM_RATIO,(*posList).y*PTM_RATIO);
        ((Sprite*)(*userData))->setColor(Color3B((*colorList).r, (*colorList).g, (*colorList).b));
        ((Sprite*)(*userData))->setOpacity((*colorList).a);
        
        //BallSprite *ball = (BallSprite*)(*userData);
        //if(ball==NULL) continue;
        //ball->cocos2d::Node::setPosition(Vec2((*posList).x*PTM_RATIO,(*posList).y*PTM_RATIO));
    }
}

GameScene_Box2D::~GameScene_Box2D()
{
    delete _debugDraw;    //debugDrawを解放
    _debugDraw = NULL;
    
    delete _world;        //Worldを解放
    _world = NULL;
}

#pragma mark -
#pragma mark ボール生成
BallSprite *GameScene_Box2D::createSprite(Vec2 &pos){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> intval(0,3);
    
    int val = (int)intval(mt);
    __String *filename;
    BallSprite::ballType type;
    
    switch (val) {
        case 0:
            filename = String::create("ball.png");
            type = BallSprite::ballType::kRed;
            break;
        case 1:
            filename = String::create("pur_ball.png");
            type = BallSprite::ballType::kPurple;
            break;
        case 2:
            filename = String::create("blue_ball.png");
            type = BallSprite::ballType::kBrue;
            break;
        case 3:
            filename = String::create("yellow_ball.png");
            type = BallSprite::ballType::kYellow;
            break;
        default:
            filename = String::create("ball.png");
            type = BallSprite::ballType::kBrue;
            break;
    }
    
    BallSprite *ball = BallSprite::createBallSprite(this, _world, filename->getCString());
    //BallSprite *ball = BallSprite::createParticleSprite(this, _world, _particleSystem, _particleGroup, filename->getCString());
    ball->setPosition(pos);
    //ball->setParticlePpos(_particleSystem, _particleGroup, pos.x, pos.y);
    ball->setTag(spriteType::kBall);
    ball->setBallType(type);
    _bollArray.push_back(ball);
    return ball;
}

void GameScene_Box2D::addParticle(){
    
    //発生起点
    b2Vec2 origin(300.0f/PTM_RATIO,300.0f/PTM_RATIO);
    
    //パーティクル加速度
    b2Vec2 velocity(0,100.0f);
    
    b2ParticleDef particleDef;
    particleDef.flags = b2_waterParticle;
    particleDef.flags |= b2_destructionListenerParticle;
    particleDef.color = b2ParticleColor(100,150,255,200);
    
    //パーティクルの寿命
   // particleDef.lifetime = 2.0f;
    
    //グループへのポインタを渡しておく事でそのグループ内で管理する事ができる。
    particleDef.group = _particleGroup;
    
    //設定した数値の間で発射方向をランダムに設定
    float ratep = (float)rand() / (float)RAND_MAX;
    particleDef.position.x = origin.x + (100 * ratep)/PTM_RATIO - 100/PTM_RATIO;
    particleDef.position.y = origin.y + (100 * ratep)/PTM_RATIO - 100/PTM_RATIO;
    
    //設定した数値の間でランダムな位置にパーティクルを生成
    float ratev = (float)rand() / (float)RAND_MAX;
    particleDef.velocity.x = velocity.x + (5 * ratev) - (5 / 2);
    
    //画像設定
    Sprite * image = Sprite::create("ball.png");
    
    image->setPositionX(particleDef.position.x * PTM_RATIO);
    image->setPositionY(particleDef.position.y * PTM_RATIO);
    
    //描画担当レイヤに渡す
    addChild(image);
    
    particleDef.userData = image;
    _particleSystem->CreateParticle(particleDef);
    
}

#pragma mark -
#pragma mark タップ処理
void GameScene_Box2D::onTouchesBegan(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event){
    
    Director* pDirector = CCDirector::getInstance();
    Point touchPoint = pDirector -> convertToGL(touches.at(0) -> getLocationInView());
    for (auto ball : _bollArray) {
        
        //Rect targetBox = ball->getBoundingBox();
        if (ball->isInCircle(touchPoint, this))
        {
            ball->setIsTouchBegan();
            ball->setBallHilightType(BallSprite::ballHilightType::kFirstTouch);
            ball->setDeleteState(BallSprite::deleteState::kPreDelete);
            return;
        }
    }
    
}

void GameScene_Box2D::onTouchesMoved(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event){
    Director* pDirector = CCDirector::getInstance();
    Point touchPoint = pDirector -> convertToGL(touches.at(0) -> getLocationInView());
    
    for (auto ball : _bollArray) {
        //Rect targetBox = ball->getBoundingBox();
        if (
            ball->isInCircle(touchPoint, this) &&
            (ball->getBallHilightType()==BallSprite::ballHilightType::kAdjacent)
            ) {
            ball->setIsTouchMoved();
            ball->setBallHilightType(BallSprite::ballHilightType::kTouch);
            ball->setDeleteState(BallSprite::deleteState::kPreDelete);
            break;
        }
    }
}

void GameScene_Box2D::onTouchesEnded(const std::vector<cocos2d::Touch *> &touches, cocos2d::Event *event){
    /*
     for (auto ball : _bollArray) {
     ball->getPhysicsBody()->setEnable(true);
     }
     */
    Director* pDirector = CCDirector::getInstance();
    Point touchPoint = pDirector -> convertToGL(touches.at(0) -> getLocationInView());
    
    for (auto ball : _bollArray) {
        ball->setBallHilightType(BallSprite::ballHilightType::kNoTouch);
        if (ball->getDeleteState() == BallSprite::deleteState::kPreDelete) {
            ball->setDeleteState(BallSprite::deleteState::kDelete);
            ball->setIsTouchEnd();
        }
    }
}

#pragma mark -
#pragma mark 衝突処理
bool GameScene_Box2D::onContactBegin(PhysicsContact& contact)
{
    //衝突したら呼ばれる
    //ノードを取得
    return true;
}

void GameScene_Box2D::onContactPostSolve(PhysicsContact& contact, const PhysicsContactPostSolve& solve)
{
    auto node1 = contact.getShapeA()->getBody()->getNode();
    auto node2 = contact.getShapeB()->getBody()->getNode();
    if (node1->getTag()== spriteType::kFloor && node2->getTag()== spriteType::kBall) {
        BallSprite *ball = (BallSprite *)node2;
        bool ball_state_none    = (ball->getBallContState() == BallSprite::ballContactState::kNone);
        if (ball_state_none) {
            ball->setBallContState(BallSprite::ballContactState::kFloorContact);
        }
        
    }
    else if (node2->getTag()== spriteType::kFloor && node1->getTag()== spriteType::kBall) {
        BallSprite *ball = (BallSprite *)node1;
        bool ball_state_none    = (ball->getBallContState() == BallSprite::ballContactState::kNone);
        ball->setBallContState(BallSprite::ballContactState::kFloorContact);
        if (ball_state_none) {
            ball->setBallContState(BallSprite::ballContactState::kFloorContact);
        }
    }
    else if (node2->getTag()== spriteType::kBall && node1->getTag()== spriteType::kBall) {
        BallSprite *ball1 = (BallSprite *)node1;
        BallSprite *ball2 = (BallSprite *)node2;
        bool ball1_state_floor   = (ball1->getBallContState() == BallSprite::ballContactState::kFloorContact);
        bool ball2_state_floor   = (ball2->getBallContState() == BallSprite::ballContactState::kFloorContact);
        bool ball1_state_none    = (ball1->getBallContState() == BallSprite::ballContactState::kNone);
        bool ball2_state_none    = (ball2->getBallContState() == BallSprite::ballContactState::kNone);
        bool ball1_state_second  = (ball1->getBallContState() == BallSprite::ballContactState::kSecondContact);
        bool ball2_state_second  = (ball2->getBallContState() == BallSprite::ballContactState::kSecondContact);
        
        if (ball1_state_none || ball2_state_floor) {
            ball1->setBallContState(BallSprite::ballContactState::kSecondContact);
        }
        else if (ball1_state_floor || ball2_state_none) {
            ball2->setBallContState(BallSprite::ballContactState::kSecondContact);
        }
        else if (ball1_state_none || ball2_state_second) {
            ball1->setBallContState(BallSprite::ballContactState::kSecondContact);
        }
        else if (ball2_state_none || ball1_state_second) {
            ball2->setBallContState(BallSprite::ballContactState::kSecondContact);
        }
        
    }
    
}


#pragma mark -
#pragma mark ボール満タン
bool GameScene_Box2D::isFullBoll(BallSprite *ball) {
    if (ball->getBallContState()!= BallSprite::ballContactState::kNone) {
        return false;
    }
    
    Rect ballRect = ball->getSpriteFrame()->getRect();
    
    if (_ceil.intersectsRect(ballRect)) {
        return true;
    }
    
    return false;
}

void GameScene_Box2D::fillBoll() {
    std::random_device rd;
    
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> score(1.0,1.8);
    
    auto window_size = Director::getInstance()->getWinSize();
    float radius = BallSprite::getBallRadius();
    Size vir_rectSize = Size(radius*2, radius*2);
    for (float pos_x = 0; pos_x <= window_size.width; pos_x += vir_rectSize.width *score(mt) ) {
        for (float pos_y = 0; pos_y <= window_size.height; pos_y += vir_rectSize.height * score(mt)) {
            Vec2 pos = Vec2(pos_x, pos_y);
            BallSprite *ball = this->createSprite(pos);
            if (this->isFullBoll(ball)) {
                return;
            }
        }
    }
}

void GameScene_Box2D::refillBoll()
{
    
    if ((int)_delballPos.size()==0) {
        return;
    }
    auto window_size = Director::getInstance()->getWinSize();
    for (Vec2 curPos : _delballPos) {
        if (curPos.x < 120) {
            curPos.x += 120;
        }
        else if (curPos.x > window_size.width -120) {
            curPos.x -= 120;
        }
        Vec2 refilPos = Vec2(curPos.x, window_size.height-120);
        this->createSprite(refilPos);
    }
    
    _delballPos.clear();
    
}

#pragma mark -
#pragma mark ボール削除 in update
void GameScene_Box2D::delTouchedBalls() {
    
    std::vector<BallSprite*>::iterator tempIterator;
    std::vector<BallSprite*> tmpbollArray;
    
    std::vector<std::vector<BallSprite*>::iterator> deleteIteratorArray;
    
    for( tempIterator = _bollArray.begin(); tempIterator != _bollArray.end(); tempIterator++ ) {
        BallSprite *ball = (BallSprite *)*tempIterator;
        if (ball->getDeleteState()==BallSprite::deleteState::kDelete) {
            _delballPos.push_back(ball->getPosition());
            _world->DestroyBody(ball->getB2Body());
            //ball->deleteParticle(_particleSystem,_particleGroup);
            ball->removeFromParent();
        }
        else {
            tmpbollArray.push_back(ball);
        }
    }
    
    _bollArray.clear();
    for (auto nodelball = tmpbollArray.begin(); nodelball != tmpbollArray.end(); nodelball++) {
        BallSprite *ball = (BallSprite *)*nodelball;
        _bollArray.push_back(ball);
    }
    
}

#pragma mark -
#pragma mark ボールハイライト
void GameScene_Box2D::setHilightAllAdjacent() {
    
    for (BallSprite* ball : _bollArray) {
        if (ball->getBallHilightType() == BallSprite::ballHilightType::kFirstTouch) {
            this->srchAllAdjacent(ball);
            this->srchNextAdjacent(ball);
            ball->setBallHilightType(BallSprite::ballHilightType::kChain);
            return;
        }
    }
    
    if(!this->isExistTouch()) {
        return;
    }
    
    for (BallSprite* ball : _bollArray) {
        if (ball->getBallHilightType() == BallSprite::ballHilightType::kAdjacent ) {
            
            ball->setBallHilightType(BallSprite::ballHilightType::kChain);
        }
    }
    for (BallSprite* ball : _bollArray) {
        if (
            (ball->getBallHilightType() == BallSprite::ballHilightType::kTouch)
            )
        {
            this->srchNextAdjacent(ball);
            ball->setBallHilightType(BallSprite::ballHilightType::kChain);
            return;
        }
    }
    
}

void GameScene_Box2D::srchAllAdjacent(BallSprite* cur_ball) {
    
    if (cur_ball==NULL) {
        return;
    }
    
    Vec2  centerpos = cur_ball->getPosition();
    float radius    = cur_ball->getBallRadius() * 1.4;
    float dist;
    for (BallSprite* ball : _bollArray) {
        if (ball->getBallHilightType() == BallSprite::ballHilightType::kNoTouch )
        {
            Vec2 cur_ballpos = ball->getPosition();
            dist = centerpos.getDistance(cur_ballpos);
            if (dist <= radius*2) {
                bool sametype = (ball->getBallType()==cur_ball->getBallType());
                bool nodelete = (ball->getDeleteState()==BallSprite::deleteState::kNoDelete);
                if (sametype && nodelete) {
                    ball->setBallHilightType(BallSprite::ballHilightType::kChain);
                    this->srchAllAdjacent(ball);
                }
            }
        }
    }
    return;
}

void GameScene_Box2D::srchNextAdjacent(BallSprite* cur_ball) {
    if (cur_ball==NULL) {
        return;
    }
    
    Vec2  centerpos = cur_ball->getPosition();
    float radius    = cur_ball->getBallRadius() * 1.4;
    float dist;
    for (BallSprite* ball : _bollArray) {
        if (ball->getBallHilightType() == BallSprite::ballHilightType::kChain )
        {
            Vec2 cur_ballpos = ball->getPosition();
            dist = centerpos.getDistance(cur_ballpos);
            if (dist <= radius*2) {
                bool sametype = (ball->getBallType()==cur_ball->getBallType());
                bool nodelete = (ball->getDeleteState()==BallSprite::deleteState::kNoDelete);
                bool istouch  = (ball->getBallHilightType()==BallSprite::ballHilightType::kTouch);
                if (sametype && nodelete && !istouch ) {
                    ball->setBallHilightType(BallSprite::ballHilightType::kAdjacent);
                    //this->srchNextAdjacent(ball);
                }
            }
        }
    }
    return;
    
}

void GameScene_Box2D::setHilightTypeAll(BallSprite::ballHilightType type) {
    for (BallSprite* ball : _bollArray) {
        ball->setBallHilightType(type);
    }
}

bool GameScene_Box2D::isExistTouch() {
    for (BallSprite* ball : _bollArray) {
        if (
            (ball->getBallHilightType() == BallSprite::ballHilightType::kTouch)
            )
        {
            return true;
        }
    }
    return false;
}

