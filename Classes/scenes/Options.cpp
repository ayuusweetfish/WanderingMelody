#include "Options.h"
#include "Global.h"
#include "widgets/ListMenu.h"
#include "widgets/KeyBindingsPanel.h"
using namespace cocos2d;

#include <vector>

bool Options::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255))) return false;

    auto label = Label::createWithTTF(
        "Hello options!",
        "OpenSans-Light.ttf", 28);
    label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    label->setPosition(WIN_SIZE / 2);
    label->setColor(Color3B(0, 0, 0));
    this->addChild(label);

    auto menu = new ListMenu();
    auto keyBindings = KeyBindingsPanel::create();

    std::vector<ListMenu::Item> items;
    for (int i = 0; i < 10; i++)
        items.push_back(ListMenu::Item("Hi", nullptr, -i, -i * 2, 5, false));
    for (int i = 0; i < 10; i++)
        items.push_back(ListMenu::Item("Heya", [] (ListMenu::Item &i) { i._value = 0; },
            i % 3, {"Yes", "Yeah", "Yup"}));
    items.push_back(ListMenu::Item("Key/Gamepad Mappings",
        [this, menu, keyBindings] (ListMenu::Item &) {
            menu->moveOut();
            keyBindings->moveIn();
        }
    ));
    menu->initWithItems(items);
    menu->setContentSize(Size(WIN_W * 0.8, WIN_H * 0.75));
    menu->setPosition(Vec2(WIN_W * 0.1, WIN_H * 0.85));
    this->addChild(menu);

    keyBindings->setContentSize(Size(WIN_W * 0.8, WIN_H * 0.75));
    keyBindings->setPosition(Vec2(WIN_W * 0.1, WIN_H * 0.85));
    this->addChild(keyBindings);
    keyBindings->setCancelCallback([this, menu, keyBindings] () {
        keyBindings->moveOut();
        menu->moveIn();
    });
    // Needs to be executed after onEnter() so that the listener is not resumed
    this->scheduleOnce([this, keyBindings] (float dt) {
        _eventDispatcher->pauseEventListenersForTarget(keyBindings);
    }, 0, "OvO");
    keyBindings->moveOut(true);

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        if (Config::isKeyCancel(keyCode)) GO_BACK_SCENE();
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
};
