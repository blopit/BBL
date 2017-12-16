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
#include "GameManager.hpp"

USING_NS_CC;
using namespace cocos2d::experimental;

EndPopup* EndPopup::create(int score, int stars, bool gold, std::vector<std::string> secretWords) {
    EndPopup* pRet = new EndPopup(score, stars, gold, secretWords);
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
    
    auto scoreLabel = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 24);
    addChild(scoreLabel, 1);
    
    auto director = Director::getInstance();
    auto visibleSize = director->getVisibleSize();
    
    //layerColor->setPosition(Vex);
    
    return true;
}

bool EndPopup::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) {
    
    return true;
}

void EndPopup::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) {
    
}

void EndPopup::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) {
    
}

void EndPopup::update(float dt) {
    
}

//////////////////////////////////////////////////////////////////////

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
    
    //auto layerColor = LayerColor::create(Color4B(192, 192, 255, 255));
    //addChild(layerColor);
    
    auto bk = Sprite::createWithSpriteFrameName("FieldBG.png");
    addChild(bk);
    bk->setScale(1.5);
    bk->setPosition(Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2));
    
    layer = LayerColor::create(Color4B(0, 0, 0, 0));
    addChild(layer, 8);
    
    scoreLabel =  Label::createWithTTF("0", "fonts/Marker Felt.ttf", 24);
    scoreLabel->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - scoreLabel ->getContentSize().height));
    scoreLabel->setColor(Color3B::WHITE);
    addChild(scoreLabel,0);
    
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
    std::regex re("([\\w\\d]*)(,|-)\\s?([\\w\\d]*)(,|-)\\s?([\\w\\d]*)(|,|-)$");
    std::smatch smatch;
    
    std::stringstream ss;
    ss << "levels/" << fname << ".bbl";
    auto fileData = FileUtils::getInstance()->getDataFromFile(ss.str());
    std::string content((const char*)fileData.getBytes(), fileData.getSize());
    
    auto lines = split_string(content, "\n");
    for (auto line : lines) {
        std::cout << line << "]\n";
        if (linenum == 0) {
            currentDepth = stoi(line);
        } else if (linenum == 1) {
            //SECRET WORDS
        } else if (linenum == 2) {
            targetScore = stoi(line);
        } else if (linenum == 3) {
            
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
                        datum.push_back(std::make_pair(BubbleType::UNORDERED, dat));
                    } else {
                        datum.push_back(std::make_pair(BubbleType::ORDERED, dat));
                    }
                    
                }
                data.push_back(datum);
            }
        }
        
        linenum++;
    }
    maxDepth = linenum - 5;
    
    for (auto depth = 0; depth < currentDepth; depth++) {
        for (auto lane = 0; lane < data[depth].size(); lane++) {
            auto datum = data[depth][lane];
            if (datum.second.size() > 0) {
                auto trueDepth = currentDepth - depth - 1;
                auto bbl = Bubble::create(datum.second, lane, trueDepth, this, datum.first);
                nodeGrid->addChild(bbl);
                bubbles.push_back(bbl);
            }
        }
    }
    
    book = Sprite::createWithSpriteFrameName("inputBook.png");
    book->setScale(1.25);
    book->setPosition(Vec2(320, 180));
    
    auto baMove = (RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(5, Vec2(0, 5)), 1.5), EaseInOut::create(MoveBy::create(5, Vec2(0, -5)), 1.5), NULL)));
    auto baRotate = (RepeatForever::create(Sequence::create(EaseInOut::create(RotateTo::create(7, 1), 1.5), EaseInOut::create(RotateTo::create(7, -1), 1.5), NULL)));
    auto baMove2 = (RepeatForever::create(Sequence::create(EaseInOut::create(MoveBy::create(3, Vec2(5, 0)), 1.5), EaseInOut::create(MoveBy::create(3, Vec2(-5, 0)), 1.5), NULL)));
    baMove->setTag(1);
    baRotate->setTag(2);
    baMove2->setTag(3);
    book->runAction(baMove);
    book->runAction(baRotate);
    book->runAction(baMove2);
    
    auto listener1 = EventListenerTouchOneByOne::create();
    listener1->setSwallowTouches(true);
    
    listener1->onTouchBegan = [=](Touch* touch, Event* event) {
        cocos2d::Vec2 p = touch->getLocation();
        cocos2d::Rect rect = book->getBoundingBox();
        
        if(rect.containsPoint(p)) {
            textBox->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
            return true;
        }
        return false;
    };
    
    listener1->onTouchMoved = [](Touch* touch, Event* event) {
    };
    
    listener1->onTouchEnded = [](Touch* touch, Event* event) {
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, book);
    
    addChild(book, 1);
    
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
    textBox->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
    
    addChild(textBox, 0);
    
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
    
    ActionInterval* wv = Waves3D::create(3, Size(40, 20), 25, 2);
    auto nd = NodeGrid::create();
    nd->runAction(RepeatForever::create(wv));
    book->addChild(nd, 10);
    
    //auto ep = EndPopup::create(100, 2, false, std::vector<std::string> {});
    //addChild(ep, 2000);
    
    /*auto spr = Sprite::createWithSpriteFrameName("flow2/flow2_K.psd");
    spr->setRotation(-90);
    spr->setScale(0.15);
    spr->setPosition(Vec2(320, 180));
    spr->setBlendFunc(BlendFunc::ADDITIVE);
    nd->addChild(spr, 0);*/
    
    return true;
    
}

void LevelScene::dropDepth() {
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
    
    score += (5 * popped + poppedLetters) * boundToRange(10, 100 * int(pow(float(popped)/float(maxBubbles), 2)), INT32_MAX);
    
    scoreLabel->setString(std::to_string(score));
    
    textBox->touchDownAction(NULL, cocos2d::ui::Widget::TouchEventType::ENDED);
    if (currentDepth > maxDepth) {
        auto next = true;
        for (auto b : bubbles) {
            if (!b->ded) {
                next = false;
                break;
            }
        }
        if (next) {
            //return
            auto scene = LevelScene::createScene(GameManager::getInstance()->nextLevel());
            Director::getInstance()->replaceScene(TransitionFlipX::create(2, scene));
        }
        return;
    };
    for (auto lane = 0; lane < data[currentDepth-1].size(); lane++) {
        auto datum = data[currentDepth-1][lane];
        if (datum.second.size() > 0) {
            auto bbl = Bubble::create(datum.second, lane, 0, this, datum.first);
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
        
        auto scaleTo = ScaleTo::create(0.2, 0.4);
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
        b->currentWord(d);
    }
    
    lastLen = strlen;
    lastText = txt;
}

void LevelScene::editBoxReturn(cocos2d::ui::EditBox* editBox) {
    
    auto gm = GameManager::getInstance();
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
        auto x = b->currentWord(text);
        for (auto q : x) {
            vec.push_back(q);
        }
        if (!b->ded) {
            maxBubbles++;
            maxLetters += b->letters.size();
        }
    }
    
    textBox->setText("");
    auto create = false;
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
            if (b->letters.size() == 0 and !b->ded) {
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

