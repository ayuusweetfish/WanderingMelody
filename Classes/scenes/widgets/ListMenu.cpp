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

    auto markerLabel = Label::createWithTTF(">", "OpenSans-Light.ttf", 28);
    markerLabel->setAlignment(TextHAlignment::RIGHT);
    markerLabel->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    markerLabel->setPosition(Vec2(-6, 0));
    markerLabel->setColor(Color3B(255, 128, 0));
    this->addChild(markerLabel);
    _marker = markerLabel;

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
            _selIndex = (_selIndex - 1 + _items.size()) % _items.size();
            updateItemPositions();
        } else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
            _selIndex = (_selIndex + 1) % _items.size();
            updateItemPositions();
        } else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
            auto &item = _items[_selIndex];
            if ((item._value -= 1) < item._minValue)
                item._value = (item._doesCycle ? item._maxValue : item._minValue);
            updateText(_selIndex);
        } else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
            auto &item = _items[_selIndex];
            if ((item._value += 1) > item._maxValue)
                item._value = (item._doesCycle ? item._minValue : item._maxValue);
            updateText(_selIndex);
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
    }
}

void ListMenu::updateItemPositions()
{
    float extraHeight = _items.size() * 28 - _contentSize.height;
    float offsetY = (extraHeight < 0 ? 0 : extraHeight * _selIndex / (_items.size() - 1));

    _marker->stopAllActions();
    _marker->runAction(EaseQuadraticActionOut::create(
        MoveTo::create(0.2, Vec2(-6, -_selIndex * 28 + offsetY))
    ));

    for (int i = 0; i < _items.size(); i++) {
        Label *l1 = _labels[i].first;
        Label *l2 = _labels[i].second;
        float t = 0.5 + 1.0 * i / (_items.size() - 1);
        l1->stopAllActions();
        l1->runAction(EaseExponentialOut::create(
            MoveTo::create(t, Vec2(0, -i * 28 + offsetY))
        ));
        if (l2 != nullptr) {
            l2->stopAllActions();
            l2->runAction(EaseExponentialOut::create(
                MoveTo::create(t, Vec2(_contentSize.width, -i * 28 + offsetY))
            ));
        }
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
