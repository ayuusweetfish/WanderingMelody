#include "ListMenu.h"
#include "Global.h"
using namespace cocos2d;

#include <cstdio>

bool ListMenu::initWithItems(const std::vector<Item> &items)
{
    if (!Node::init()) return false;

    _selIndex = 0;
    _items = items;

    for (int i = 0; i < items.size(); i++) {
        const Item &item = items[i];

        Label *l1 = Label::createWithTTF(item._title, "OpenSans-Light.ttf", 28);
        Label *l2 = nullptr;

        l1->setAlignment(TextHAlignment::LEFT);
        l1->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        l1->setPosition(Vec2(0, -i * 28));
        l1->setColor(Color3B(0, 0, 0));
        this->addChild(l1);

        if (item._minValue <= item._maxValue) {
            l2 = Label::createWithTTF("", "OpenSans-Light.ttf", 28);
            l2->setAlignment(TextHAlignment::RIGHT);
            l2->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
            l2->setPosition(Vec2(0, -i * 28));
            l2->setColor(Color3B(0, 0, 0));
            this->addChild(l2);
        }

        _labels.push_back({l1, l2});
        updateText(i);
    }

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
        } else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    this->setContentSize(WIN_SIZE);
    return true;
}

static inline std::string tostring(int val)
{
    char s[16];
    snprintf(s, sizeof s, "%d", val);
    return std::string(s);
}

void ListMenu::updateText(int index)
{
    const Item &item = _items[index];
    Label *l2 = _labels[index].second;
    if (l2 != nullptr) {
        if (item._enumText.empty())
            l2->setString(tostring(item._value));
        else
            l2->setString(item._enumText[item._value]);
        puts(l2->getString().c_str());
    }
}

void ListMenu::setContentSize(const Size &size)
{
    Node::setContentSize(size);

    for (int i = 0; i < _items.size(); i++) {
        const Item &item = _items[i];
        Label *l1 = _labels[i].first;
        Label *l2 = _labels[i].second;
        l1->setContentSize(Size(size.width, 28));
        if (l2 != nullptr) {
            l2->setContentSize(Size(size.width, 28));
            l2->setPositionX(size.width);
        }
    }
}
