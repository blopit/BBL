//
//  Util.hpp
//  BBL
//
//  Created by Shrenil Patel on 2017-11-11.
//

#ifndef Util_hpp
#define Util_hpp

//#define USE_AUDIO_ENGINE 1
#define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

#include <stdio.h>

float getX(int len, int idx, float sp = 96);

template <class T>
T boundToRange(T lower, T value, T upper) {
    return value < lower ? lower : value > upper ? upper : value;
}

template <class T>
void remove(std::vector<T> &vec, T element) {
    vec.erase(std::remove(vec.begin(), vec.end(), element), vec.end());
}

float randFloat(float a, float b);

void playSound(std::string name, bool loop, float pitch = 1.0f, float volume = 1.0f, float pan = 1.0f, float gain = 1.0f);

void preloadSound(std::string name);

std::vector<std::string> split_string(const std::string& str, const std::string& delimiter);
#endif /* Util_hpp */
