#ifndef __WanderingMelody__Config_h__
#define __WanderingMelody__Config_h__

#include "cocos2d.h"

class Config {
#define CONFIG_ITEM(__type, __name, __default) \
public: \
    static inline void set##__name(__type val) { \
        _##__name = val; \
        save(); \
    } \
    static inline __type get##__name() { return _##__name; } \
private: \
    static __type _##__name;

#define KEY_BIND(__name, __p1, __p2, __p3, __p4) \
public: \
    static inline void setKey##__name(int index, \
        cocos2d::EventKeyboard::KeyCode code) \
    { \
        _key##__name[index] = code; \
        save(); \
    } \
    static inline cocos2d::EventKeyboard::KeyCode getKey##__name(int index) { \
        return _key##__name[index]; \
    } \
    static inline bool isKey##__name(cocos2d::EventKeyboard::KeyCode code) { \
        for (int i = 0; i < 4; i++) if (_key##__name[i] == code) return true; \
        return false; \
    } \
private: \
    static cocos2d::EventKeyboard::KeyCode _key##__name[4];

#include "ConfigList.txt"

#undef CONFIG_ITEM
#undef KEY_BIND

public:
    static inline void load() { }
    static inline void save() { }
};

#endif
