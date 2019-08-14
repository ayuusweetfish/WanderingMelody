#ifndef __WanderingMelody__KeyHintLabel_h__
#define __WanderingMelody__KeyHintLabel_h__

#include "cocos2d.h"

class KeyHintLabel : public cocos2d::Label
{
public:
    struct Entry {
        int key;
        std::string desc;
    };
    bool init(cocos2d::Color4B colour, std::vector<Entry> entries);

    static inline KeyHintLabel *create(const cocos2d::Color4B colour, const std::vector<Entry> &entries) {
        KeyHintLabel *ret = new KeyHintLabel();
        if (ret && !ret->init(colour, entries)) {
            delete ret;
            return nullptr;
        }
        return ret;
    }

protected:
};

#endif
