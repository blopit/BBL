//
//  GameManager.hpp
//  BBL
//
//  Created by Shrenil Patel on 2017-11-15.
//

#ifndef GameManager_hpp
#define GameManager_hpp

#include "cocos2d.h"

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
    std::vector<std::string> files;
    GameManager();
    ~GameManager();
    
public:
    std::string currentLevel = "";
    std::vector<std::string> words;
    std::vector<Level *> levels;
    void completeLevel(std::string level, int stars, std::vector<std::string> hiddenWords, int score);
    static GameManager* getInstance();
    std::string nextLevel(std::string current);
    
    void restart();
    void next();
};

#endif /* GameManager_hpp */
