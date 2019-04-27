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

#define IS_ON_PC (CC_TARGET_PLATFORM == CC_PLATFORM_WINDOWS \
    || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX \
    || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)

#endif
