//
//  Bubble.hpp
//  BBL
//
//  Created by Shrenil Patel on 2017-11-09.
//

#ifndef Bubble_hpp
#define Bubble_hpp

#include "cocos2d.h"
#include "LevelScene.hpp"

class Bubble;
class LevelScene;

class Letter : public cocos2d::Sprite {
    int tick;
    float bouy = 0.0f;
    float hsp, vsp;
    bool highlight = false;
    Letter(char letter, int idx, Bubble *owner) : letter(letter), tick(int(random()) % 360), anchorPosition(cocos2d::Vec2(0, 0)), idx(idx), hsp(0), vsp(0), owner(owner) {}
public:
    int idx;
    float weight = 0.2;
    float radius = 50;
    char letter;
    cocos2d::Vec2 anchorPosition;
    void setHighlight(bool hl);
    static Letter* create(char c, int idx, Bubble *owner);
    void enactForce(Letter *source);
    virtual bool init() override;
    virtual void update(float dt) override;
    CC_SYNTHESIZE(cocos2d::Sprite *, hlSprite, HlSprite);
    CC_SYNTHESIZE(Bubble *, owner, Owner);
    void pop();
};


class Bubble : public cocos2d::Sprite {
    bool ded = false;
    int tick, uptick, lane, depth;
    Bubble(std::vector<char> chars, int lane, int depth, LevelScene *owner) : tick(int(random()) % 360), uptick(int(random()) % 360), lane(lane), depth(depth), owner(owner) {
        auto i = 0;
        for (auto x : chars) {
            letters.push_back(Letter::create(x, i, this));
            i++;
        }
    }
public:
    std::vector<std::pair<Letter *, int>> currentWord(std::string word);
    std::vector<Letter *> letters;
    static Bubble* create(std::vector<char> letters, int lane, int depth, LevelScene *owner);
    virtual bool init() override;
    virtual void update(float dt) override;
    void popLetter(Letter * letter);
    void recalculate();
    void pop();
    CC_SYNTHESIZE(LevelScene *, owner, Owner);
};

#endif /* Bubble_hpp */
