#include "Gameplay.h"
#include "Global.h"
#include "gig/Musician.h"
#include "gig/MusicianNKeys.h"
using namespace cocos2d;

bool Gameplay::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255))) return false;

    Gig::FileReadResult r = _gig.init("/Users/lsq/Downloads/OpenXLSX-master/exp/2.txt");
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

    int numMusicians = _gig.getMusicianCount();
    Musician::Display *mus[numMusicians];
    for (int i = 0; i < numMusicians; i++) {
        mus[i] = _gig.getMusician(i)->createDisplay();
        mus[i]->setContentSize(Size(WIN_W * 0.23, WIN_H));
        mus[i]->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        mus[i]->setPosition(Vec2(WIN_W * (0.016 + 0.246 * i), 0));
        this->addChild(mus[i]);
    }

    ((MusicianNKeys<4> *)_gig.getMusician(0))->setKeyMapping((int []){149, 147, 126, 145});
    //((MusicianNKeys<2> *)_gig.getMusician(1))->setKeyMapping((int []){134, 135});

    auto layerStart = LayerColor::create(Color4B(240, 235, 230, 192), WIN_W / 2, WIN_H / 8);
    layerStart->setPosition((WIN_SIZE - Size(WIN_W / 2, WIN_H / 8)) / 2);
    this->addChild(layerStart, 9999);
    _layerStart = layerStart;

    auto labelStart = Label::createWithTTF("Press Enter", "fonts/arial.ttf", 42);
    labelStart->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    labelStart->setPosition(Size(WIN_W / 2, WIN_H / 8) / 2);
    labelStart->setColor(Color3B(64, 64, 64));
    layerStart->addChild(labelStart);
    _labelStart = labelStart;

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        if (_playState == 0 && keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
            _labelStart->runAction(Sequence::createWithTwoActions(
                EaseQuadraticActionIn::create(
                    Spawn::createWithTwoActions(
                        ScaleTo::create(0.4, 2), FadeOut::create(0.4)
                    )
                ),
                CallFunc::create([this] () {
                    for (int i = 0; i < _gig.getMusicianCount(); i++)
                        _gig.getMusician(i)->startPlay();
                })
            ));
            _layerStart->runAction(FadeOut::create(0.4));
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}
