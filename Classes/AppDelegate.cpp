#include "AppDelegate.h"
#include "LevelScene.hpp"
#include "Util.hpp"
#include "GameManager.hpp"

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(640, 1136);
static cocos2d::Size smallResolutionSize = cocos2d::Size(320, 568);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);
static cocos2d::Size macResolutionSize = cocos2d::Size(432, 768);

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("BBL", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
        glview = GLViewImpl::createWithRect("BBL", cocos2d::Rect(0, 0, macResolutionSize.width, macResolutionSize.height));
#else
        glview = GLViewImpl::create("BBL");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    auto frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
    if (frameSize.height > mediumResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else
    {        
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }

    register_all_packages();
    
    auto spriteFrameCache = SpriteFrameCache::getInstance();
    spriteFrameCache->addSpriteFramesWithFile("fonts/letters-0.plist");
    spriteFrameCache->addSpriteFramesWithFile("fonts/letters-1.plist");
    spriteFrameCache->addSpriteFramesWithFile("fonts/letters-2.plist");
    spriteFrameCache->addSpriteFramesWithFile("fonts/letters-3.plist");
    spriteFrameCache->addSpriteFramesWithFile("fonts/letters-4.plist");
    spriteFrameCache->addSpriteFramesWithFile("fonts/letters-5.plist");
    spriteFrameCache->addSpriteFramesWithFile("fonts/letters-6.plist");
    spriteFrameCache->addSpriteFramesWithFile("fonts/letters-7.plist");
    spriteFrameCache->addSpriteFramesWithFile("fonts/letters-8.plist");
    
    auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
    audio->setEffectsVolume(1);
    
    preloadSound("bubbling");
    preloadSound("bubbling2");
    preloadSound("burst1");
    preloadSound("burst2");
    preloadSound("pop1");
    preloadSound("pop2");
    preloadSound("stretch");
    preloadSound("woosh1");
    preloadSound("woosh2");
    preloadSound("woosh3");
    
    auto gm = GameManager::getInstance();
    
    // create a scene. it's an autorelease object
    auto scene = LevelScene::createScene("1-1");

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}
