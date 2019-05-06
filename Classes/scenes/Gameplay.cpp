#include "Gameplay.h"
#include "Global.h"
#include "widgets/Musician.h"
using namespace cocos2d;

bool Gameplay::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255))) return false;

    auto mus = Musician::create();
    mus->setPosition(Point::ZERO);
    this->addChild(mus);

    return true;
}
