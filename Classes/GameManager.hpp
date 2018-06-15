//
//  GameManager.hpp
//  BBL
//
//  Created by Shrenil Patel on 2017-11-15.
//

#ifndef GameManager_hpp
#define GameManager_hpp

#include "cocos2d.h"
#include "PluginAdMob/PluginAdMob.h"

class AdListener : public sdkbox::AdMobListener
{
private:
    void adViewDidReceiveAd(const std::string &name) {}
    void adViewDidFailToReceiveAdWithError(const std::string &name, const std::string &msg) {}
    void adViewWillPresentScreen(const std::string &name) {}
    void adViewDidDismissScreen(const std::string &name);
    void adViewWillDismissScreen(const std::string &name) {}
    void adViewWillLeaveApplication(const std::string &name) {}
};

class Level {

public:
    bool goldStars = false;
    int stars = 0;
    bool locked = true;
    int highscore = 0;
    int index;
    std::string fname;
    std::vector<std::string> hiddenWords;
    Level(std::string fname, int index) : fname(fname), index(index) {}
};

class GameManager
{
private:
    int levelIndex = 0;
    int coins = 0;
    std::vector<std::string> files;
    GameManager();
    ~GameManager();
    
public:
    std::vector<std::string> hiddenWords;
    std::string currentLevel = "";
    std::vector<std::string> words;
    std::vector<Level *> levels;
    void completeLevel(std::string level, int stars, std::vector<std::string> hiddenWords, int score);
    static GameManager* getInstance();
    std::string nextLevel(std::string current);
    
    void restart();
    void next();
    int level();
};

#endif /* GameManager_hpp */
