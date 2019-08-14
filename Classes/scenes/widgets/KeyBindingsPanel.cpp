#include "KeyBindingsPanel.h"
#include "Global.h"
using namespace cocos2d;

#include <cstdlib>

static const int FONT_SZ = 28;
static const int LINE_HT = 32;

static const int ACTION_MOVE_TAG = 6135;

float KeyBindingsPanel::getXForCol(float col)
{
    return (col == -1 ? 0 : _contentSize.width * (0.475 + 0.15 * col));
}

bool KeyBindingsPanel::init()
{
    if (!Node::init()) return false;

    _selRow = _selCol = 0;
    _entering = false;

#define CONFIG_ITEM(...)
#define KEY_BIND(__name, ...) \
    _getters.push_back(Config::getKey##__name); \
    _setters.push_back(Config::setKey##__name); \
    this->addItem(#__name);

#include "ConfigList.txt"

    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        event->stopPropagation();
        handleButton((int)keyCode);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    auto controllerListener = EventListenerController::create();
    controllerListener->onKeyDown = [this](Controller *controller, int keyCode, Event *event) {
        event->stopPropagation();
        handleButton(gamepadCode(controller->getDeviceId(), keyCode));
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(controllerListener, this);

    // TODO: DRY? scenes/widgets/ListMenu.cpp
    auto markerLabel = Label::createWithTTF(">", "OpenSans-Light.ttf", FONT_SZ);
    markerLabel->setAlignment(TextHAlignment::RIGHT);
    markerLabel->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    markerLabel->setPosition(Vec2(-6, 0));
    markerLabel->setColor(Color3B(255, 128, 0));
    this->addChild(markerLabel);
    _marker = markerLabel;

    this->setContentSize(WIN_SIZE);
    return true;
}

void KeyBindingsPanel::addItem(const char *name)
{
    // TODO: DRY; gig/MusicianNKeysDisplay.cpp
    const Color4F playerColors[4] = {
        {0.8f, 0.8f, 0.5f, 0.9f},
        {1.0f, 0.7f, 1.0f, 0.9f},
        {0.5f, 1.0f, 1.0f, 0.9f},
        {0.7f, 1.0f, 0.7f, 0.9f}
    };

    LabelRow row;
    row[-1] = Label::createWithTTF(name, "OpenSans-Light.ttf", FONT_SZ);
    row[-1]->setAlignment(TextHAlignment::LEFT);
    row[-1]->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    row[-1]->setPosition(Vec2(0, -(int)_labels.size() * LINE_HT));
    row[-1]->setColor(Color3B(0, 0, 0));
    this->addChild(row[-1]);

    for (int i = 0; i < 4; i++) {
        row[i] = Label::createWithTTF("", "OpenSans-Light.ttf", FONT_SZ);
        row[i]->setAlignment(TextHAlignment::CENTER);
        row[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        // X will be changed in setContentSize()
        row[i]->setPosition(Vec2(0, -(int)_labels.size() * LINE_HT));
        row[i]->setColor(Color3B(playerColors[i]));
        this->addChild(row[i]);
    }

    _labels.push_back(row);

    Array4 arr;
    for (int i = 0; i < 4; i++)
        arr.val[i] = _getters[_stagedSettings.size()](i);
    _stagedSettings.push_back(arr);

    for (int i = 0; i < 4; i++) updateText(_labels.size() - 1, i);
}

void KeyBindingsPanel::updateText(int row, int col)
{
    int code = _stagedSettings[row].val[col];
    _labels[row][col]->setString(buttonName(code));
}

void KeyBindingsPanel::updateItemPositions()
{
    float extraHeight = (float)_labels.size() * LINE_HT - _contentSize.height;
    float offsetY = (extraHeight < 0 ? 0 : extraHeight * _selRow / (_labels.size() - 1));

    // Not that many items, only moving the marker is sufficient currently
    _marker->stopAllActionsByTag(ACTION_MOVE_TAG);
    _marker->runAction(EaseQuadraticActionOut::create(
        MoveTo::create(0.12, Vec2(
            getXForCol(_selCol - 0.5), -_selRow * LINE_HT + offsetY
        ))
    ))->setTag(ACTION_MOVE_TAG);

    for (int i = 0; i < _labels.size(); i++)
        for (int j = -1; j < 4; j++) {
            float t = 0.5 +
                1.0 * std::abs(i - _selRow) / (_labels.size() - 1) +
                0.2 * std::abs(j - _selCol);
            _labels[i][j]->stopAllActionsByTag(ACTION_MOVE_TAG);
            _labels[i][j]->runAction(EaseExponentialOut::create(
                MoveTo::create(t, Vec2(getXForCol(j), -i * LINE_HT + offsetY))
            ))->setTag(ACTION_MOVE_TAG);
        }
}

void KeyBindingsPanel::handleButton(int keyCode)
{
    if (_entering) {
        _entering = false;
        _marker->runAction(TintTo::create(0.2, Color3B(255, 128, 0)));
        this->unschedule("uwu");
        _stagedSettings[_selRow].val[_selCol] = keyCode;
        updateText(_selRow, _selCol);
        return;
    }

    if (Config::isKeyCancel(keyCode)) {
        if (_cancelCallback) _cancelCallback();
        writeBackSettings();
        return;
    } else if (Config::isKeyConfirm(keyCode)) {
        _entering = true;
        _marker->runAction(TintTo::create(0.2, Color3B(64, 128, 255)));
        this->scheduleOnce([this] (float dt) {
            _entering = false;
            _marker->runAction(TintTo::create(0.2, Color3B(255, 128, 0)));
            // Clear key binding on timeout
            _stagedSettings[_selRow].val[_selCol] = 0;
            updateText(_selRow, _selCol);
        }, 3, "uwu");
    }
    int linearIndex = _selRow * 4 + _selCol;
    if (Config::isKeyArrowUp(keyCode)) linearIndex -= 4;
    else if (Config::isKeyArrowDown(keyCode)) linearIndex += 4;
    else if (Config::isKeyArrowLeft(keyCode)) linearIndex -= 1;
    else if (Config::isKeyArrowRight(keyCode)) linearIndex += 1;
    if (linearIndex != _selRow * 4 + _selCol) {
        int modulus = _labels.size() * 4;
        linearIndex = (linearIndex + modulus) % modulus;
        _selRow = linearIndex / 4;
        _selCol = linearIndex % 4;
        updateItemPositions();
    }
}

void KeyBindingsPanel::setContentSize(const cocos2d::Size &size)
{
    Node::setContentSize(size);

    for (LabelRow &row : _labels)
        for (int i = 0; i < 4; i++)
            row[i]->setPositionX(getXForCol(i));

    updateItemPositions();
}

void KeyBindingsPanel::moveIn()
{
    for (int i = 0; i < _labels.size(); i++)
        for (int j = -1; j < 4; j++) {
            float t = 0.25 + 0.5 * (i * 5 + j + 1) / (_labels.size() * 5 - 1);
            auto a1 = EaseExponentialInOut::create(MoveBy::create(t, Vec2(-20, 0)));
            auto a2 = EaseExponentialInOut::create(FadeIn::create(t));
            _labels[i][j]->runAction(a1)->setTag(ACTION_MOVE_TAG);
            _labels[i][j]->runAction(a2);
            if (_selRow == i && _selCol == j) {
                _marker->runAction(a1->clone())->setTag(ACTION_MOVE_TAG);
                _marker->runAction(a2->clone());
            }
        }
    _eventDispatcher->resumeEventListenersForTarget(this);
}

void KeyBindingsPanel::moveOut(bool instant)
{
    for (int i = 0; i < _labels.size(); i++)
        for (int j = -1; j < 4; j++) {
            float t = (instant ?
                0 : 0.25 + 0.5 * (i * 5 + j + 1) / (_labels.size() * 5 - 1));
            auto a1 = EaseExponentialInOut::create(MoveBy::create(t, Vec2(+20, 0)));
            auto a2 = EaseExponentialInOut::create(FadeOut::create(t));
            _labels[i][j]->runAction(a1)->setTag(ACTION_MOVE_TAG);
            _labels[i][j]->runAction(a2);
            if (_selRow == i && _selCol == j) {
                _marker->runAction(a1->clone())->setTag(ACTION_MOVE_TAG);
                _marker->runAction(a2->clone());
            }
        }
    _eventDispatcher->pauseEventListenersForTarget(this);
}

void KeyBindingsPanel::writeBackSettings()
{
    for (int i = 0; i < _labels.size(); i++)
        for (int j = 0; j < 4; j++)
            _setters[i](j, _stagedSettings[i].val[j]);
}
