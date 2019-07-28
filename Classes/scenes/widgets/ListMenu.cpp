#include "ListMenu.h"
#include "Global.h"
using namespace cocos2d;

#include <cstdio>
#include <cstdlib>

static const int FONT_SZ = 28;
static const int LINE_HT = 32;

static const int ACTION_MOVE_TAG = 6135;

bool ListMenu::initWithItems(const std::vector<Item> &items)
{
    if (!Node::init()) return false;

    _selIndex = 0;
    _items = items;

    for (int i = 0; i < items.size(); i++) {
        const Item &item = items[i];

        Label *l1 = Label::createWithTTF(item._title, "OpenSans-Light.ttf", FONT_SZ);
        Label *l2 = nullptr;

        l1->setAlignment(TextHAlignment::LEFT);
        l1->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        l1->setPosition(Vec2(0, -i * LINE_HT));
        l1->setColor(Color3B(0, 0, 0));
        this->addChild(l1);

        if (item._minValue <= item._maxValue) {
            l2 = Label::createWithTTF("", "OpenSans-Light.ttf", FONT_SZ);
            l2->setAlignment(TextHAlignment::RIGHT);
            l2->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
            l2->setPosition(Vec2(0, -i * LINE_HT));
            l2->setColor(Color3B(0, 0, 0));
            this->addChild(l2);
        }

        _labels.push_back({l1, l2});
        updateText(i);
    }

    auto markerLabel = Label::createWithTTF(">", "OpenSans-Light.ttf", FONT_SZ);
    markerLabel->setAlignment(TextHAlignment::RIGHT);
    markerLabel->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    markerLabel->setPosition(Vec2(-6, 0));
    markerLabel->setColor(Color3B(255, 128, 0));
    this->addChild(markerLabel);
    _marker = markerLabel;

    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        handleKey((int)keyCode);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    auto controllerListener = EventListenerController::create();
    controllerListener->onKeyDown = [this](Controller *controller, int keyCode, Event *event) {
        handleKey((int)keyCode + controller->getDeviceId() * CONTROLLER_KEY_STEP);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(controllerListener, this);

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
            l2->setString(
                (item._prefix == "+" && item._value < 0 ? "" : item._prefix) +
                tostring(item._value * item._displayMul) + item._suffix);
        else
            l2->setString(item._enumText[item._value]);
    }
}

void ListMenu::updateItemPositions()
{
    float extraHeight = _items.size() * LINE_HT - _contentSize.height;
    float offsetY = (extraHeight < 0 ? 0 : extraHeight * _selIndex / (_items.size() - 1));

    _marker->stopAllActionsByTag(ACTION_MOVE_TAG);
    _marker->runAction(EaseQuadraticActionOut::create(
        MoveTo::create(0.2, Vec2(-6, -_selIndex * LINE_HT + offsetY))
    ))->setTag(ACTION_MOVE_TAG);

    for (int i = 0; i < _items.size(); i++) {
        Label *l1 = _labels[i].first;
        Label *l2 = _labels[i].second;
        float t = 0.5 + 1.0 * std::abs(i - _selIndex) / (_items.size() - 1);
        l1->stopAllActionsByTag(ACTION_MOVE_TAG);
        l1->runAction(EaseExponentialOut::create(
            MoveTo::create(t, Vec2(0, -i * LINE_HT + offsetY))
        ))->setTag(ACTION_MOVE_TAG);
        if (l2 != nullptr) {
            l2->stopAllActionsByTag(ACTION_MOVE_TAG);
            l2->runAction(EaseExponentialOut::create(
                MoveTo::create(t, Vec2(_contentSize.width, -i * LINE_HT + offsetY))
            ))->setTag(ACTION_MOVE_TAG);
        }
    }
}

void ListMenu::handleKey(int keyCode)
{
    auto &item = _items[_selIndex];
    if (Config::isKeyArrowUp(keyCode)) {
        _selIndex = (_selIndex - 1 + _items.size()) % _items.size();
        updateItemPositions();
    } else if (Config::isKeyArrowDown(keyCode)) {
        _selIndex = (_selIndex + 1) % _items.size();
        updateItemPositions();
    } else if (Config::isKeyArrowLeft(keyCode)) {
        if ((item._value -= 1) < item._minValue)
            item._value = (item._doesCycle ? item._maxValue : item._minValue);
        updateText(_selIndex);
    } else if (Config::isKeyArrowRight(keyCode)) {
        if ((item._value += 1) > item._maxValue)
            item._value = (item._doesCycle ? item._minValue : item._maxValue);
        updateText(_selIndex);
    } else if (Config::isKeyConfirm(keyCode)) {
        if (item._selectCallback) item._selectCallback(item);
        updateText(_selIndex);
    }
}

void ListMenu::setContentSize(const Size &size)
{
    Node::setContentSize(size);

    for (int i = 0; i < _items.size(); i++) {
        const Item &item = _items[i];
        Label *l1 = _labels[i].first;
        Label *l2 = _labels[i].second;
        l1->setContentSize(Size(size.width, LINE_HT));
        if (l2 != nullptr) {
            l2->setContentSize(Size(size.width, LINE_HT));
            l2->setPositionX(size.width);
        }
    }
}

void ListMenu::moveIn()
{
    for (int i = 0; i < _items.size(); i++) {
        Label *l1 = _labels[i].first;
        Label *l2 = _labels[i].second;
        float t = 0.25 + 0.5 * i / (_items.size() - 1);
        auto a1 = EaseExponentialInOut::create(MoveBy::create(t, Vec2(+20, 0)));
        auto a2 = EaseExponentialInOut::create(FadeIn::create(t));
        l1->runAction(a1)->setTag(ACTION_MOVE_TAG);
        l1->runAction(a2);
        if (l2 != nullptr) {
            l2->runAction(a1->clone())->setTag(ACTION_MOVE_TAG);
            l2->runAction(a2->clone());
        }
        if (i == _selIndex) {
            _marker->runAction(a1->clone())->setTag(ACTION_MOVE_TAG);
            _marker->runAction(a2->clone());
        }
    }
    _eventDispatcher->resumeEventListenersForTarget(this);
}

void ListMenu::moveOut()
{
    for (int i = 0; i < _items.size(); i++) {
        Label *l1 = _labels[i].first;
        Label *l2 = _labels[i].second;
        float t = 0.25 + 0.5 * i / (_items.size() - 1);
        auto a1 = EaseExponentialInOut::create(MoveBy::create(t, Vec2(-20, 0)));
        auto a2 = EaseExponentialInOut::create(FadeOut::create(t));
        l1->runAction(a1)->setTag(ACTION_MOVE_TAG);
        l1->runAction(a2);
        if (l2 != nullptr) {
            l2->runAction(a1->clone())->setTag(ACTION_MOVE_TAG);
            l2->runAction(a2->clone());
        }
        if (i == _selIndex) {
            _marker->runAction(a1->clone())->setTag(ACTION_MOVE_TAG);
            _marker->runAction(a2->clone());
        }
    }
    _eventDispatcher->pauseEventListenersForTarget(this);
}
