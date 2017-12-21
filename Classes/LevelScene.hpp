//
//  LevelScene.hpp
//  BBL
//
//  Created by Shrenil Patel on 2017-11-09.
//

#ifndef LevelScene_hpp
#define LevelScene_hpp

#include "cocos2d.h"
#include "Bubble.hpp"
#include "Util.hpp"

#include <ui/CocosGUI.h>

#define EDIT_DEPTH 488


class LevelScene;
class Letter;
class Bubble;
enum class BubbleType;

class EndPopup : public cocos2d::Layer {
    int score;
    int dscore = 0;
    int stars;
    bool gold;
    bool death;
    std::vector<std::string> secretWords;
    EndPopup(int score, int stars, bool gold, std::vector<std::string> secretWords, bool death): score(score), stars(stars), gold(gold), secretWords(secretWords), death(death) {}
public:
    static EndPopup* create(int score, int stars, bool gold, std::vector<std::string> secretWords, bool death);
    virtual bool init() override;
    virtual void update(float dt) override;
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) override;
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
    
    void restart(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
    void next(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
    
    CC_SYNTHESIZE(cocos2d::Label *, scoreLabel, ScoreLabel);
};

class Light : public cocos2d::Sprite {
    cocos2d::Vec2 start;
    std::vector<Letter *> letters;
    std::vector<std::pair<cocos2d::Vec2, float>> trail;
    Light(std::vector<Letter *> letters, cocos2d::Vec2 start, LevelScene * levelScene) : letters(letters), start(start), levelScene(levelScene) {}
public:
    static Light* create(std::vector<Letter *> letters, cocos2d::Vec2 start, LevelScene * levelScene);
    virtual bool init() override;
    virtual void update(float dt) override;
    CC_SYNTHESIZE(cocos2d::DrawNode *, drawNode, DrawNode);
    CC_SYNTHESIZE(cocos2d::Layer *, layer, Layer);
    CC_SYNTHESIZE(LevelScene *, levelScene, LevelScene);
};

class Card : public cocos2d::Sprite {
    Card(char letter) : letter(letter) {}
public:
    int idx;
    bool ded = false;
    cocos2d::Vec2 dest;
    char letter;
    static Card* create(char c);
    virtual bool init() override;
    virtual void update(float dt) override;
};

class LevelScene : public cocos2d::Scene, public cocos2d::ui::EditBoxDelegate
{
    std::string fname;
    int lastLen = 0;
    int bubbleRetain = 0;
    int cardsLen = 0;
    int currentDepth = 0;
    int maxDepth = 0;
    LevelScene(std::string fname) : fname(fname) {}
    std::vector<Card *> cards;
    std::vector<Bubble *> bubbles;
    std::vector<std::vector<std::pair<BubbleType, std::vector<char>>>> data;
public:
    int score = 0;
    int targetScore = 0;
    int highlights = 0;
    int lightRetain = 0;
    int lights = 0;
    int maxBubbles = 0;
    int maxLetters = 0;
    static cocos2d::Scene* createScene(std::string fname);
    virtual bool init();
    static LevelScene* create(std::string fname);
    std::string lastText = "";
    
    virtual void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox);
    virtual void editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox);
    virtual void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text);
    virtual void editBoxReturn(cocos2d::ui::EditBox* editBox);
    
    void dropDepth();
    void addLightRetain();
    void removeLightRetain();
    void addBubbleRetain();
    void removeBubbleRetain();
    
    CC_SYNTHESIZE(cocos2d::Label *, editLabel, EditLabel);
    CC_SYNTHESIZE(cocos2d::Label *, scoreLabel, ScoreLabel);
    CC_SYNTHESIZE(cocos2d::ui::EditBox *, textBox, TextBox);
    CC_SYNTHESIZE(cocos2d::NodeGrid *, nodeGrid, NodeGrid);
    CC_SYNTHESIZE(cocos2d::Sprite *, book, Book);
    CC_SYNTHESIZE(cocos2d::LayerColor *, layer, Layer);
    CC_SYNTHESIZE(cocos2d::ParticleSystemQuad *, psemitter1, Psemitter1);
    CC_SYNTHESIZE(cocos2d::ParticleSystemQuad *, psemitter2, Psemitter2);
    
    
    /*CC_SYNTHESIZE(cocos2d::Action *, baMove, baMove);
    CC_SYNTHESIZE(cocos2d::Action *, baRotate, baRotate);
    CC_SYNTHESIZE(cocos2d::Action *, baSkew, baSkew);*/
};

#endif /* LevelScene_hpp */
