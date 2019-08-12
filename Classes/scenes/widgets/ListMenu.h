#ifndef __WanderingMelody__ListMenu_h__
#define __WanderingMelody__ListMenu_h__

#include "cocos2d.h"

#include <functional>
#include <string>
#include <utility>
#include <vector>

class ListMenu : public cocos2d::Node
{
public:
    struct Item {
        std::string _title;
        std::function<void (Item &)> _selectCallback;
        int _value;
        int _minValue, _maxValue;
        bool _doesCycle;
        std::vector<std::string> _enumText;
        int _displayMul;
        std::string _prefix, _suffix;

        Item() { }
        Item(std::string title, std::function<void (Item &)> callback)
        : _title(title),
          _selectCallback(callback),
          _value(0),
          _minValue(0),
          _maxValue(-1)
        { }
        Item(std::string title, std::function<void (Item &)> callback,
            int value, int minValue, int maxValue,
            bool doesCycle = false, int displayMul = 1,
            std::string prefix = "", std::string suffix = "")
        : _title(title),
          _selectCallback(callback),
          _value(value),
          _minValue(minValue),
          _maxValue(maxValue),
          _doesCycle(doesCycle),
          _displayMul(displayMul),
          _prefix(prefix),
          _suffix(suffix)
        { }
        Item(std::string title, std::function<void (Item &)> callback,
            int value, const std::vector<std::string> &enumText)
        : _title(title),
          _selectCallback(callback),
          _value(value),
          _minValue(0),
          _maxValue(enumText.size() - 1),
          _doesCycle(true),
          _enumText(enumText)
        { }
    };

    bool initWithItems(const std::vector<Item> &items, int fontSize = 28, bool centred = false);
    static inline ListMenu *createWithItems(const std::vector<Item> &items) {
        ListMenu *ret = new ListMenu();
        if (ret && !ret->initWithItems(items)) {
            delete ret;
            return nullptr;
        }
        return ret;
    }

    void setContentSize(const cocos2d::Size &size) override;

    void moveIn();
    void moveOut();

protected:
    void handleKey(int keyCode);    // Handles both keyboard and gamepads

    int _fontSize;
    bool _centred;

    int _selIndex;
    std::vector<Item> _items;
    std::vector<std::pair<cocos2d::Label *, cocos2d::Label *>> _labels;

    cocos2d::Node *_marker;

    void updateText(int index);
    void updateItemPositions();
};

#endif
