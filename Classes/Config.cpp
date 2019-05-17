#include "Config.h"

#define CONFIG_ITEM(__type, __name, __default) \
    __type Config::_##__name = __default;

#include "ConfigList.txt"

#undef CONFIG_ITEM
