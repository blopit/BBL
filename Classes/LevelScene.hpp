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

#define BZTO_TIME 0.2

class LevelScene;
class Letter;
class Bubble;

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
    LevelScene(std::string fname) : fname(fname) {}
    std::vector<Card *> cards;
    std::vector<Bubble *> bubbles;
public:
    int highlights = 0;
    int lightRetain = 0;
    static cocos2d::Scene* createScene(std::string fname);
    virtual bool init();
    static LevelScene* create(std::string fname);
    
    virtual void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox);
    virtual void editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox);
    virtual void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text);
    virtual void editBoxReturn(cocos2d::ui::EditBox* editBox);
    
    void addLightRetain();
    void removeLightRetain();
    void addBubbleRetain();
    void removeBubbleRetain();
    
    CC_SYNTHESIZE(cocos2d::Label *, editLabel, EditLabel);
    CC_SYNTHESIZE(cocos2d::ui::EditBox *, textBox, TextBox);
    
};

#endif /* LevelScene_hpp */
