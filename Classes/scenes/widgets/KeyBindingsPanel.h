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
    inline void setCancelCallback(const std::function<void ()> &callback) {
        _cancelCallback = callback;
    }

    void moveIn();
    void moveOut(bool instant = false);

protected:
    void handleButton(int keyCode); // Handles both keyboard and gamepads

    std::function<void ()> _cancelCallback;

    int _selRow, _selCol;
    cocos2d::Node *_marker;

    struct LabelRow {
        cocos2d::Label *_lala[5];
        inline cocos2d::Label *& operator [] (int index) {
            return _lala[index + 1];
        }
    };
    std::vector<LabelRow> _labels;
    std::vector<std::function<int (int)>> _getters;
    std::vector<std::function<void (int, int)>> _setters;

    struct Array4 { int val[4]; };
    std::vector<Array4> _stagedSettings;

    bool _entering;

    void addItem(const char *name);

    float getXForCol(float col);

    void updateText(int row, int col);
    void updateItemPositions();

    void writeBackSettings();
};

#endif
