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
#include "PluginSdkboxPlay/PluginSdkboxPlay.h"

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

class GameManager: public sdkbox::SdkboxPlayListener
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
    std::vector<std::string> smwords;
    std::vector<Level *> levels;
    void completeLevel(std::string level, int stars, std::vector<std::string> hiddenWords, int score);
    static GameManager* getInstance();
    std::string nextLevel(std::string current);
    
    void restart();
    void next();
    int level();
    
    virtual void onConnectionStatusChanged(int connection_status) {
        if (sdkbox::GPS_CONNECTED) {
            /*std::string sData("1bC\0u\4;Y\5L", 10);
            const void* data = (const void*)sData.c_str();*/
            std::map<std::string, int> m = {{"level", 8}, {"coins", 9087}};
            int a[] = {9, 9087};
            const void* data = &a;
            int len = (int)(sizeof(a));
            sdkbox::PluginSdkboxPlay::saveGameDataBinary("save", data, len);
            
            sdkbox::PluginSdkboxPlay::loadAllGameData();
        }
        
    }
    virtual void onScoreSubmitted( const std::string& leaderboard_name, long score, bool maxScoreAllTime, bool maxScoreWeek, bool maxScoreToday ) {}
    virtual void onIncrementalAchievementUnlocked( const std::string& achievement_name ) {}
    virtual void onIncrementalAchievementStep( const std::string& achievement_name, double step ) {}
    virtual void onAchievementUnlocked( const std::string& achievement_name, bool newlyUnlocked ) {}
    
    virtual void onGameData(const std::string& action, const std::string& name, const std::string& data, const std::string& error) {}
    virtual void onSaveGameData(bool success,
                                const std::string& error) {
        
    }
    virtual void onLoadGameData(const sdkbox::SavedGameData* savedData,
                                const std::string& error) {
        int i,j;
        if (savedData) {
            if (savedData->name == "save"){
                auto d = (int *)savedData->data;
                i = *(d);
                j = *(d + 1);
            }
        }
    }
    virtual void onGameDataNames(const std::vector<std::string>& names,
                                 const std::string& error) {}
};

#endif /* GameManager_hpp */
