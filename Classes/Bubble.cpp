//
//  Bubble.cpp
//  BBL
//
//  Created by Shrenil Patel on 2017-11-09.
//

#include "Bubble.hpp"
#include "Util.hpp"
#include <sstream>
USING_NS_CC;
using namespace std;

Letter* Letter::create(char c, int idx, Bubble *owner) {
    Letter* pSprite = new Letter(c, idx, owner);
    stringstream ss;
    string cstr(1, toupper(c));
    
    if (isnumber(c)) {
        ss << "goldnum/" << cstr << ".psd";
    } else {
        ss << "evil/" << cstr << ".psd";
    }
    
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

void Letter::enactForce(Letter *source) {
    auto dest = getPosition();
    auto curr = source->getPosition();
    auto ang = atan2(dest.y - curr.y, dest.x - curr.x);
    hsp += cos(ang) * weight;
    vsp += sin(ang) * weight;
}

bool Letter::init() {
    
    if (!isalnum(letter)) {
        letter = '?';
    } else {
        letter = toupper(letter);
    }
    
    stringstream ss;
    string cstr(1, toupper(letter));
    
    if (isnumber(letter)) {
        ss << "highlightnum/" << cstr << ".psd";
    } else {
        ss << "highlight/" << cstr << ".psd";
    }
    
    auto str = ss.str();
    hlSprite = Sprite::createWithSpriteFrameName(str);
    hlSprite->setPosition(getBoundingBox().size/2);
    addChild(hlSprite, 100);
    hlSprite->setScale(1.25);
    hlSprite->setScale(0);
    
    setScale(0);
    auto delay = DelayTime::create(0.3*(idx+1));
    auto scaleTo = EaseElasticOut::create(ScaleTo::create(1, 0.8));
    auto sound = CallFunc::create([=](){
        playSound("inflate", false, 1.0f, 0.5f, 0.5, 0.05f);
    });
    
    auto seq = Sequence::create(delay, sound, scaleTo, NULL);
    
    runAction(seq);
    
    setHighlight(false);
    
    this->scheduleUpdate();
    
    /*auto drawNode = DrawNode::create();
    addChild(drawNode, 100);
    drawNode->setPosition(getBoundingBox().size/2/0.75);
    drawNode->drawCircle(Vec2(0, 0), 40/0.75, 0, 8, false, Color4F::MAGENTA);
    drawNode->drawCircle(Vec2(0, 0), 70/0.75, 0, 8, false, Color4F::MAGENTA);
    */
    
    return true;
}

void Letter::update(float dt) {
    auto targ = 1;
    if (highlight) {
        targ = 0;
    }
    bouy += (targ - bouy) * 0.1;
    
    auto dest = anchorPosition;
    dest.y += 10 * bouy * cos(MATH_DEG_TO_RAD(tick));
    auto curr = getPosition();
    
    auto dist = dest.distance(curr);
    auto ang = atan2(dest.y - curr.y, dest.x - curr.x);
    
    hsp += cos(ang) * 0.1 * dist/5;
    vsp += sin(ang) * 0.1 * dist/5;
    
    auto fric = 0.01;
    auto spd = Vec2(hsp, vsp).distance(Vec2(0, 0));
    auto aspd = atan2(vsp, hsp);
    auto fspd = spd - fric > 0 ? spd - fric : 0;
    hsp = fspd * cos(aspd);
    vsp = fspd * sin(aspd);
    
    tick = tick + 1 % 360;
    setRotation(3 * sin(MATH_DEG_TO_RAD(tick)));
    hlSprite->setRotation(3 * sin(MATH_DEG_TO_RAD(tick)));
    
    setPosition(curr + Vec2(hsp, vsp));
    
    /*if (tick % 181 == 0) {
        //setHighlight(!highlight);
        auto emitter = ParticleSystemQuad::create("res/BubbleSm.plist");
        addChild(emitter);
        emitter->setAutoRemoveOnFinish(true);
        emitter->setPosition(getBoundingBox().size/2);
    }*/
    
}

void Letter::pop() {
    auto scaleTo = EaseBackIn::create(ScaleTo::create(0.1, 0));
    auto rmSelf = RemoveSelf::create();
    auto seq = Sequence::create(scaleTo, rmSelf, NULL);
    runAction(seq);
}

void Letter::setHighlight(bool hl) {
    if (hl != highlight) {
        if (hl) {
            owner->getOwner()->highlights++;
            playSound("inflate", false, 2.0f + float(owner->getOwner()->highlights)/5.0f, 0.2f, 0.5f, 0.5f);
        } else {
            owner->getOwner()->highlights--;
            playSound("deflate", false, 1.0f + float(owner->getOwner()->highlights)/5.0f, 0.2f, 0.5f, 0.5f);
        }
    }
    
    highlight = hl;
    //hlSprite->setVisible(hl);
    
    if (highlight) {
        radius = 105;
        weight = 0.1;
        auto scaleTo = EaseElasticOut::create(ScaleTo::create(0.2, 1.25));
        hlSprite->runAction(scaleTo);
    } else {
        radius = 60;
        weight = 0.2;
        auto scaleTo = EaseBackIn::create(ScaleTo::create(0.1, 0));
        hlSprite->runAction(scaleTo);
    }
}

bool Letter::getHighlight() {
    return highlight;
}

Bubble* Bubble::create(vector<char> letters, int lane, int depth, LevelScene *owner, BubbleType bubbleType, int td)
{
    Bubble* pSprite = new Bubble(letters, lane, depth, owner, bubbleType, td);
    
    
    
    auto bubbleFormat = "";
    
    if (bubbleType == BubbleType::UNORDERED) {
        bubbleFormat = "bubbles/reg.png";
    } else if (bubbleType == BubbleType::ORDERED) {
        bubbleFormat = "bubbles/square.png";
    } else if (bubbleType == BubbleType::BOMB) {
        bubbleFormat = "bubbles/bomb.png";
    }
    
    if (pSprite->initWithSpriteFrameName(bubbleFormat))
    {
        pSprite->autorelease();
        pSprite->init();
        return pSprite;
    }
    CC_SAFE_DELETE(pSprite);
    return NULL;
}

void Bubble::setAnchor(Letter *letter, int len, int idx) {
    /*switch (bubbleType) {
        case BubbleType::ORDERED:*/
    
    if (bubbleType == BubbleType::BOMB) {
        letter->anchorPosition = Vec2(284 + getX(int(len), idx), 256);
    } else {
        letter->anchorPosition = Vec2(256 + getX(int(len), idx), 256);
    }
    
            /*break;
        case BubbleType::UNORDERED:
            auto ang = rand + idx * 360.0f/len;
            auto dis = len == 1? 0 : letter->getHighlight() ? 96 : 80;
            auto x = 190 + dis * cos(MATH_DEG_TO_RAD(ang));
            auto y = 190 + dis * sin(MATH_DEG_TO_RAD(ang));
            letter->anchorPosition = Vec2(x, y);
            break;
    }*/
}

bool Bubble::init() {
    setScale(0);
    
    if (bubbleType == BubbleType::BOMB) {
        setAnchorPoint(Vec2(0.55, 0.5));
    }
    
    runAction(EaseElasticOut::create(ScaleTo::create(3, 0.37)));
    playSound("stretch", false, 1.0f, 0.2f, 0.5, 0.05f);
    playSound("inflate", false, 1.0f, 0.2f, 0.5, 0.05f);
    
    auto len = letters.size();
    auto i = 0;
    
    for (auto x: letters) {
        addChild(x, -(i+1));
        x->idx = i;
        setAnchor(x, int(len), i);
        x->setPosition(x->anchorPosition);
        i += 1;
    }
    
    psemitter = ParticleSystemQuad::create("res/Dazzle.plist");
    addChild(psemitter, -1);
    psemitter->setPosition(Vec2(256, 256));
    
    if (td < owner->maxDepth) {
        psemitter->stop();
    }
    
    setPosition(136 + 184 * lane, 1000 - 184 * depth);
    
    this->scheduleUpdate();
    return true;
}

void Bubble::update(float dt) {
    tick = tick + 2 % 360;
    uptick = uptick + 2 % 360;
    tick2 = (tick2 + 10) % 360;
    
    /*if (bubbleType == BubbleType::UNORDERED) {
        rand += 0.05;
        if (rand >= 360) {
            rand -= 360;
        }
        auto i = 0;
        auto len = letters.size();
        for (auto x: letters) {
            setAnchor(x, int(len), i);
            i += 1;
        }
    }*/
    
    if (bubbleType == BubbleType::BOMB) {
        setColor(Color3B(255, 223+32*sin(MATH_DEG_TO_RAD(tick2)), 223+32*sin(MATH_DEG_TO_RAD(tick2))));
    }
    
    setSkewX(1*sin(MATH_DEG_TO_RAD(tick)));
    setSkewY(1*sin(MATH_DEG_TO_RAD(uptick)));
    
    for (auto x : letters) {
        for (auto y : letters) {
            if (x->idx != y->idx and (y->getPosition()).distance(x->getPosition()) < (y->radius + x->radius)) {
                x->enactForce(y);
            }
        }
    }
    
}

std::vector<std::pair<Letter *, int>> Bubble::currentWord(std::string word, bool simulate) {
    switch (bubbleType) {
        case BubbleType::UNORDERED:
            return currentWordUnordered(word, simulate);
            break;
        case BubbleType::ORDERED:
            return currentWordOrdered(word, simulate);
            break;
        case BubbleType::BOMB:
            return currentWordUnordered(word, simulate);
            break;
        default:
            break;
    }
}

std::vector<std::pair<Letter *, int>> Bubble::currentWordUnordered(std::string word, bool simulate) {
    std::vector<std::pair<Letter *, int>> ret;
    std::vector<int> used;
    for (auto x : letters) {
        auto done = false;
        auto letr = x->letter;
        
        if (isnumber(x->letter)) {
            if (atoi(&letr) == word.size()) {
                ret.push_back(make_pair(x, word.length()-1));
                done = true;
            }
        } else {
            for (int i = 0; i < word.size(); i++) {
                auto c = word[i];
                if (toupper(letr) == toupper(c) and std::find(used.begin(), used.end(), i) == used.end()) {
                    done = true;
                    used.push_back(i);
                    ret.push_back(make_pair(x, i));
                    break;
                }
            }
        }
        if (!simulate) x->setHighlight(done);
    }
    return ret;
}

std::vector<std::pair<Letter *, int>> Bubble::currentWordOrdered(std::string word, bool simulate) {
    std::vector<std::pair<Letter *, int>> ret;
    auto crush = false;
    auto i = 0;
    auto ii = 0;
    auto id = 0;
    for (auto x : letters) {
        
        auto done = false;
        
        if (ii == id and isnumber(x->letter) and atoi(&x->letter) == word.size()){
            if (!simulate) x->setHighlight(true);
            ret.push_back(make_pair(x, word.length()-1));
            done = true;
            crush = true;
            id++;
            continue;
        }
        
        if (crush) {
            if (!simulate) x->setHighlight(false);
            ii++;
            continue;
        }
        
        
        for (;i < word.size(); i++) {
            auto c = word[i];
            auto letr = x->letter;
            if (toupper(letr) == toupper(c)) {
                if (!simulate) x->setHighlight(true);
                ret.push_back(make_pair(x, i));
                done = true;
                id++;
                i++;
                break;
            } else if (isnumber(letr) and atoi(&letr) == word.size()){
                if (!simulate) x->setHighlight(true);
                ret.push_back(make_pair(x, i));
                done = true;
                id++;
                crush = true;
                break;
            } else {
                continue;
            }
        }
        
        if (!done) {
            if (!simulate) x->setHighlight(false);
            crush = true;
        }
        
        ii++;
    }
    return ret;
}

void Bubble::popLetter(Letter * letter) {
    owner->combo();
    remove(letters, letter);
    
    auto emitter = ParticleSystemQuad::create("res/BubbleSm.plist");
    owner->addChild(emitter, 9);
    emitter->setAutoRemoveOnFinish(true);
    emitter->setPosition(convertToWorldSpace(letter->getPosition()));
    
    auto sk1 = EaseBackOut::create(ScaleTo::create(0.1, randFloat(0.4, 0.55)));
    auto sk2 = EaseElasticOut::create(ScaleTo::create(2, 0.37));
    auto seq = Sequence::create(sk1, sk2, NULL);
    
    playSound("burst2", false, randFloat(0.5, 2), 0.5f, 0.5f, 0.5f);
    
    runAction(seq);
    
    letter->pop();
    if (bubbleType == BubbleType::BOMB and !safedelete) {
        auto cf = CallFunc::create([=](){
            owner->ending = true;
            auto null = CallFunc::create([=](){
                owner->lost();
            });
            pop(null);
        });
        auto emit = CallFunc::create([=](){
            auto emitter = ParticleSystemQuad::create("res/Fire.plist");
            owner->addChild(emitter, 90);
            emitter->setAutoRemoveOnFinish(true);
            emitter->setPosition(getPosition());
        });
        setColor(Color3B::WHITE);
        runAction(Sequence::create(emit, DelayTime::create(1), cf, NULL));
    }
    
}

void Bubble::recalculate() {
    if (ded) return;
    auto callback = CallFunc::create([=](){
        if (letters.size() > 0) {
            auto emitter = ParticleSystemQuad::create("res/BubbleSimp.plist");
            owner->addChild(emitter, 9);
            emitter->setAutoRemoveOnFinish(true);
            emitter->setPosition(getPosition());
            playSound("bubbling", false, randFloat(0.5, 2), 0.2, 0.5, 0.05);
        }
    });
    auto scaleTo = ScaleTo::create(0.1, randFloat(0.4, 0.6), randFloat(0.25, 0.45));
    auto scaleTo2 = EaseElasticOut::create(ScaleTo::create(3, 0.37));
    auto seq = Sequence::create(scaleTo, callback, scaleTo2, NULL);
    
    if (bubbleType == BubbleType::BOMB) {
        auto destroy = true;
        for (auto b : owner->bubbles) {
            if (b->bubbleType != BubbleType::BOMB and !b->ded and b->cdepth == cdepth) {
                destroy = false;
            }
        }
        if (destroy) {
            safedelete = true;
            auto null = CallFunc::create([=](){
            });
            pop(null);
            Vector<cocos2d::FiniteTimeAction *> actions;
            
            for (auto l : letters) {
                popLetter(l);
            }
        }
    }
    
    
    depth += 1;
    if (depth >= 3) {
        if (bubbleType == BubbleType::BOMB) {
            safedelete = true;
        } else {
            owner->ending = true;
        }
        auto cf = CallFunc::create([=](){
            if (bubbleType != BubbleType::BOMB) {
                auto null = CallFunc::create([=](){
                    owner->lost();
                });
                pop(null);
            } else {
                auto null = CallFunc::create([=](){
                });
                pop(null);
            }
        });
        auto emit = CallFunc::create([=](){
            auto emitter = ParticleSystemQuad::create("res/Fire.plist");
            owner->addChild(emitter, 9);
            emitter->setAutoRemoveOnFinish(true);
            emitter->setPosition(getPosition());
        });
        
        Vector<cocos2d::FiniteTimeAction *> actions;
        
        for (auto l : letters) {
            auto p = CallFunc::create([=](){
                popLetter(l);
            });
            actions.pushBack(Sequence::create(DelayTime::create(0.3), p, NULL));
        }
        
        if (bubbleType != BubbleType::BOMB) {
            runAction(Sequence::create(emit, DelayTime::create(1), Sequence::create(actions), cf, NULL));
        } else {
            runAction(Sequence::create(emit, Sequence::create(actions), cf, NULL));
        }
        return;
    }
    
    if (owner->ending) return;
    
    auto moveTo = EaseElasticOut::create(MoveTo::create(3, Vec2(136 + 184 * lane, 1000 - 184 * (depth))));
    
    auto len = letters.size();
    auto i = 0;
    for (auto x: letters) {
        x->idx = i;
        setAnchor(x, int(len), i);
        x->setPosition(x->anchorPosition);
        i+= 1;
    }
    
    runAction(moveTo);
    runAction(seq);
    
    /*if (letters.size() == 0) {
        if (!ded) pop();
    } else {
        depth += 1;
        runAction(moveTo);
        runAction(seq);
    }*/
}

void Bubble::pop(cocos2d::CallFunc* cf) {
    psemitter->stop();
    auto callback = CallFunc::create([=](){
        ded = true;
        auto emitter = ParticleSystemQuad::create("res/BubbleLg.plist");
        owner->addChild(emitter, 9);
        emitter->setAutoRemoveOnFinish(true);
        emitter->setPosition(getPosition());
        playSound("pop2", false, randFloat(0.5, 2));
        playSound("bubbling2", false, randFloat(0.5, 2), 0.5, 0.5, 0.1);
        stopActionByTag(1);
    });
    auto fl = randFloat(0.6, 0.7);
    auto action0 = ScaleTo::create(0.05f, fl + randFloat(0.04, 0.06));
    auto action1 = ScaleTo::create(0.05f, fl);
    auto seq = Sequence::create(action0, action1, NULL);
    auto action = Repeat::create(seq, 2);
    action->setTag(1);
    
    runAction(Sequence::create(EaseOut::create(ScaleTo::create(0.25, fl), 1.5), action, NULL));
    runAction(Sequence::create(FadeTo::create(0.45, 192), FadeTo::create(0, 0), callback, DelayTime::create(0.5f), cf, NULL));
    playSound("stretch", false, randFloat(0.5, 2), 0.05f);
    
    runAction(action);
    
}
