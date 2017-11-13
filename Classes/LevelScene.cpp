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

USING_NS_CC;
using namespace cocos2d::experimental;

Light* Light::create(std::vector<Letter *> letters, cocos2d::Vec2 start, LevelScene * levelScene) {
    Light* pSprite = new Light(letters, start, levelScene);
    if (pSprite->initWithSpriteFrameName("diamond.png"))
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
    
    for (auto l : letters) {
        auto sound = CallFunc::create([=](){
            playSound("woosh2", false, randFloat(0.75, 3), 0.5f);
        });
        ftas.pushBack(sound);
        
        ccBezierConfig bezier;
        auto dest = l->getParent()->convertToWorldSpace(l->getPosition());
        bezier.controlPoint_1 = Vec2(rand() % 640, start.y + (dest.y-start.y) * 0.35);
        bezier.controlPoint_2 = Vec2(rand() % 640, start.y + (dest.y-start.y) * 0.65);
        bezier.endPosition = dest;
        auto bzto = EaseInOut::create(BezierTo::create(BZTO_TIME, bezier), 1.5);
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
            
            drawNode->drawSegment(last.first, x.first, sz, Color4F::WHITE);
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
    
    auto x = curr.x + (dest.x - curr.x) * 0.1;
    auto y = curr.y + (dest.y - curr.y) * 0.1;
    setPosition(Vec2(x, y));
}

Scene* LevelScene::createScene(std::string fname) {
    return LevelScene::create(fname);
}

bool LevelScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    
    auto layerColor = LayerColor::create(Color4B::ORANGE);
    addChild(layerColor);
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    auto director = Director::getInstance();
    
    auto visibleSize = director->getVisibleSize();
    Vec2 origin = director->getVisibleOrigin();
    
    auto label =  Label::createWithTTF(fname, "fonts/Marker Felt.ttf", 24);
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));
    label->setColor(Color3B::WHITE);
    addChild(label,0);
    
    // create a Waved3D action
    ActionInterval* waves = Waves3D::create(100, Size(20, 40), 10, 10);
    
    auto nodeGrid = NodeGrid::create();
    nodeGrid->runAction(RepeatForever::create(waves));
    this->addChild(nodeGrid);
    
    auto linenum = 0;
    std::ifstream infile;
    std::regex re("([\\w\\d]*),\\s([\\w\\d]*),\\s([\\w\\d]*)");
    std::smatch smatch;
    
    auto fileData = FileUtils::getInstance()->getDataFromFile(fname);
    std::string content((const char*)fileData.getBytes(), fileData.getSize());
    
    auto lines = split_string(content, "\n");
    for (auto line : lines) {
        std::cout << line << "]\n";
        if (linenum == 0) {
            currentDepth = stoi(line);
        } else if (linenum == 1) {
            
        } else {
            std::vector<std::vector<char>> datum;
            std::cout << line << "[\n";
            if (std::regex_match(line, smatch, re)) {
                std::cout << line << '\n';
                for (size_t i = 1; i < smatch.size(); ++i) {
                    std::ssub_match sub_match = smatch[i];
                    std::string piece = sub_match.str();
                    std::vector<char> dat;
                    for (auto c : piece) {
                        dat.push_back(c);
                    }
                    datum.push_back(dat);
                }
                data.push_back(datum);
            }
        }
        
        linenum++;
    }
    
    for (auto depth = 0; depth < currentDepth; depth++) {
        for (auto lane = 0; lane < data[depth].size(); lane++) {
            auto datum = data[depth][lane];
            if (datum.size() > 0) {
                auto trueDepth = currentDepth - depth - 1;
                auto bbl = Bubble::create(datum, lane, trueDepth, this);
                nodeGrid->addChild(bbl);
                bubbles.push_back(bbl);
            }
        }
    }
    
    
    /*auto bbl = Bubble::create(std::vector<char>{'k'}, 0, 0, this);
    nodeGrid->addChild(bbl);
    bubbles.push_back(bbl);
    
    bbl = Bubble::create(std::vector<char>{'m', 'o', 'k'}, 1, 0, this);
    nodeGrid->addChild(bbl);
    bubbles.push_back(bbl);
    
    bbl = Bubble::create(std::vector<char>{'o', 'k'}, 2, 0, this);
    nodeGrid->addChild(bbl);
    bubbles.push_back(bbl);
    
    auto drawNode = DrawNode::create();
    nodeGrid->addChild(drawNode);
    drawNode->drawSolidCircle(Vec2(256, 256), 16, 0, 8, Color4F::YELLOW);*/
    
    
    textBox = cocos2d::ui::EditBox::create(Size(512, 80), cocos2d::ui::Scale9Sprite::createWithSpriteFrameName("input.png"));
    textBox->setPosition(Vec2(320,448));
    textBox->setFont("arial", 0);
    textBox->setMaxLength(9);
    textBox->setReturnType(cocos2d::ui::EditBox::KeyboardReturnType::DONE);
    textBox->setDelegate(this);
    textBox->setInputFlag(cocos2d::ui::EditBox::InputFlag::INITIAL_CAPS_ALL_CHARACTERS);
    textBox->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
    textBox->setInputFlag(cocos2d::ui::EditBox::InputFlag::SENSITIVE);
    textBox->setFontSize(0);
    addChild(textBox, 0);
    
    editLabel = Label::createWithTTF("", "fonts/ccs.ttf", 56);
    editLabel->setTextColor(Color4B::BLACK);
    editLabel->setPosition(Vec2(320,448));
    addChild(editLabel, 1);
    
    return true;
    
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
    auto ds = 0;
    auto spc = 70;
    
    if (lastLen < strlen) {
        auto card = Card::create(text.back());
        card->setPosition(Vec2(320 + getX(cardsLen+1, cardsLen, spc), 448-50));
        cards.push_back(card);
        card->setScale(0, 0.4);
        card->setOpacity(64);
        
        auto scaleTo = ScaleTo::create(0.2, 0.4);
        auto fadeTo = FadeTo::create(0.2, 255);
        auto moveBy = EaseBackOut::create(MoveBy::create(0.2, Vec2(0, 50)));
        card->runAction(fadeTo);
        card->runAction(moveBy);
        card->runAction(scaleTo);
        cardsLen++;
        
        playSound("woosh5", false, 2.0f + float(strlen)/5.0f, 0.2f, 0.5f, 0.2f);
        addChild(card, 100);
    } else {
        auto callback = CallFunc::create([=](){
            cardsLen--;
            auto i = 0;
            for (auto c : cards) {
                auto x = getX(cardsLen, i, spc);
                c->idx = i;
                c->dest = Vec2(320 + x,448);
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
        playSound("vanish", false, 2.0f + float(strlen)/5.0f, 0.2f, 0.5f, 0.1f);
    }
    
    auto i = 0;
    for (auto c : cards) {
        auto x = getX(cardsLen, i, spc);
        c->idx = i;
        c->dest = Vec2(320 + x,448);
        i++;
    }
    
    for (auto b : bubbles) {
        b->currentWord(text);
    }
    
    lastLen = strlen;
}

void LevelScene::editBoxReturn(cocos2d::ui::EditBox* editBox) {
    std::vector<std::pair<Letter *, int>> vec;
    auto text = std::string(editBox->getText());
    for (auto b : bubbles) {
        auto x = b->currentWord(text);
        for (auto q : x) {
            vec.push_back(q);
        }
    }
    
    textBox->setText("");
    int i = 0;
    for (auto c : cards) {
        if (!c->ded) {
            std::vector<Letter *> ls;
            for (auto v: vec) {
                if (v.second == i) {
                    ls.push_back(v.first);
                }
            }
            
            auto l = Light::create(ls, c->getPosition(), this);
            addChild(l);
            c->runAction(RemoveSelf::create());
        }
        i++;
    }
    cards.clear();
    lastLen = 0;
    cardsLen = 0;
    
    textBox->setFocused(true);
}

void LevelScene::addLightRetain() {
    lightRetain++;
}

void LevelScene::removeLightRetain() {
    lightRetain--;
    if (lightRetain <= 0) {
        for (auto b : bubbles) {
            b->recalculate();
        }
    }
}

void LevelScene::addBubbleRetain() {
    bubbleRetain++;
}

void LevelScene::removeBubbleRetain() {
    bubbleRetain--;
    if (bubbleRetain <= 0) {
        //????
    }
}

