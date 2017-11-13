//
//  Util.cpp
//  BBL
//
//  Created by Shrenil Patel on 2017-11-11.
//

#include "Util.hpp"
#include <sstream>


float getX(int len, int idx, float sp) {
    return - ((len - 1) * sp / 2) + idx * sp;
}


float randFloat(float a, float b) {
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}

void playSound(std::string name, bool loop, float pitch, float volume, float pan, float gain) {
    
    std::stringstream ss;
    auto audio = SimpleAudioEngine::getInstance();
    audio->setEffectsVolume(volume);
    ss << "sfx/mp3/" << name << ".mp3";
    audio->playEffect(ss.str().c_str(), loop, pitch, pan, gain);
    
}

void preloadSound(std::string name) {
    std::stringstream ss;
    ss << "sfx/mp3/" << name << ".mp3";
    
    auto audio = SimpleAudioEngine::getInstance();
    
    audio->preloadEffect(ss.str().c_str());
}


std::vector<std::string> split_string(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> strings;
    
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + 1;
    }
    
    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));
    
    return strings;
}
