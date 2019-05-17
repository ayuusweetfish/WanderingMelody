#ifndef __WanderingMelody__Config_h__
#define __WanderingMelody__Config_h__

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

#include "ConfigList.txt"

#undef CONFIG_ITEM

public:
    static inline void load() { }
    static inline void save() { }
};

#endif
