#include "KeyHintLabel.h"
#include "Global.h"

using namespace cocos2d;

static const int LABEL_HEIGHT = 40;

bool KeyHintLabel::init(Color4B colour, std::vector<Entry> entries)
{
    std::string text;
    for (const Entry &e : entries) {
        if (!text.empty()) text += "     ";
        text += "[" + buttonName(e.key) + "] " + e.desc;
    }

    // Alignment parameters to initWithTTF() have no effect
    // XXX: Report this as an issue?
    if (!Label::initWithTTF(text, "OpenSans-Light.ttf", 32)) return false;

    this->setContentSize(Size(WIN_W / 4, LABEL_HEIGHT));
    this->setHorizontalAlignment(TextHAlignment::CENTER);
    this->setVerticalAlignment(TextVAlignment::CENTER);

    return true;
}
