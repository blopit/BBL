//
//  GameManager.cpp
//  BBL
//
//  Created by Shrenil Patel on 2017-11-15.
//

#include "GameManager.hpp"
#include "LevelScene.hpp"
#include "Util.hpp"

USING_NS_CC;

static GameManager* gameManager = nullptr;

GameManager::GameManager() {
    files = {
        "1-1",
        "assault_related",
        "bald_related",
        "brag_related",
        "cast_related",
        "certain_related",
        "clash_related",
        "collar_related",
        "convention_related",
        "cure_related",
        "dip_related",
        "dirt_related",
        "escape_related",
        "feeling_related",
        "fluffy_related",
        "forest_descby",
        "forest_related",
        "forest_trg",
        "ground_related",
        "have_related",
        "hover_related",
        "imagine_related",
        "manner_descby",
        "manner_related",
        "motivation_related",
        "nature_related",
        "probable_related",
        "qualify_related",
        "safari_related",
        "sand_trg",
        "silver_related",
        "size_related",
        "soul_related",
        "suite_related",
        "switch_related",
        "tolerant_related",
        "tower_related",
        "wheel_related",
        "wood_related",
        "world_related",
        "incredible_related",
    };
    
    auto i = 0;
    for (auto f : files) {
        auto level = new Level(f, i);
        levels.push_back(level);
        i++;
    }
    
    auto fileData = FileUtils::getInstance()->getDataFromFile("dict");
    std::string content((const char*)fileData.getBytes(), fileData.getSize());
    
    auto lines = split_string(content, "\n");
    for (auto line : lines) {
        words.push_back(line);
    }
}

GameManager* GameManager::getInstance() {
    if (!gameManager) {
        gameManager = new (std::nothrow)GameManager();
    }
    return gameManager;
}

void GameManager::completeLevel(std::string level, int stars, std::vector<std::string> hiddenWords, int score) {
    for (auto l : levels) {
        if (l->fname == level) {
            l->stars = stars;
            l->hiddenWords = hiddenWords;
            l->highscore = score > l->highscore ? score : l->highscore;
            if (levelIndex < l->index + 1) {
                levelIndex = l->index + 1;
            }
            break;
        }
    }
}

std::string GameManager::nextLevel(std::string current) {
    int idx = int(std::find(files.begin(), files.end(), current) - files.begin());
    return files[idx + 1];
}

void GameManager::restart() {
    auto scene = LevelScene::createScene(currentLevel);
    Director::getInstance()->replaceScene(scene);
}
void GameManager::next() {
    auto scene = LevelScene::createScene(nextLevel(currentLevel));
    Director::getInstance()->replaceScene(scene);
}

