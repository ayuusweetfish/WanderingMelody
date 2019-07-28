#ifndef __WanderingMelody__Global_h__
#define __WanderingMelody__Global_h__

#define SCENE_FUNC(__type__) \
    static cocos2d::Scene* createScene() \
    { \
        auto scene = cocos2d::Scene::create(); \
        auto layer = __type__::create(); \
        scene->addChild(layer); \
        return scene; \
    }

#define GO_TO_SCENE(__nextScene__) do { \
    Director::getInstance()->pushScene( \
        TransitionFade::create(0.4, scene, Color3B::WHITE) \
    ); \
    LayerColor *cover = LayerColor::create(Color4B::WHITE); \
    cover->setOpacity(0); \
    this->addChild(cover, INT_MAX); \
    auto listener = EventListenerKeyboard::create(); \
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) { \
        event->stopPropagation(); \
    }; \
    cover->getEventDispatcher()-> \
        addEventListenerWithFixedPriority(listener, INT_MIN); \
    cover->runAction(Sequence::create( \
        DelayTime::create(0.4), \
        CallFunc::create([cover, listener]() { \
            cover->setOpacity(255); \
            cover->getEventDispatcher()->removeEventListener(listener); \
        }), \
        FadeOut::create(0.2), \
        RemoveSelf::create(), \
        nullptr \
    )); \
} while (0)

#define GO_BACK_SCENE() do { \
    LayerColor *cover = LayerColor::create(Color4B::WHITE); \
    this->getScene()->addChild(cover, INT_MAX); \
    auto listener = EventListenerKeyboard::create(); \
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) { \
        event->stopPropagation(); \
    }; \
    cover->getEventDispatcher()-> \
        addEventListenerWithFixedPriority(listener, INT_MIN); \
    cover->setOpacity(0); \
    cover->runAction(FadeIn::create(0.2)); \
    Director::getInstance()->getScheduler()->schedule( \
        [cover, listener](float _) { \
            Director::getInstance()->popScene(); \
            cover->getEventDispatcher()->removeEventListener(listener); \
        }, \
        Director::getInstance(), 0, 0, 0.2, false, "go_back"); \
} while (0)

#define IS_ON_PC (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 \
    || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX \
    || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)

#if IS_ON_PC
    #define WIN_W   960
    #define WIN_H   540
#else
    #define WIN_W   640
    #define WIN_H   360
#endif

#define WIN_SIZE cocos2d::Size(WIN_W, WIN_H)

#define HIT_LINE_POS    (WIN_H / 3.0)

#define CONTROLLER_KEY_STEP ((int)Controller::Key::JOYSTICK_LEFT_X)

#include "Config.h"

#endif
