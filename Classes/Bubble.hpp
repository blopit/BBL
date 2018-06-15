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
    float radius = 75;
    char letter;
    cocos2d::Vec2 anchorPosition;
    void setHighlight(bool hl);
    bool getHighlight();
    static Letter* create(char c, int idx, Bubble *owner);
    void enactForce(Letter *source);
    virtual bool init() override;
    virtual void update(float dt) override;
    CC_SYNTHESIZE(cocos2d::Sprite *, hlSprite, HlSprite);
    CC_SYNTHESIZE(Bubble *, owner, Owner);
    void pop();
};

enum class BubbleType {
    ORDERED,
    UNORDERED,
    BOMB
};

class Bubble : public cocos2d::Sprite {
    BubbleType bubbleType;
    int tick, uptick, lane, depth, td;
    int combo = 0;
    float rand;
    Bubble(std::vector<char> chars, int lane, int depth, LevelScene *owner, BubbleType bubbleType, int td) : tick(int(random()) % 360), uptick(int(random()) % 360), lane(lane), depth(depth), owner(owner), bubbleType(bubbleType), rand(90 * (int(random()) % 4)), td(td) {
        auto i = 0;
        for (auto x : chars) {
            letters.push_back(Letter::create(x, i, this));
            i++;
        }
    }
public:
    bool ded = false;
    std::vector<std::pair<Letter *, int>> currentWord(std::string word);
    std::vector<std::pair<Letter *, int>> currentWordUnordered(std::string word);
    std::vector<std::pair<Letter *, int>> currentWordOrdered(std::string word);
    std::vector<Letter *> letters;
    static Bubble* create(std::vector<char> letters, int lane, int depth, LevelScene *owner, BubbleType bubbleType, int td);
    virtual bool init() override;
    virtual void update(float dt) override;
    void popLetter(Letter *letter);
    void recalculate();
    void setAnchor(Letter *letter, int len, int idx);
    void pop(cocos2d::CallFunc* cf);
    CC_SYNTHESIZE(LevelScene *, owner, Owner);
    CC_SYNTHESIZE(cocos2d::ParticleSystemQuad *, psemitter, Psemitter);
};

#endif /* Bubble_hpp */
