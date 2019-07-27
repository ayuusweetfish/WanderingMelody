#include "Options.h"
#include "Global.h"
#include "widgets/ListMenu.h"
#include "widgets/KeyBindingsPanel.h"
using namespace cocos2d;

#include <vector>

bool Options::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255))) return false;

    auto menu = new ListMenu();
    auto keyBindings = KeyBindingsPanel::create();

    std::vector<ListMenu::Item> items;
    for (int i = 0; i < 10; i++)
        items.push_back(ListMenu::Item("Hi", nullptr, -i, -i * 2, 5, false));
    for (int i = 0; i < 10; i++)
        items.push_back(ListMenu::Item("Heya", [] (ListMenu::Item &i) { i._value = 0; },
            i % 3, {"Yes", "Yeah", "Yup"}));
    items.push_back(ListMenu::Item("Key/gamepad mappings",
        [this, menu, keyBindings] (ListMenu::Item &) {
            menu->moveOut();
            keyBindings->moveIn();
        }
    ));
    items.push_back(ListMenu::Item("Screen resolution", nullptr, 1,
        {"640 * 360", "960 * 540", "1440 * 810", "1920 * 1080", "2400 * 1350", "Fullscreen"}));
    items.push_back(ListMenu::Item("Audio offset", nullptr, 0, -1000, 1000, false));
    items.push_back(ListMenu::Item("Input offset", nullptr, 0, -1000, 1000, false));
    items.push_back(ListMenu::Item("Frame rate", nullptr, 1, {"30", "60", "90", "120", "240"}));
    items.push_back(ListMenu::Item("Master volume", nullptr, 20, 0, 20, false));
    items.push_back(ListMenu::Item("Sound effects volume", nullptr, 20, 0, 20, false));
    items.push_back(ListMenu::Item("Scroll speed", nullptr, 9, 1, 20, false));
    items.push_back(ListMenu::Item("Display stats", nullptr, 0, {"Yes", "No"}));
    menu->initWithItems(items);
    menu->setContentSize(Size(WIN_W * 0.8, WIN_H * 0.8));
    menu->setPosition(Vec2(WIN_W * 0.1, WIN_H * 0.9));
    this->addChild(menu);

    keyBindings->setContentSize(Size(WIN_W * 0.8, WIN_H * 0.8));
    keyBindings->setPosition(Vec2(WIN_W * 0.1, WIN_H * 0.9));
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
