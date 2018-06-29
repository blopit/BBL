//
//  LevelScene.cpp
//  BBL
//
//  Created by Shrenil Patel on 2017-11-09.
//

#include "LevelScene.hpp"
#include <sstream>
#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <algorithm>
#include "GameManager.hpp"
#include "PluginSdkboxAds/PluginSdkboxAds.h"
#include "ColorConv.hpp"

USING_NS_CC;
using namespace cocos2d::experimental;

#define iar(pRet) if (pRet && pRet->init()) \
{\
    pRet->autorelease();\
    return pRet;\
}\
else\
{\
    delete pRet;\
    pRet = NULL;\
    return NULL;\
}

#define iSprite(pSprite, spr) if (pSprite->initWithSpriteFrameName(spr))\
{\
    pSprite->autorelease();\
    pSprite->init();\
    return pSprite;\
}\
CC_SAFE_DELETE(pSprite);\
return NULL;\

Coin* Coin::create(cocos2d::Vec2 start, cocos2d::Vec2 end, LevelScene * levelScene) {
    Coin* pRet = new Coin(start, end, levelScene);
    iSprite(pRet, "coin.png");
}

bool Coin::init() {
    levelScene->coinLi += 0.05;
    auto li = levelScene->coinLi;
    auto sound = CallFunc::create([=] () mutable {
        playSound("coin", false, 0.9 + li, 0.1f);
        levelScene->li1 = 1.0;
        levelScene->coins += 1;
    });
    
    setScale(0);
    setPosition(start);
    //setPosition(start + Vec2(32 - std::rand() % 64 , 32 - std::rand() % 64));
    auto d1 = Vec2(64 - std::rand() % 128 , 64 - std::rand() % 128);
    
    ccBezierConfig bezier;
    //auto dest = l->getParent()->convertToWorldSpace(end);
    bezier.controlPoint_1 = Vec2(rand() % 120, start.y + (end.y-start.y) * 0.35);
    bezier.controlPoint_2 = Vec2(rand() % 120, start.y + (end.y-start.y) * 0.65);
    bezier.endPosition = end;
    
    runAction(Sequence::create(ScaleTo::create(0.1, 0.125), DelayTime::create(0.3), MoveBy::create(1.0, d1),
                               EaseBackIn::create(BezierTo::create(1.0, bezier)), sound, RemoveSelf::create(), NULL));
    
    playSound("woosh3", false, 0.25 + li, 0.2f);
    scheduleUpdate();
    return true;
}

void Coin::update(float dt) {
    
}

MessagePopup* MessagePopup::create(std::string text, LevelScene *owner) {
    MessagePopup* pRet = new MessagePopup(text, owner);
    iar(pRet)
}

void MessagePopup::dismiss(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
    
    auto end = CallFunc::create([=](){
        owner->getTextBox()->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
    });
    
    panel->runAction(Sequence::create(EaseElasticOut::create(ScaleTo::create(0.3, 0)), end, RemoveSelf::create(), NULL));
    //panel->runAction(Sequence::create(FadeTo::create(0.3, 0), RemoveSelf::create(), end, NULL));
    runAction(Sequence::create(DelayTime::create(0.3), RemoveSelf::create(), end, NULL));
}

bool MessagePopup::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) {
    return true;
}

void MessagePopup::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) {
}

void MessagePopup::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) {
}

bool MessagePopup::init() {
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(MessagePopup::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(MessagePopup::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(MessagePopup::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    auto layerColor = LayerColor::create(Color4B(0, 0, 0, 128));
    addChild(layerColor);
    
    auto director = Director::getInstance();
    auto visibleSize = director->getVisibleSize();
    
    panel = Sprite::createWithSpriteFrameName("message.png");
    panel->setPosition(visibleSize/2);
    addChild(panel);
    panel->setScale(0);
    panel->runAction(EaseElasticOut::create(ScaleTo::create(0.3, 1)));
    
    scoreLabel = Label::createWithBMFont("fonts/bbl.fnt", text);
    scoreLabel->setColor(Color3B::WHITE);
    scoreLabel->setPosition(Vec2(286, 380));
    scoreLabel->setDimensions(500, 0);
    scoreLabel->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    panel->addChild(scoreLabel, 1);
    
    auto uDismiss = cocos2d::ui::Button::create();
    uDismiss->setTouchEnabled(true);
    uDismiss->loadTextures("menu-button.png", "menu-pressed.png", "");
    uDismiss->setPosition(Vec2(288, 128));
    uDismiss->addTouchEventListener(CC_CALLBACK_2(MessagePopup::dismiss, this));
    panel->addChild(uDismiss);
    
    return true;
}
    
EndPopup* EndPopup::create(int score, int stars, bool gold, std::vector<std::string> secretWords, bool death, LevelScene *owner) {
    EndPopup* pRet = new EndPopup(score, stars, gold, secretWords, death, owner);
    iar(pRet)
}

bool EndPopup::init() {
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(EndPopup::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(EndPopup::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(EndPopup::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    this->scheduleUpdate();
    //auto ss = Director::getInstance()->getVisibleSize();
    
    auto layerColor = LayerColor::create(Color4B(0, 0, 0, 128));
    addChild(layerColor);
    
    
    auto director = Director::getInstance();
    auto visibleSize = director->getVisibleSize();
    
    auto panel = Sprite::createWithSpriteFrameName("panel.png");
    panel->setPosition(visibleSize/2);
    addChild(panel);
    
    auto star1 = Sprite::createWithSpriteFrameName("stars/silver.png");
    auto star2 = Sprite::createWithSpriteFrameName("stars/silver.png");
    auto star3 = Sprite::createWithSpriteFrameName("stars/silver.png");
    
    auto corner = Vec2(0, visibleSize.height);
    
    auto rto = 3;
    auto rp = 3;
    
    float waitt = 0.5 * (stars - 1);
    
    Vec2 locs = Vec2(0, 0);
    int am = 5;
    float ix = 0;
    
    if (!death) {
        if (stars >= 1) {
            panel->addChild(star1);
            auto sound = CallFunc::create([=] () {
                playSound("star", false, 0.5, 1.0f);
            });
            auto coinc = CallFunc::create([=]() mutable {
                ix += MATH_PIOVER2 * 4 / 5;
                auto vp = Vec2(50 * sin(ix), 50 * cos(ix));
                auto c = Coin::create(star1->getParent()->convertToWorldSpace(star1->getPosition()) + vp,
                                      corner, owner);
                addChild(c, 200);
            });
            Vector<cocos2d::FiniteTimeAction *> ftas;
            ftas.pushBack(DelayTime::create(0.5));
            ftas.pushBack(sound);
            ftas.pushBack(EaseElasticOut::create(ScaleTo::create(0.5, 1.0)));
            ftas.pushBack(DelayTime::create(waitt));
            for (auto i = 0; i < am; i++) {
                ftas.pushBack(DelayTime::create(0.1));
                ftas.pushBack(coinc);
            }
            
            star1->setPosition(Vec2(91, 521));
            star1->setScale(0);
            star1->runAction(Sequence::create(ftas));
            star1->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(RotateTo::create(rp, rto), 1.5), EaseInOut::create(RotateTo::create(rp, -rto), 1.5), NULL)));
        }
        if (stars >= 2) {
            panel->addChild(star2);
            auto sound = CallFunc::create([=](){
                playSound("star", false, 1, 1.0f);
            });
            auto coinc = CallFunc::create([=]() mutable {
                ix += MATH_PIOVER2 * 4 / 5;
                auto vp = Vec2(50 * sin(ix), 50 * cos(ix));
                auto c = Coin::create(star2->getParent()->convertToWorldSpace(star2->getPosition()) + vp,
                                      corner, owner);
                addChild(c, 200);
            });
            Vector<cocos2d::FiniteTimeAction *> ftas;
            ftas.pushBack(DelayTime::create(1.0));
            ftas.pushBack(sound);
            ftas.pushBack(EaseElasticOut::create(ScaleTo::create(0.5, 1.0)));
            ftas.pushBack(DelayTime::create(waitt));
            for (auto i = 0; i < am; i++) {
                ftas.pushBack(DelayTime::create(0.1));
                ftas.pushBack(coinc);
            }
            
            star2->setPosition(Vec2(288, 587));
            star2->setScale(0);
            star2->runAction(Sequence::create(ftas));
            star2->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(RotateTo::create(rp, rto), 1.5), EaseInOut::create(RotateTo::create(rp, -rto), 1.5), NULL)));
        }
        if (stars >= 3) {
            panel->addChild(star3);
            auto sound = CallFunc::create([=](){
                playSound("star", false, 1.5, 1.0f);
            });
            auto coinc = CallFunc::create([=]() mutable {
                ix += MATH_PIOVER2 * 4 / 5;
                auto vp = Vec2(50 * sin(ix), 50 * cos(ix));
                auto c = Coin::create(star3->getParent()->convertToWorldSpace(star3->getPosition()) + vp,
                                      corner, owner);
                addChild(c, 200);
            });
            Vector<cocos2d::FiniteTimeAction *> ftas;
            ftas.pushBack(DelayTime::create(1.5));
            ftas.pushBack(sound);
            ftas.pushBack(EaseElasticOut::create(ScaleTo::create(0.5, 1.0)));
            ftas.pushBack(DelayTime::create(waitt));
            for (auto i = 0; i < am; i++) {
                ix += MATH_PIOVER2 * 4 / 5;
                ftas.pushBack(DelayTime::create(0.1));
                ftas.pushBack(coinc);
            }
            
            star3->setPosition(Vec2(486, 521));
            star3->setScale(0);
            star3->runAction(Sequence::create(ftas));
            star3->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(RotateTo::create(rp, rto), 1.5), EaseInOut::create(RotateTo::create(rp, -rto), 1.5), NULL)));
        }
    } else {
        auto sound = CallFunc::create([=](){
            std::string evils[3] = {"evil1", "evil2", "evil3"};
            random_shuffle(&evils[0], &evils[2]);
            auto p = evils[0];
            playSound(evils[0], false, 1.5, 1.0f);
        });
        auto death = Sprite::createWithSpriteFrameName("death.png");
        death->setPosition(Vec2(288, 587));
        death->setScale(0);
        death->runAction(Sequence::create(DelayTime::create(0.3), sound, EaseElasticOut::create(ScaleTo::create(0.5, 1.0)), NULL));
        death->runAction(RepeatForever::create(Sequence::create(EaseInOut::create(RotateTo::create(rp, rto), 1.5), EaseInOut::create(RotateTo::create(rp, -rto), 1.5), NULL)));
        panel->addChild(death);
    }
    
    panel->setScale(0);
    panel->runAction(EaseElasticOut::create(ScaleTo::create(1, 1.0)));
    
    scoreLabel = Label::createWithBMFont("fonts/bbl.fnt", "Score: 0");
    scoreLabel->setColor(Color3B::WHITE);
    scoreLabel->setPosition(Vec2(286, 380));
    panel->addChild(scoreLabel, 1);
    
    auto uRestart = cocos2d::ui::Button::create();
    uRestart->setTouchEnabled(true);
    uRestart->loadTextures("retry-button.png", "retry-pressed.png", "");
    uRestart->setPosition(Vec2(death ? 387 : 288, -16));
    uRestart->addTouchEventListener(CC_CALLBACK_2(EndPopup::restart, this));
    panel->addChild(uRestart);
    
    auto uMenu = cocos2d::ui::Button::create();
    uMenu->setTouchEnabled(true);
    uMenu->loadTextures("menu-button.png", "menu-pressed.png", "");
    uMenu->setPosition(Vec2(death ? 190 : 91, -16));
    //uMenu->addTouchEventListener(CC_CALLBACK_2(EndPopup::restart, this));
    panel->addChild(uMenu);
    
    if (!death) {
        auto uNext = cocos2d::ui::Button::create();
        uNext->setTouchEnabled(true);
        uNext->loadTextures("next-button.png", "next-pressed.png", "");
        uNext->setPosition(Vec2(486, -16));
        uNext->addTouchEventListener(CC_CALLBACK_2(EndPopup::next, this));
        panel->addChild(uNext);
    }
    
    std::vector<Sprite *> foundwords;
    auto i = 0;
    for (auto scr : GameManager::getInstance()->hiddenWords) {
        auto secret = strip(scr);
        std::stringstream ss;
        auto len = secret.length();
        ss << "secret/" << len << ".png";
        
        auto s = Sprite::createWithSpriteFrameName(ss.str());
        s->setPosition(Vec2(288, 272 - i * 42));
        panel->addChild(s,8);
        
        auto done = false;
        for (auto x : secretWords) {
            if (x == secret) {
                done = true;
                s->setColor(Color3B::MAGENTA);
                break;
            }
        }
        
        //if (done) {
            auto j = 0;
            for (auto c : secret) {
                std::string sc(1, c);
                if (!done) sc = "?";
                
                auto label = Label::createWithTTF(sc, "fonts/ccs.ttf", 24);
                auto x = getX(int(len), j, 32);
                label->setPosition(Vec2(s->getContentSize().width / 2 + x, 21));
                s->addChild(label, 10);
                label->setColor(done ? Color3B::WHITE : Color3B::BLACK);
                auto config = label->getTTFConfig();
                config.bold = true;
                label->setTTFConfig(config);
                if (!done) {
                    label->setOpacity(32);
                    label->setScale(0.8);
                }
                
                j++;
            }
        //}
        
        auto emit = CallFunc::create([=](){
            if (done) {
                auto emitter = ParticleSystemQuad::create("res/SecretFog.plist");
                emitter->setPosVar(Vec2(s->getContentSize().width / 2, 0));
                s->addChild(emitter, 9);
                emitter->setAutoRemoveOnFinish(true);
                emitter->setPosition(convertToWorldSpace(Vec2(s->getContentSize().width / 2, 18)));
            }
        });
        
        if (done) {
            foundwords.push_back(s);
        }
        
        s->setScale(0);
        s->runAction(Sequence::create(DelayTime::create(1+i*0.1), emit, EaseElasticOut::create(ScaleTo::create(1, 1)), NULL));
        i++;
    }
    
    /* */
    auto coinc = CallFunc::create([=]() mutable {
        ix += MATH_PIOVER2 * 4 / 5;
        auto vp = Vec2(50 * sin(ix), 50 * cos(ix));
        auto c = Coin::create(star3->getParent()->convertToWorldSpace(star3->getPosition()) + vp,
                              corner, owner);
        addChild(c, 200);
    });
    
    this->scheduleUpdate();
    return true;
}

void EndPopup::restart(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
    if (type != cocos2d::ui::Widget::TouchEventType::BEGAN) return;
    auto delayed = CallFunc::create([=](){
        GameManager::getInstance()->restart();
    });
    if (death and sdkbox::PluginAdMob::isAvailable("gameover")) {
        sdkbox::PluginAdMob::show("gameover");
        return;
    }
    runAction(Sequence::create(DelayTime::create(0.1), delayed, NULL));
}

void EndPopup::next(Ref* sender, cocos2d::ui::Widget::TouchEventType type) {
    auto delayed = CallFunc::create([=](){
        GameManager::getInstance()->next();
    });
    runAction(Sequence::create(DelayTime::create(0.1), delayed, NULL));
}
                                    
bool EndPopup::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) {
    
    return true;
}

void EndPopup::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) {
    
}

void EndPopup::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) {
    
}

void EndPopup::update(float dt) {
    dscore += boundToRange(1, int(score / ( 60 * 2.0 )), INT8_MAX);
    if (dscore > score) dscore = score;
    std::stringstream ss;
    
    ss << "Score: " << dscore;
    scoreLabel->setString(ss.str());
}

//////////////////////////////////////////////////////////////////////

Light* Light::create(std::vector<Letter *> letters, cocos2d::Vec2 start, LevelScene * levelScene) {
    Light* pSprite = new Light(letters, start, levelScene);
    if (pSprite->initWithSpriteFrameName("diamondWhite.png"))
    {
        pSprite->autorelease();
        pSprite->init();
        return pSprite;
    }
    CC_SAFE_DELETE(pSprite);
    return NULL;
}

bool Light::init() {
    levelScene->addLightRetain();
    layer = Layer::create();
    addChild(layer);
    
    drawNode = DrawNode::create();
    layer->addChild(drawNode);
    
    setPosition(start);
    
    Vector<cocos2d::FiniteTimeAction *> ftas;
    ftas.pushBack(DelayTime::create(levelScene->lightRetain*0.1));
    
    auto mylr = levelScene->lightRetain;
    for (auto l : letters) {
        auto sound = CallFunc::create([=](){
            playSound("woosh2", false, randFloat(0.5, 3), 1.0f);
            if (mylr == levelScene->lights and l == letters.back()) {
                levelScene->getPsemitter2()->stopSystem();
            }
        });
        ftas.pushBack(sound);
        
        ccBezierConfig bezier;
        auto dest = l->getParent()->convertToWorldSpace(l->getPosition());
        bezier.controlPoint_1 = Vec2(rand() % 640, start.y + (dest.y-start.y) * 0.35);
        bezier.controlPoint_2 = Vec2(rand() % 640, start.y + (dest.y-start.y) * 0.65);
        bezier.endPosition = dest;
        auto bzto = EaseInOut::create(BezierTo::create(0.2, bezier), 1.5);
        ftas.pushBack(bzto);
        
        auto callback = CallFunc::create([=](){
            l->getOwner()->popLetter(l);
        });
        ftas.pushBack(callback);
    }
    auto fadeTo = FadeTo::create(0.3, 0);
    ftas.pushBack(fadeTo);
    
    ftas.pushBack(CallFunc::create([=](){
        levelScene->removeLightRetain();
    }));
    
    ftas.pushBack(RemoveSelf::create());
    
    auto seq = Sequence::create(ftas);
    runAction(seq);
    
    this->scheduleUpdate();
    return true;
}

void Light::update(float dt) {
    layer->setPosition(convertToNodeSpace(Vec2(0,0)));
    
    drawNode->clear();
    BlendFunc bf;
    bf.src = GL_SRC_ALPHA;
    bf.dst = GL_ONE;
    drawNode->setBlendFunc(bf);
    
    auto curr = getPosition();
    float angle = 0;
    if (trail.size() > 0) {
        auto prev = trail.back().first;
        angle = atan2(curr.y - prev.y, curr.x - prev.x);
    }
    trail.push_back(std::make_pair(curr, angle));
    
    std::pair<cocos2d::Vec2, float> last = std::make_pair(Vec2(0,0), 0);
    auto i = trail.size();
    for (auto x : trail) {
        if (last.first != Vec2(0,0)) {
            //auto dist = last.first.distance(x.first);
            
            auto sz = boundToRange(0.0f, float(24.0f * pow(0.65f, i)), 20.0f);
            
            drawNode->drawSegment(last.first, x.first, sz, Color4F::WHITE/*Color4F(0.8f, 0.4f, 1.0f, 1.0f)*/);
        }
        last = x;
        i--;
    }
    
}

//////////////////////////////////////

Card* Card::create(char c)
{
    if (!isalpha(c)) {
        c = '?';
    }
    Card* pSprite = new Card(c);
    std::stringstream ss;
    std::string cstr(1, toupper(c));
    ss << "card2/" << cstr << ".psd";
    auto str = ss.str();
    
    if (pSprite->initWithSpriteFrameName(str))
    {
        pSprite->autorelease();
        pSprite->init();
        return pSprite;
    }
    CC_SAFE_DELETE(pSprite);
    return NULL;
}

bool Card::init() {
    setScale(0.35);
    this->scheduleUpdate();
    return true;
}

void Card::update(float dt) {
    auto curr = getPosition();
    
    if (int(curr.x) == int(dest.x) and int(curr.y) == int(dest.y)) return;
    
    auto x = curr.x + (dest.x - curr.x) * 0.1;
    auto y = curr.y + (dest.y - curr.y) * 0.1;
    setPosition(Vec2(x, y));
}

Scene* LevelScene::createScene(std::string fname) {
    return LevelScene::create(fname);
}

void LevelScene::combo() {
    cmbo++;
    if (cmbo < 2) {
        return;
    }
    
    auto sound = CallFunc::create([=](){
        playSound("deflate", false, ((float)cmbo)/5.0f, 1.0f);
    });
    
    std::stringstream sscc;
    sscc << "combo/x" << cmbo << ".png";
    cmb->setSpriteFrame(sscc.str());
    //cmb->runAction(FadeTo::create(0.1, 255));
    //cmb->setScale(0.7f + ((float)c)/20.0f);
    cmb->runAction(Sequence::create(
                                    sound,
                                    EaseElasticOut::create(ScaleTo::create(0.3, 1 + ((float)cmbo)/20.0f)),
                               DelayTime::create(0.7f),
                                    ScaleTo::create(0.3, 1),
                                    NULL));
}

void LevelScene::resetcombo() {
    cmbo = 0;
    cmb->runAction(ScaleTo::create(0.3, 0));
}

bool LevelScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    
    auto director = Director::getInstance();
    auto visibleSize = director->getVisibleSize();
    Vec2 origin = director->getVisibleOrigin();
    Vec2 center = origin + visibleSize / 2;
    auto corner = Vec2(0, visibleSize.height);
    GameManager::getInstance()->currentLevel = fname;
    this->scheduleUpdate();
    
    //auto layerColor = LayerColor::create(Color4B(192, 192, 255, 255));
    //addChild(layerColor);
    
    /*auto bk = Sprite::createWithSpriteFrameName("bkg1.png");
    addChild(bk);
    bk->setScale(1);
    bk->setPosition(Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2));*/
    
    auto otherCorner = Vec2(visibleSize.width, visibleSize.height) + Vec2(40, -8);
    coinAm = Sprite::createWithSpriteFrameName("coin_bk.png");
    addChild(coinAm, 30001);
    coinAm->setPosition(corner + Vec2(0, -8));
    
    help = Sprite::createWithSpriteFrameName("coin_bk.png");
    addChild(help, 30001);
    help->setPosition(otherCorner );
    
    coinAmCoin = Sprite::createWithSpriteFrameName("coin.png");
    coinAm->addChild(coinAmCoin, 30002);
    coinAmCoin->setScale(0.125);
    coinAmCoin->setPosition(Vec2(140 + 20, 50-14 ));
    
    helpAmCoin = Sprite::createWithSpriteFrameName("help.png");
    help->addChild(helpAmCoin, 30002);
    helpAmCoin->setScale(0.0625);
    helpAmCoin->setPosition(Vec2(40, 50-14 ));
    
    coinLabel = Label::createWithBMFont("fonts/bbl2.fnt", "0");
    coinLabel->setBMFontSize(32);
    coinLabel->setAnchorPoint(Vec2(0, 0.5));
    coinAm->addChild(coinLabel, 30002);
    coinLabel->setPosition(Vec2(140 + 50, 50-14 ));
    
    helpLabel = Label::createWithBMFont("fonts/bbl2.fnt", "0");
    helpLabel->setBMFontSize(32);
    helpLabel->setAnchorPoint(Vec2(0, 0.5));
    help->addChild(helpLabel, 30002);
    helpLabel->setPosition(Vec2(50, 14 ));
    
    auto cspr = Sprite::createWithSpriteFrameName("coin.png");
    help->addChild(cspr, 30002);
    cspr->setScale(0.125);
    cspr->setPosition(Vec2(20, 14 ));
    
    auto listener1 = EventListenerTouchOneByOne::create();
    listener1->setSwallowTouches(true);
    listener1->onTouchBegan = [](Touch* touch, Event* event) {
        return true; // if you are consuming it
    };
    listener1->onTouchMoved = [](Touch* touch, Event* event){
    };
    listener1->onTouchEnded = [=](Touch* touch, Event* event){
        cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([=](){
            getHint();
        });
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, help);
    
    std::stringstream sscc;
    sscc << "combo/x" << 2 << ".png";
    cmb = Sprite::createWithSpriteFrameName(sscc.str());
    addChild(cmb, 200);
    cmb->setPosition(Vec2(320, 200));
    cmb->setScale(0);
    
    ActionInterval* wv = Waves3D::create(100, Size(20, 40), 10, 12);
    
   // Waves3D::create(100, Size(20, 40), 10, 12);
    
    std::srand(time(NULL));
    skytime = rand() % 720;
    
    auto clvl = GameManager::getInstance()->currentLevel;
    hue1 = std::hash<std::string>()(clvl);
    hue2 = hue1 >> 5;
    
    hsv h = {
        (double)(hue1 % 360),
        0.8,
        1.0};
    auto rgb1 = hsv2rgb(h);
    
    hsv h2 = {
        (double)(hue2 % 360),
        0.4,
        0.7};
    auto rgb2 = hsv2rgb(h2);
    
    auto col = Color4B(Color4F(rgb1.r, rgb1.g, rgb1.b, 1.0f));
    auto col2 = Color4B(Color4F(rgb2.r, rgb2.g, rgb2.b, 1.0f));
    
    gradbk = LayerGradient::create(col2, col);
    gradbk->setVector(Vec2(0,1));
    addChild(gradbk);
    
    auto ng = NodeGrid::create();
    ng->runAction(RepeatForever::create(wv));
    addChild(ng);
    
    diag = Sprite::create("grad_diamond.png");
    ng->addChild(diag);
    
    Texture2D::TexParams params;
    params.minFilter = GL_NEAREST;
    params.magFilter = GL_NEAREST;
    params.wrapS = GL_REPEAT;
    params.wrapT = GL_REPEAT;
    
    diag->getTexture()->setTexParameters(params);
    diag->setTextureRect(Rect(0, 0, 2048, 6144));
    diag->setPosition(origin);
    diag->setOpacity(128);
    
    layer = LayerColor::create(Color4B(0, 0, 0, 0));
    addChild(layer, 8);
    
    scoreLabel =  Label::createWithTTF("0", "fonts/Marker Felt.ttf", 24);
    scoreLabel->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - scoreLabel ->getContentSize().height));
    scoreLabel->setColor(Color3B::WHITE);
    scoreLabel->enableOutline(Color4B::BLACK, 3);
    addChild(scoreLabel,1);
    
    scoreEmit = ParticleSystemQuad::create("res/Score.plist");
    scoreEmit->setSpeed(0);
    scoreEmit->setPosition(Vec2(origin.x + visibleSize.width/2, // - scorewid/2,
                       origin.y + visibleSize.height - scoreLabel ->getContentSize().height + 8));
    addChild(scoreEmit);
    
    scoreEmit2 = ParticleSystemQuad::create("res/Score.plist");
    scoreEmit2->setSpeed(0);
    scoreEmit2->setAngle(180);
    scoreEmit2->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - scoreLabel ->getContentSize().height + 8));
    addChild(scoreEmit2);
    
    
    hintLabel =  Label::createWithBMFont("fonts/bbl.fnt", "Loading...");
    hintLabel->setPosition(Vec2(origin.x + visibleSize.width/2,
                                 origin.y + visibleSize.height/2));
    //hintLabel->setColor(Color3B::WHITE);
    //hintLabel->enableOutline(Color4B::BLACK, 3);
    hintLabel->setOpacity(0);
    addChild(hintLabel,100);
    
    /*auto score =  Label::createWithTTF(fname, "fonts/Marker Felt.ttf", 24);
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));
    label->setColor(Color3B::WHITE);
    addChild(label,0);*/
    
    // create a Waved3D action
    ActionInterval* waves = Waves3D::create(100, Size(20, 40), 10, 12);
    
    nodeGrid = NodeGrid::create();
    nodeGrid->runAction(RepeatForever::create(waves));
    this->addChild(nodeGrid, 10);
    
    auto linenum = 0;
    std::ifstream infile;
    std::regex re("([\\w\\d]*)(,|-|&)\\s?([\\w\\d]*)(,|-|&)\\s?([\\w\\d]*)(|,|-|&)$");
    std::smatch smatch;
    
    std::stringstream ss;
    ss << "levels/" << fname << ".bbl";
    auto fileData = FileUtils::getInstance()->getDataFromFile(ss.str());
    std::string content((const char*)fileData.getBytes(), fileData.getSize());
    
    textBox = cocos2d::ui::EditBox::create(Size(640, 60), cocos2d::ui::Scale9Sprite::createWithSpriteFrameName("inputBook.png"));
    textBox->setPosition(Vec2(320, EDIT_DEPTH));
    textBox->setFont("arial", 0);
    textBox->setMaxLength(9);
    textBox->setReturnType(cocos2d::ui::EditBox::KeyboardReturnType::DONE);
    textBox->setDelegate(this);
    textBox->setVisible(false);
    textBox->setInputFlag(cocos2d::ui::EditBox::InputFlag::INITIAL_CAPS_ALL_CHARACTERS);
    textBox->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
    textBox->setInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
    textBox->setFontSize(0);
    addChild(textBox, 0);
    
    auto lines = split_string(content, "\n");
    currentDepth = 1;
    
    for (auto line : lines) {
        std::cout << line << "]\n";
        
        if (linenum == 0) {
           ;//stoi(line);
            levelName = line;
        } else if (linenum == 1) {
            difficulty = stoi(line);
        } else if (linenum == 2) {
            auto hw = split_string(line, ",");
            for (auto h : hw) {
                hiddenWords.push_back(strip(h));
            }
            GameManager::getInstance()->hiddenWords = hiddenWords;
        } else if (linenum == 3) {
            targetScore = stoi(line);
        } else if (linenum == 4) {
            //OPTIONS HERE
        } else if (linenum == 5) {
            
            auto callback = CallFunc::create([=](){
                if (line != "") {
                    addChild(MessagePopup::create(line, this), 2000);
                } else {
                    textBox->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
                }
            });
            std::stringstream ss;
            ss << "Level " << GameManager::getInstance()->level();
            auto lvl = Label::createWithBMFont("fonts/bbl2.fnt", ss.str());
            lvl->setBMFontSize(52);
            
            auto p1 = Vec2(center.x, origin.y - 200);
            auto p2 = center + Vec2(0, 128);
            
            lvl->setPosition(p1);
            addChild(lvl, 2000);
            
            auto creator = CallFunc::create([=](){
                auto idx = 0;
                for (auto c : levelName) {
                    if (c == ' ') {
                        idx++;
                        continue;
                    }
                    std::stringstream ssc;
                    std::string cstr(1, toupper(c));
                    ssc << "goldnum/" << cstr << ".psd";
                    auto str = ssc.str();
                    auto spr = Sprite::createWithSpriteFrameName(str);
                    spr->setPosition(p1);
                    spr->setScale(0.25);
                    addChild(spr, 1000);
                    playSound("woosh4", false, 1.5 * idx/levelName.length(), 0.1f, -1 + 2 * idx/levelName.length());
                    
                    auto p3 = Vec2(getX(levelName.size(), idx, 48), - 64);
                    
                    spr->runAction(Sequence::create(
                                                    DelayTime::create(0.1f * idx),
                                                EaseBackOut::create(MoveTo::create(1.0f, p2 + p3)),
                                                    DelayTime::create(1.5f),
                                                //EaseBackOut::create(MoveTo::create(0.f, p1)),
                                                    FadeTo::create(0.3, 0),
                                                    EaseBackOut::create(MoveTo::create(0.1, Vec2((p2+p3).x, 100))),
                                                    FadeTo::create(0.3, 255),
                                                    NULL));
                    
                    idx++;
                }
            });
            
            
            lvl->runAction(Sequence::create(
                                            EaseElasticOut::create(MoveTo::create(1.0f, p2)),
                                            creator,
                                            DelayTime::create(0.1f * levelName.size() + 2.6f),
                                            //EaseBackOut::create(MoveTo::create(0.3f, p1)),
                                            FadeTo::create(0.3, 0),
                                            DelayTime::create(0.3f),
                                            callback,
                                            NULL));
            
        } else {
            std::vector<std::pair<BubbleType, std::vector<char>>> datum;
            std::cout << line << "[\n";
            if (std::regex_match(line, smatch, re)) {
                std::cout << line << '\n';
                for (size_t i = 1; i < smatch.size(); i+=2 ) {
                    std::ssub_match sub_match = smatch[i];
                    std::ssub_match sub_match2 = smatch[i+1];
                    std::string piece = sub_match.str();
                    std::vector<char> dat;
                    for (auto c : piece) {
                        dat.push_back(c);
                    }
                    
                    if (sub_match2.str() == "-") {
                        datum.push_back(std::make_pair(BubbleType::ORDERED, dat));
                    } else if (sub_match2.str() == "&") {
                        datum.push_back(std::make_pair(BubbleType::BOMB, dat));
                    } else {
                        datum.push_back(std::make_pair(BubbleType::UNORDERED, dat));
                    }
                    
                }
                data.push_back(datum);
            }
        }
        
        linenum++;
    }
    maxDepth = linenum - 7;
    
    for (auto depth = 0; depth < currentDepth; depth++) {
        for (auto lane = 0; lane < data[depth].size(); lane++) {
            auto datum = data[depth][lane];
            if (datum.second.size() > 0) {
                auto trueDepth = currentDepth - depth - 1;
                auto bbl = Bubble::create(datum.second, lane, trueDepth, this, datum.first, 1);
                nodeGrid->addChild(bbl);
                bubbles.push_back(bbl);
            }
        }
    }
    
    book = Sprite::createWithSpriteFrameName("book.png");
    book->setScale(1);
    book->setPosition(Vec2(320, 200));
    
    auto baMove = (RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(5, Vec2(0, 5)), 1.5), EaseInOut::create(MoveBy::create(5, Vec2(0, -5)), 1.5), NULL)));
    auto baRotate = (RepeatForever::create(Sequence::create(EaseInOut::create(RotateTo::create(7, 1), 1.5), EaseInOut::create(RotateTo::create(7, -1), 1.5), NULL)));
    auto baMove2 = (RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(3, Vec2(5, 0)), 1.5), EaseInOut::create(MoveBy::create(3, Vec2(-5, 0)), 1.5), NULL)));
    baMove->setTag(1);
    baRotate->setTag(2);
    baMove2->setTag(3);
    book->runAction(baMove);
    book->runAction(baRotate);
    book->runAction(baMove2);
    
    auto listener2 = EventListenerTouchOneByOne::create();
    listener2->setSwallowTouches(true);
    
    listener2->onTouchBegan = [=](Touch* touch, Event* event) {
        cocos2d::Vec2 p = touch->getLocation();
        cocos2d::Rect rect = book->getBoundingBox();
        
        if(rect.containsPoint(p)) {
            textBox->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
            return true;
        }
        return false;
    };
    
    listener2->onTouchMoved = [](Touch* touch, Event* event) {
    };
    
    listener2->onTouchEnded = [](Touch* touch, Event* event) {
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, book);
    
    addChild(book, 1);
    
    editLabel = Label::createWithTTF("", "fonts/ccs.ttf", 56);
    editLabel->setTextColor(Color4B::BLACK);
    editLabel->setPosition(Vec2(320, EDIT_DEPTH));
    addChild(editLabel, 2);
    
    
    psemitter1 = ParticleSystemQuad::create("res/Burst.plist");
    addChild(psemitter1, 9);
    psemitter1->stopSystem();
    psemitter1->setPosition(book->getPosition());
    
    psemitter2 = ParticleSystemQuad::create("res/Gather.plist");
    addChild(psemitter2, 9);
    psemitter2->stopSystem();
    psemitter2->setPosition(book->getPosition());
    
    /*ActionInterval* wv = Waves3D::create(3, Size(40, 20), 25, 2);
    auto nd = NodeGrid::create();
    nd->runAction(RepeatForever::create(wv));
    book->addChild(nd, 10);*/
    
    /*auto spr = Sprite::createWithSpriteFrameName("flow2/flow2_K.psd");
    spr->setRotation(-90);
    spr->setScale(0.15);
    spr->setPosition(Vec2(320, 180));
    spr->setBlendFunc(BlendFunc::ADDITIVE);
    nd->addChild(spr, 0);*/
    
    /*auto endpopup = EndPopup::create(8000, 3, 100, std::vector<std::string> {"one", "two", "three", "five"}, true);
    addChild(endpopup, 20000);*/
    
    //sdkbox::PluginSdkboxAds::playAd();
    return true;
    
}

void LevelScene::update(float dt) {
    auto diffoff = (difficulty / 5);
    bkpos += 1 + diffoff;
    if (bkpos >= 2048) {
        bkpos -= 2048;
    }
    
    if (li1 - 0.05 > 0) {
        li1 -= 0.05;
    } else {
        li1 = 0;
    }
    coinAm->setScale(1 + 0.25 * li1);
    auto cstr = std::to_string(coins);
    auto corner = Vec2(0, Director::getInstance()->getVisibleSize().height);
    coinLabel->setString(cstr);
    coinbkshift = coinLabel->getBoundingBox().size.width;
    coinAm->setPosition(corner + Vec2(coinbkshift - 52, -8));
    coinAmCoin->setPosition(Vec2(140 + 52 + 20 - coinbkshift, 50-14 ));
    coinLabel->setPosition(Vec2(140 + 52 + 50 - coinbkshift, 50-14 ));
    
    float tps = (float)score/(float)targetScore;
    
    if (tps >= 1) {
        tps = 1; //MAX
        scoreEmit->setStartColor(Color4F(0, 0.6, 1, 0.2));
        scoreEmit->setEndColor(Color4F(1, 0.2, 0.6, 0.2));
        
        scoreEmit2->setStartColor(Color4F(0, 0.6, 1, 0.2));
        scoreEmit2->setEndColor(Color4F(1, 0.2, 0.6, 0.2));
    } else if (tps > 0.65) { //2 star
        scoreEmit->setEndColor(Color4F(0.3, 0.1, 1, 0.2));
        scoreEmit2->setEndColor(Color4F(0.3, 0.1, 1, 0.2));
    } else if (tps > 0.35) { //1 star
        scoreEmit->setEndColor(Color4F(0, 0.6, 1, 0.2));
        scoreEmit2->setEndColor(Color4F(0, 0.6, 1, 0.2));
    } else { //0 star
        scoreEmit->setStartColor(Color4F(0.5, 0.5, 0.5, 0.2));
        scoreEmit->setEndColor(Color4F(0.5, 0.9, 0.5, 0.2));
        scoreEmit2->setStartColor(Color4F(0.5, 0.5, 0.5, 0.2));
        scoreEmit2->setEndColor(Color4F(0.5, 0.9, 0.5, 0.2));
    }
    
    auto sp = scorewid * tps;
    
    if (csw < sp) {
        csw += (sp - csw) * 0.1;
        scoreEmit->setSpeed(csw);
        scoreEmit2->setSpeed(csw);
    }
    
    skytime += 0.004;
    
    float rad = MATH_DEG_TO_RAD(skytime*15);
    float rad2 = MATH_DEG_TO_RAD(180 + skytime*15 * 1.89);
    float rad3 = MATH_DEG_TO_RAD(90 - skytime * 20 * difficulty);
    auto c2 = (cos(rad) + 1.0f) / 2;
    auto c22 = (cos(rad2) + 1.0f) / 2;
    auto c23 = (cos(rad3) + 1.0f) / 2;
    
    auto cc1 = (((double)(hue1 % 360)) / 360 ) * 0.8 + 0.2 * c2;
    auto cc2 = (((double)(hue2 % 360)) / 360 ) * 0.8 + 0.2 * c22;
    
    hsv h = {
        (double)((int)(340 - cc1 * 210) % 360),
        0.6 + 0.2 * c2,
        0.9 + 0.1 * c2};
    auto rgb1 = hsv2rgb(h);
    
    hsv h2 = {
        (double)((int)(140 + cc2 * 290) % 360), //(140 + cc2 * 290), //skytime * 15 * 1.5,
        0.3 + 0.2 * (1 - c2),
        0.7 + 0.1 * (1 - c2)};
    auto rgb2 = hsv2rgb(h2);

    auto white = - 0.1 + c23 * 0.6;
    rgb rgb3 = {
        (rgb1.r * 0.375 * (1-white) + rgb2.r * 0.625 * (1-white) + white),
        (rgb1.g * 0.375 * (1-white) + rgb2.g * 0.625 * (1-white) + white),
        (rgb1.b * 0.375 * (1-white) + rgb2.b * 0.625 * (1-white) + white)
    };
    
    auto col = Color4F(rgb1.r, rgb1.g, rgb1.b, 1.0f);
    gradbk->setEndColor(Color3B(col));
    
    //gradbk->setEndOpacity(255 * (1 - (0.8 + 0.2 * c2)));
    
    auto col2 = Color4F(rgb2.r, rgb2.g, rgb2.b, 1.0f);
    gradbk->setStartColor(Color3B(col2));
    
    auto col3 = Color4F(rgb3.r, rgb3.g, rgb3.b, 1.0f);
    diag->setColor(Color3B(col3));
    
    float wx = sin(rad2) * 32 * diffoff / 5;
    float wy = cos(rad2) * 128 *  diffoff / 5;
    diag->setPosition(Director::getInstance()->getVisibleOrigin() + Vec2(wx, bkpos + wy));
}

void LevelScene::lost() {
    if (end) return;
    end = true;
    auto endpopup = EndPopup::create(score, 0, false, words, true, this);
    addChild(endpopup, 2000);
}

void LevelScene::dropDepth() {
    if (ending) return;
    resetcombo();
    currentDepth++;
    
    auto currBubbles = 0;
    auto currLetters = 0;
    for (auto b : bubbles) {
        if (!b->ded) {
            currBubbles++;
            currLetters += b->letters.size();
        }
    }
    auto popped = maxBubbles - currBubbles;
    auto poppedLetters = maxLetters - currLetters;
    
    //score += (5 * popped + poppedLetters) * boundToRange(10, 100 * int(pow(float(popped)/float(maxBubbles), 2)), INT32_MAX);
    score += 20 * (poppedLetters * poppedLetters);
    
    scoreLabel->setString(std::to_string(score));

    if (currentDepth > maxDepth) {
        auto next = true;
        for (auto b : bubbles) {
            if (!b->ded) {
                next = false;
                break;
            }
        }
        if (next) {
            auto stars = 1;
            if (score >= targetScore) {
                stars = 3;
            } else if (score >= targetScore/2) {
                stars = 2;
            }
            
            auto popup = CallFunc::create([=](){
                auto endpopup = EndPopup::create(score, stars, 100, words, false, this);
                addChild(endpopup, 20000);
            });
            
            runAction(Sequence::create(DelayTime::create(0.5), popup, NULL));
            
        } else {
            if (!end) textBox->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
        }
        return;
    };
    textBox->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
    for (auto lane = 0; lane < data[currentDepth-1].size(); lane++) {
        auto datum = data[currentDepth-1][lane];
        if (datum.second.size() > 0) {
            auto bbl = Bubble::create(datum.second, lane, 0, this, datum.first, currentDepth);
            nodeGrid->addChild(bbl);
            bubbles.push_back(bbl);
        }
    }
}

LevelScene *LevelScene::create(std::string fname) {
    LevelScene *pRet = new LevelScene(fname);
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

void LevelScene::editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox) {
    editBox->setFontSize(0);
}

void LevelScene::editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox) {
}

void LevelScene::editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text) {
    
    int strlen = int(text.length());
    auto txt = text;
    auto spc = 70;
    
    auto d = txt;
    d.erase( std::remove_if( d.begin(), d.end(), []( char c ) { return !std::isalpha(c) ; } ), d.end() ) ;
    editBox->setText(d.c_str());
    
    if (lastLen < strlen) {
        if (!isalpha(d.back())) return;
        auto card = Card::create(d.back());
        card->setPosition(Vec2(320 + getX(cardsLen+1, cardsLen, spc), EDIT_DEPTH-50));
        cards.push_back(card);
        card->setScale(0, 0.4);
        card->setOpacity(128);
        
        auto scaleTo = ScaleTo::create(0.2, 0.33);
        auto fadeTo = FadeTo::create(0.2, 255);
        auto moveBy = EaseBackOut::create(MoveBy::create(0.2, Vec2(0, 50)));
        card->runAction(fadeTo);
        card->runAction(moveBy);
        card->runAction(scaleTo);
        cardsLen++;
        
        playSound("woosh5", false, 2.0f + float(strlen)/5.0f, 0.5f, 0.5f, 0.5f);
        addChild(card, 100);
    } else {
        auto callback = CallFunc::create([=](){
            cardsLen--;
            auto i = 0;
            for (auto c : cards) {
                auto x = getX(cardsLen, i, spc);
                c->idx = i;
                c->dest = Vec2(320 + x, EDIT_DEPTH);
                i++;
            }
        });
        Card *last = cards.back();
        auto scaleTo = ScaleTo::create(0.1, 0, 0.5);
        auto fadeTo = FadeTo::create(0.1, 0);
        auto moveBy = EaseBackIn::create(MoveBy::create(0.1, Vec2(0, -25)));
        auto seq = Sequence::create(fadeTo, callback, RemoveSelf::create(), nullptr);
        last->runAction(scaleTo);
        last->runAction(moveBy);
        last->runAction(seq);
        last->ded = true;
        cards.pop_back();
        playSound("vanish", false, 2.0f + float(strlen)/5.0f, 0.2f, 0.5f, 0.2f);
    }
    
    auto i = 0;
    for (auto c : cards) {
        auto x = getX(cardsLen, i, spc);
        c->idx = i;
        c->dest = Vec2(320 + x, EDIT_DEPTH);
        i++;
    }
    
    for (auto b : bubbles) {
        b->currentWord(d, false);
    }
    
    lastLen = strlen;
    lastText = txt;
}

void LevelScene::getHint() {
    hintLabel->setString("Loading...");
    //std::vector<std::pair<Letter *, int>>
    auto ws = GameManager::getInstance()->smwords;
    //std::random_shuffle(ws.begin(), ws.end());
    auto mxscore = 0;
    for (auto b : this->bubbles) {
        if (b->bubbleType != BubbleType::BOMB) {
            mxscore += b->letters.size();
        }
    }
    
    auto cscore = 0;
    std::string cword = "";
    auto ite = 0;
    for (auto w : ws) {
        int score = 0;
        for (auto b : this->bubbles) {
            if (b->bubbleType == BubbleType::BOMB) {
                if (b->currentWord(w, true).size() > 0) {
                    score = 0;
                    break;
                }
            }
            score += b->currentWord(w, true).size();
        }
        if (score > cscore) {
            cscore = score;
            cword = w;
            if (score >= mxscore) {
                break;
            }
        }
        ite++;
        if (ite % 10000 == 0) {
            mxscore--;
        }
    }
    auto oword = cword;
    std::vector<int> v(cword.length());
    std::generate(v.begin(), v.end(), [n = 0] () mutable { return n++; });
    std::random_shuffle(v.begin(), v.end());
    int replace = floor(0.4 * cword.length());
    for (int i = 0; i < replace; i++) {
        cword[v[i]] = '_';
    }
    hintLabel->setString(cword);
}

void LevelScene::editBoxReturn(cocos2d::ui::EditBox* editBox) {
    auto gm = GameManager::getInstance();
    auto director = Director::getInstance();
    auto visibleSize = director->getVisibleSize();
    
    auto dict = gm->words;
    auto text = std::string(editBox->getText());
    text.erase( std::remove_if( text.begin(), text.end(), []( char c ) { return !std::isalpha(c) ; } ), text.end() ) ;
    
    /*if (text == "") {
        auto callback = CallFunc::create([=](){
            textBox->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
        });
        runAction(Sequence::create(DelayTime::create(0.5), callback, NULL));
        return;
    }*/
    
    std::string d = text;
    std::transform(d.begin(), d.end(), d.begin(), ::tolower);
    //d.erase(std::remove_if(d.begin(), d.end(), std::not1(std::ptr_fun( (int(*)(int))std::isalpha ))), d.end());
    
    words.push_back(d);
    
    if (text != "" and std::find(dict.begin(), dict.end(), d) == dict.end()) {
        auto callback = CallFunc::create([=](){
            textBox->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
        });
        runAction(Sequence::create(DelayTime::create(cards.size()*0.1+1), callback, NULL));
        
        auto i = 0;
        for (auto c : cards) {
            
            auto x = getX(cardsLen, i, 70);
            auto dst = Vec2(320 + x, EDIT_DEPTH);
            
            auto cb1 = CallFunc::create([=](){
                c->dest = dst + Vec2(0, 50);
                playSound("woosh5", false, 2.0f + float(i)/5.0f, 0.2f, 0.5f, 0.2f);
            });
            auto cb2 = CallFunc::create([=](){
                c->dest = dst;
            });
            
            c->runAction(Sequence::create(TintTo::create(0.1, 255, 128, 128), NULL));
            c->runAction(Sequence::create(DelayTime::create(0.2+i*0.1f), cb1, DelayTime::create(0.3), cb2, TintTo::create(0.1, 255, 255, 255), callback,  NULL));
            
            i++;
        }
        
        playSound("wrong", false, randFloat(0.75, 1.5));
        return;
    }
    
    std::vector<std::pair<Letter *, int>> vec;
    
    maxBubbles = 0;
    maxLetters = 0;
    for (auto b : bubbles) {
        auto x = b->currentWord(text, false);
        for (auto q : x) {
            vec.push_back(q);
        }
        if (!b->ded) {
            maxBubbles++;
            maxLetters += b->letters.size();
        }
    }
    
    auto it = std::find(hiddenWords.begin(), hiddenWords.end(), d);
    if (it != hiddenWords.end()) {
        auto spr = Sprite::createWithSpriteFrameName("secretWord.png");
        spr->setScale(0);
        spr->setPosition(visibleSize/2);
        spr->runAction(Sequence::create(EaseElasticOut::create(ScaleTo::create(0.3, 1)), DelayTime::create(2), EaseOut::create(ScaleTo::create(0.1, 0), 1.5), NULL));
        addChild(spr, 30000);
    }
    
    textBox->setText("");
    auto create = false;
    
    for (auto v: vec) {
        if (v.first->getOwner()->bubbleType == BubbleType::BOMB) {
            ending = true;
            vec.erase(std::remove_if(
                                    vec.begin(), vec.end(),
                                    [](decltype(vec)::value_type const& x) {
                                        return x.first->getOwner()->bubbleType != BubbleType::BOMB;                                        }), vec.end());
            break;
        }
    }
    
    int i = 0;
    for (auto c : cards) {
        if (!c->ded) {
            std::vector<Letter *> ls;
            for (auto v: vec) {
                if (v.second == i) {
                    ls.push_back(v.first);
                }
            }
            
            c->runAction(RemoveSelf::create());
            if (ls.size() != 0) {
                create = true;
                auto l = Light::create(ls, book->getPosition()/*c->getPosition()*/, this);
                addChild(l, 20);
                //book->runAction(Sequence::create(DelayTime::create(lightRetain*0.1), EaseOut::create(ScaleTo::create(0.1, 1.5, 1.5), 1.5), EaseBackOut::create(ScaleTo::create(0.5, 1.25)), NULL));
                /*auto action0 = ScaleTo::create(0.05f, 1.52);
                auto action1 = ScaleTo::create(0.05f, 1.48);
                auto seq = Sequence::create(action0, action1, NULL);
                book->runAction(seq);*/
            }
        }
        i++;
    }
    cards.clear();
    lastLen = 0;
    cardsLen = 0;
    
    lights = lightRetain;
    
    /************/
    
    
    /************/
    
    hintLabel->setOpacity(255);
    hintLabel->setString("Loading...");
   // cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread(); //, this, 0.0f, 0, 0, false, "name"
    
    if (create) {
        psemitter1->resetSystem();
        psemitter2->resetSystem();
        playSound("portal", false, randFloat(0.5, 2), 0.2, 0.5, 0.2);
        
        layer->runAction(FadeTo::create(0.1, 192));
        book->runAction(RotateTo::create(0.1, 0));
        book->runAction(SkewTo::create(0.1, 1, 1));
        
        book->stopAction(book->getActionByTag(1));
        book->stopAction(book->getActionByTag(2));
        book->stopAction(book->getActionByTag(3));
        
        auto action0 = EaseInOut::create(ScaleTo::create(0.1f, 1.52), 1.5);
        auto action1 = EaseInOut::create(ScaleTo::create(0.01f, 1.48), 1.5);
        auto seq = RepeatForever::create(Sequence::create(action0, action1, NULL));
        seq->setTag(4);
        book->runAction(seq);
        
    } else {
        textBox->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
    }
    
}

void LevelScene::addLightRetain() {
    lightRetain++;
}

void LevelScene::removeLightRetain() {
    lightRetain--;
    if (lightRetain <= 0) {
        
        for (auto b : bubbles) {
            if (b->letters.size() == 0 and !b->ded and b->bubbleType != BubbleType::BOMB) {
                addBubbleRetain();
                auto callback = CallFunc::create([=](){
                    removeBubbleRetain();
                });
                b->pop(callback);
            }
        }
        addBubbleRetain();
        removeBubbleRetain();
    }
}

void LevelScene::addBubbleRetain() {
    bubbleRetain++;
}

void LevelScene::removeBubbleRetain() {
    bubbleRetain--;
    if (bubbleRetain <= 0) {
        for (auto b : bubbles) {
            b->cdepth = b->depth + 1;
        }
        for (auto b : bubbles) {
            b->recalculate();
        }
        dropDepth();
        psemitter1->stopSystem();
        
        layer->runAction(FadeTo::create(0.3, 0));
        book->stopAction(book->getActionByTag(4));
        book->runAction(EaseBackOut::create(ScaleTo::create(1, 1.25f)));
        auto baMove = (RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(5, Vec2(0, 5)), 1.5), EaseInOut::create(MoveBy::create(5, Vec2(0, -5)), 1.5), NULL)));
        auto baRotate = (RepeatForever::create(Sequence::create(EaseInOut::create(RotateTo::create(7, 1), 1.5), EaseInOut::create(RotateTo::create(7, -1), 1.5), NULL)));
        auto baMove2 = (RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(3, Vec2(5, 0)), 1.5), EaseInOut::create(MoveBy::create(3, Vec2(-5, 0)), 1.5), NULL)));
        baMove->setTag(1);
        baRotate->setTag(2);
        baMove2->setTag(3);
        book->runAction(baMove);
        book->runAction(baRotate);
        book->runAction(baMove2);
    }
}

