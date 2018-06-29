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

void AdListener::adViewDidDismissScreen(const std::string &name) {
    if (name == "gameover") {
        GameManager::getInstance()->restart();
    }
}

GameManager::GameManager() {
    sdkbox::PluginAdMob::cache("gameover");
    sdkbox::PluginAdMob::setListener(new AdListener());
    
    sdkbox::PluginSdkboxPlay::setListener(this);
    sdkbox::PluginSdkboxPlay::signin();
    
    files = {
        "1-1",
        "0_current",
        "1_glass",
        "2_broken",
        "3_stone",
        "4_increase",
        "5_reading",
        "6_alive",
        "7_oil",
        "8_aware",
        "9_church",
        "10_save",
        "11_edge",
        "12_chief",
        "13_writing",/**/
        "14_list",
        "15_interesting",
        "16_material",
        "17_offer",
        "18_picture",
        "19_huge",
        "20_phone",
        "21_charge",
        "22_hurt",
        "23_price",
        "24_gold",
        "25_telling",
        "26_camp",
        "27_size",
        "28_distance",
        "29_river",
        "30_fair",
        "31_gun",
        "32_value",
        "33_married",
        "34_remain",
        "35_growth",
        "36_empty",
        "37_legs",
        "38_check",
        "39_key",
        "40_north",
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
    
    fileData = FileUtils::getInstance()->getDataFromFile("smalldict");
    std::string cnt((const char*)fileData.getBytes(), fileData.getSize());
    lines = split_string(cnt, "\n");
    for (auto line : lines) {
        smwords.push_back(line);
    }
    //std::random_shuffle(smwords.begin(), smwords.end());
    
    currentLevel = files[0];
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

int GameManager::level() {
    int idx = int(std::find(files.begin(), files.end(), currentLevel) - files.begin());
    return idx + 1;
}

