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
    ss << "evil/" << cstr << ".psd";
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
    auto dist = dest.distance(curr);
    auto ang = atan2(dest.y - curr.y, dest.x - curr.x);
    hsp += cos(ang) * weight;
    vsp += sin(ang) * weight;
}

bool Letter::init() {
    stringstream ss;
    string cstr(1, toupper(letter));
    ss << "highlight/" << cstr << ".psd";
    auto str = ss.str();
    hlSprite = Sprite::createWithSpriteFrameName(str);
    hlSprite->setPosition(getBoundingBox().size/2);
    addChild(hlSprite, 100);
    hlSprite->setScale(1.25);
    hlSprite->setScale(0);
    
    setScale(0);
    auto delay = DelayTime::create(0.3*(idx+1));
    auto scaleTo = EaseElasticOut::create(ScaleTo::create(1, 0.75));
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
    
    if (tick % 181 == 0) {
        //setHighlight(!highlight);
        /*auto emitter = ParticleSystemQuad::create("res/BubbleSm.plist");
        addChild(emitter);
        emitter->setAutoRemoveOnFinish(true);
        emitter->setPosition(getBoundingBox().size/2);*/
    }
    
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
        radius = 70;
        weight = 0.1;
        auto scaleTo = EaseElasticOut::create(ScaleTo::create(0.2, 1.25));
        hlSprite->runAction(scaleTo);
    } else {
        radius = 40;
        weight = 0.2;
        auto scaleTo = EaseBackIn::create(ScaleTo::create(0.1, 0));
        hlSprite->runAction(scaleTo);
    }
}

Bubble* Bubble::create(vector<char> letters, int lane, int depth, LevelScene *owner)
{
    Bubble* pSprite = new Bubble(letters, lane, depth, owner);
    
    if (pSprite->initWithSpriteFrameName("bubbles/blue.png"))
    {
        pSprite->autorelease();
        pSprite->init();
        return pSprite;
    }
    CC_SAFE_DELETE(pSprite);
    return NULL;
}

bool Bubble::init() {
    setScale(0);
    
    runAction(EaseElasticOut::create(ScaleTo::create(3, 0.5)));
    playSound("stretch", false, 1.0f, 0.2f, 0.5, 0.05f);
    playSound("inflate", false, 1.0f, 0.2f, 0.5, 0.05f);
    
    auto len = letters.size();
    auto i = 0;
    
    for (auto x: letters) {
        addChild(x, -(i+1));
        x->idx = i;
        x->anchorPosition = Vec2(190 + getX(int(len), i), 190);
        x->setPosition(x->anchorPosition);
        i+= 1;
    }
    
    setPosition(136 + 184 * lane, 976 - 184 * depth);
    
    this->scheduleUpdate();
    return true;
}

void Bubble::update(float dt) {
    tick = tick + 2 % 360;
    uptick = uptick + 2 % 360;
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

std::vector<std::pair<Letter *, int>> Bubble::currentWord(std::string word) {
    std::vector<std::pair<Letter *, int>> ret;
    auto crush = false;
    auto i = 0;
    for (auto x : letters) {
        if (crush) {
            x->setHighlight(false);
            continue;
        }
        auto done = false;
        for (;i < word.size(); i++) {
            auto c = word[i];
            if (toupper(x->letter) != c) {
                continue;
            } else {
                x->setHighlight(true);
                ret.push_back(make_pair(x, i));
                done = true;
                i++;
                break;
            }
        }
        if (!done) {
            x->setHighlight(false);
            crush = true;
        }
    }
    return ret;
}

void Bubble::popLetter(Letter * letter) {
    remove(letters, letter);
    
    auto emitter = ParticleSystemQuad::create("res/BubbleSm.plist");
    owner->addChild(emitter);
    emitter->setAutoRemoveOnFinish(true);
    emitter->setPosition(convertToWorldSpace(letter->getPosition()));
    
    auto sk1 = EaseBackOut::create(ScaleTo::create(0.1, randFloat(0.55, 0.75)));
    auto sk2 = EaseElasticOut::create(ScaleTo::create(2, 0.5));
    auto seq = Sequence::create(sk1, sk2, NULL);
    
    playSound("burst2", false, randFloat(0.5, 2), 0.25f);
    
    runAction(seq);
    
    letter->pop();
    
}

void Bubble::recalculate() {
    auto moveTo = EaseElasticOut::create(MoveTo::create(3, Vec2(136 + 184 * lane, 976 - 184 * (depth + 1))));
    
    auto scaleTo = ScaleTo::create(0.1, randFloat(0.55, 0.75), randFloat(0.25, 0.45));
    auto scaleTo2 = EaseElasticOut::create(ScaleTo::create(3, 0.5, 0.5));
    auto callback = CallFunc::create([=](){
        if (letters.size() > 0) {
            auto emitter = ParticleSystemQuad::create("res/BubbleSimp.plist");
            owner->addChild(emitter);
            emitter->setAutoRemoveOnFinish(true);
            emitter->setPosition(getPosition());
            playSound("bubbling", false, randFloat(0.5, 2), 0.2, 0.5, 0.05);
        }
    });
    
    auto seq = Sequence::create(scaleTo, callback, scaleTo2, NULL);
    
    auto len = letters.size();
    auto i = 0;
    for (auto x: letters) {
        x->idx = i;
        x->anchorPosition = Vec2(190 + getX(int(len), i), 190);
        x->setPosition(x->anchorPosition);
        i+= 1;
    }
    
    if (letters.size() == 0) {
        if (!ded) pop();
    } else {
        depth += 1;
        runAction(moveTo);
        runAction(seq);
    }
}

void Bubble::pop() {
    auto callback = CallFunc::create([=](){
        ded = true;
        auto emitter = ParticleSystemQuad::create("res/BubbleLg.plist");
        owner->addChild(emitter);
        emitter->setAutoRemoveOnFinish(true);
        emitter->setPosition(getPosition());
        playSound("pop2", false, randFloat(0.5, 2));
        playSound("bubbling2", false, randFloat(0.5, 2), 0.5, 0.5, 0.1);
    });
    auto action0 = ScaleTo::create(0.05f, 0.80f);
    auto action1 = ScaleTo::create(0.05f, 0.75f);
    auto seq = Sequence::create(action0, action1, NULL);
    auto action = Repeat::create(seq, 2);
    
    runAction(Sequence::create(EaseOut::create(ScaleTo::create(0.25, 0.75), 1.5), action, NULL));
    runAction(Sequence::create(FadeTo::create(0.45, 192), callback, FadeTo::create(0, 0), NULL));
    playSound("stretch", false, randFloat(0.5, 2), 0.05f);
    
    
    runAction(action);
    
}
