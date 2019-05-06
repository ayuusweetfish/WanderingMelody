#include "Gameplay.h"
#include "Global.h"
#include "data/GameData.h"
#include "widgets/MusicianNode.h"
using namespace cocos2d;

bool Gameplay::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255))) return false;

    Gig gig;
    Gig::FileReadResult r = gig.init("/Users/lsq/Downloads/OpenXLSX-master/exp/1.txt");
    if (!r.succeeded) {
        auto label = Label::createWithTTF(r.errMsg, "fonts/arial.ttf", 42);
        label->setMaxLineWidth(WIN_W * 2 / 3);
        label->enableWrap(true);
        label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        label->setPosition(WIN_SIZE / 2);
        label->setColor(Color3B(0, 0, 0));
        this->addChild(label);
        return true;
    }

    MusicianNode *mus[4];
    for (int i = 0; i < 4; i++) {
        mus[i] = MusicianNodeBasicKeys::create();
        mus[i]->setContentSize(Size(WIN_W * 0.23, WIN_H));
        mus[i]->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        mus[i]->setPosition(Vec2(WIN_W * (0.016 + 0.246 * i), 0));
        this->addChild(mus[i]);
    }

    return true;
}
