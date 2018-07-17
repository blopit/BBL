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
#include <stdlib.h>
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
    bool pressed = false;
    std::vector<std::string> secretWords;
    EndPopup(int score, int stars, bool gold, std::vector<std::string> secretWords, bool death, LevelScene *owner): score(score), stars(stars), gold(gold), secretWords(secretWords), death(death), owner(owner) {}
public:
    static EndPopup* create(int score, int stars, bool gold, std::vector<std::string> secretWords, bool death, LevelScene *owner);
    virtual bool init() override;
    virtual void update(float dt) override;
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) override;
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
    
    void restart(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
    void next(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
    
    CC_SYNTHESIZE(cocos2d::Label *, scoreLabel, ScoreLabel);
    CC_SYNTHESIZE(LevelScene *, owner, Owner);
};

class MessagePopup : public cocos2d::Layer {
    std::string text;
    MessagePopup(std::string text, LevelScene *owner): text(text), owner(owner) {}
    void dismiss(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
public:
    static MessagePopup* create(std::string text, LevelScene *owner);
    virtual bool init() override;
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) override;
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
    
    CC_SYNTHESIZE(cocos2d::Label *, scoreLabel, ScoreLabel);
    CC_SYNTHESIZE(cocos2d::Sprite *, panel, Panel);
    CC_SYNTHESIZE(LevelScene *, owner, Owner);
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

class Coin : public cocos2d::Sprite {
    cocos2d::Vec2 start;
    cocos2d::Vec2 end;
    //std::vector<std::pair<cocos2d::Vec2, float>> trail;
    Coin(cocos2d::Vec2 start, cocos2d::Vec2 end, LevelScene * levelScene) : start(start), end(end), levelScene(levelScene) {}
public:
    static Coin* create(cocos2d::Vec2 start, cocos2d::Vec2 end, LevelScene * levelScene);
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
    LevelScene(std::string fname) : fname(fname) {}
    std::vector<Card *> cards;
    std::vector<std::string> hiddenWords;
    std::vector<std::string> words;
    std::vector<std::vector<std::pair<BubbleType, std::vector<char>>>> data;
    
    float skytime = 0;
    float bkpos = 0;
    
    int depthRetainCount = 0;
    int hue1 = 0;
    int hue2 = 0;
    int cmbo = 0;
    int coinbkshift = 0;
    int scorewid = 128;
    int csw = 0;
    int startlevel = 0;
    
public:
    std::vector<Bubble *> bubbles;
    float li1 = 0;
    float coinLi = 0;
    int difficulty = 1;
    std::string levelName = "";
    bool redZone = false;
    bool timed = false;
    
    int score = 0;
    int coins;
    int targetScore = 0;
    int highlights = 0;
    int lightRetain = 0;
    int lights = 0;
    int maxBubbles = 0;
    int maxLetters = 0;
    int maxDepth = 0;
    bool end = false;
    bool ending = false;
    static cocos2d::Scene* createScene(std::string fname);
    virtual bool init() override;
    virtual void update(float dt) override;
    static LevelScene* create(std::string fname);
    std::string lastText = "";
    
    virtual void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox) override;
    virtual void editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox) override;
    virtual void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text) override;
    virtual void editBoxReturn(cocos2d::ui::EditBox* editBox) override;
    
    void dropDepth();
    void addLightRetain();
    void removeLightRetain();
    void addBubbleRetain();
    void removeBubbleRetain();
    void lost();
    void combo();
    void resetcombo();
    void getHint();
    
    CC_SYNTHESIZE(cocos2d::Label *, editLabel, EditLabel);
    CC_SYNTHESIZE(cocos2d::Label *, scoreLabel, ScoreLabel);
    CC_SYNTHESIZE(cocos2d::Label *, coinLabel, CoinLabel);
    CC_SYNTHESIZE(cocos2d::Label *, helpLabel, HelpLabel);
    CC_SYNTHESIZE(cocos2d::Label *, hintLabel, HintLabel);
    CC_SYNTHESIZE(cocos2d::ui::EditBox *, textBox, TextBox);
    CC_SYNTHESIZE(cocos2d::NodeGrid *, nodeGrid, NodeGrid);
    CC_SYNTHESIZE(cocos2d::Sprite *, book, Book);
    CC_SYNTHESIZE(cocos2d::Sprite *, diag, Diag);
    CC_SYNTHESIZE(cocos2d::Sprite *, cmb, Cmb);
    CC_SYNTHESIZE(cocos2d::Sprite *, coinAm, CoinAm);
    CC_SYNTHESIZE(cocos2d::Sprite *, coinAmCoin, CoinAmCoin);
    CC_SYNTHESIZE(cocos2d::Sprite *, helpAmCoin, HelpAmCoin);
    CC_SYNTHESIZE(cocos2d::Sprite *, helpAmCoin2, HelpAmCoin2);
    CC_SYNTHESIZE(cocos2d::Sprite *, help, Help);
    CC_SYNTHESIZE(cocos2d::LayerColor *, layer, Layer);
    CC_SYNTHESIZE(cocos2d::LayerGradient *, gradbk, Gradbk);
    CC_SYNTHESIZE(cocos2d::ParticleSystemQuad *, psemitter1, Psemitter1);
    CC_SYNTHESIZE(cocos2d::ParticleSystemQuad *, psemitter2, Psemitter2);
    CC_SYNTHESIZE(cocos2d::ParticleSystemQuad *, psemitter3, Psemitter3);
    CC_SYNTHESIZE(cocos2d::ParticleSystemQuad *, scoreEmit, ScoreEmit);
    CC_SYNTHESIZE(cocos2d::ParticleSystemQuad *, scoreEmit2, ScoreEmit2);
    
    
    /*CC_SYNTHESIZE(cocos2d::Action *, baMove, baMove);
    CC_SYNTHESIZE(cocos2d::Action *, baRotate, baRotate);
    CC_SYNTHESIZE(cocos2d::Action *, baSkew, baSkew);*/
};

#endif /* LevelScene_hpp */
