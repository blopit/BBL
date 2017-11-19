//
//  GameManager.hpp
//  BBL
//
//  Created by Shrenil Patel on 2017-11-15.
//

#ifndef GameManager_hpp
#define GameManager_hpp

#include "cocos2d.h"

class GameManager
{
private:
    int levelIndex = 0;
    std::vector<std::string> files;
    GameManager();
    ~GameManager();
    
public:
    std::vector<std::string> words;
    static GameManager* getInstance();
    std::string nextLevel();
};

#endif /* GameManager_hpp */
