#include "KeyBindingsPanel.h"
#include "Global.h"
using namespace cocos2d;

bool KeyBindingsPanel::init()
{
    if (!Node::init()) return false;

    _selRow = _selCol = 0;


#define CONFIG_ITEM(...)
#define KEY_BIND(__name, ...) \
    _getters.push_back(Config::getKey##__name); \
    _setters.push_back(Config::setKey##__name); \
    this->addItem(#__name);

#include "ConfigList.txt"

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
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
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // TODO: DRY? scenes/widgets/ListMenu.cpp
    auto markerLabel = Label::createWithTTF(">", "OpenSans-Light.ttf", 28);
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
    row[-1] = Label::createWithTTF(name, "OpenSans-Light.ttf", 28);
    row[-1]->setAlignment(TextHAlignment::LEFT);
    row[-1]->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    row[-1]->setPosition(Vec2(0, -(int)_labels.size() * 28));
    row[-1]->setColor(Color3B(0, 0, 0));
    this->addChild(row[-1]);

    for (int i = 0; i < 4; i++) {
        row[i] = Label::createWithTTF("", "OpenSans-Light.ttf", 28);
        row[i]->setAlignment(TextHAlignment::CENTER);
        row[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        // X will be changed in setContentSize()
        row[i]->setPosition(Vec2(0, -(int)_labels.size() * 28));
        row[i]->setColor(Color3B(playerColors[i]));
        this->addChild(row[i]);
    }

    _labels.push_back(row);

    for (int i = 0; i < 4; i++) updateText(_labels.size() - 1, i);
}

void KeyBindingsPanel::updateText(int row, int col)
{
    auto code = _getters[row](col);
    if (code == EventKeyboard::KeyCode::KEY_NONE)
        _labels[row][col]->setString("--");
    else
        _labels[row][col]->setString("\\('~')/");
}

void KeyBindingsPanel::updateItemPositions()
{
    // Not that many items, only moving the marker is sufficient currently
    _marker->stopAllActions();
    _marker->runAction(EaseQuadraticActionOut::create(
        MoveTo::create(0.12, Vec2(_contentSize.width * (0.4 + 0.15 * _selCol), -28 * _selRow))
    ));
}

void KeyBindingsPanel::setContentSize(const cocos2d::Size &size)
{
    Node::setContentSize(size);

    for (LabelRow &row : _labels)
        for (int i = 0; i < 4; i++)
            row[i]->setPositionX(size.width * (0.475 + 0.15 * i));

    updateItemPositions();
}
