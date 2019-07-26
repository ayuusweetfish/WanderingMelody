#ifndef __WanderingMelody__KeyBindingsPanel_h__
#define __WanderingMelody__KeyBindingsPanel_h__

#include "cocos2d.h"

#include <functional>
#include <vector>

class KeyBindingsPanel : public cocos2d::Node {
public:
    bool init() override;
    CREATE_FUNC(KeyBindingsPanel);

    void setContentSize(const cocos2d::Size &size) override;

protected:
    int _selRow, _selCol;
    cocos2d::Node *_marker;

    struct LabelRow {
        cocos2d::Label *_lala[5];
        inline cocos2d::Label *& operator [] (int index) {
            return _lala[index + 1];
        }
    };
    std::vector<LabelRow> _labels;
    std::vector<std::function<cocos2d::EventKeyboard::KeyCode (int)>> _getters;
    std::vector<std::function<void (int, cocos2d::EventKeyboard::KeyCode)>> _setters;

    void addItem(const char *name);

    void updateText(int row, int col);
    void updateItemPositions();
};

#endif
