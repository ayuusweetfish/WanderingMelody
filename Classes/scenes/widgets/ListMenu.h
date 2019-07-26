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
    class Item {
    protected:
        std::string _title;
        std::function<void ()> _selectCallback;
        int _value;
        int _minValue, _maxValue;
        bool _doesCycle;
        std::vector<std::string> _enumText;

        friend class ListMenu;

    public:
        Item() { }
        Item(std::string title, std::function<void ()> callback,
            int value, int minValue, int maxValue, bool doesCycle)
        : _title(title),
          _selectCallback(callback),
          _value(value),
          _minValue(minValue),
          _maxValue(maxValue),
          _doesCycle(doesCycle)
        { }
        Item(std::string title, std::function<void ()> callback,
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

    bool initWithItems(const std::vector<Item> &items);
    static inline ListMenu *createWithItems(const std::vector<Item> &items) {
        ListMenu *ret = new ListMenu();
        if (ret && !ret->initWithItems(items)) {
            delete ret;
            return nullptr;
        }
        return ret;
    }

    void setContentSize(const cocos2d::Size &size) override;

protected:
    int _selIndex;
    std::vector<Item> _items;
    std::vector<std::pair<cocos2d::Label *, cocos2d::Label *>> _labels;

    cocos2d::Node *_marker;

    void updateText(int index);
    void updateItemPositions();
};

#endif
