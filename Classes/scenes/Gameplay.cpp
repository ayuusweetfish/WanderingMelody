#include "Gameplay.h"
#include "Global.h"
#include "gig/Musician.h"
#include "gig/MusicianNKeys.h"
using namespace cocos2d;

class Gameplay::ModPanel : public cocos2d::LayerColor
{
public:
    virtual bool init() override;
    CREATE_FUNC(ModPanel);

    void setNumMusicians(int num);
    inline bool isAutoplay(int i) { return _autoplay[i]; }

protected:
    bool _autoplay[4];
    cocos2d::Label *_autoplayLabel[4];
};

bool Gameplay::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255))) return false;

    _playState = -1;

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        if (_playState == 0 && keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
            _playState = 1;
            _labelStart->runAction(Sequence::createWithTwoActions(
                EaseQuadraticActionIn::create(
                    Spawn::createWithTwoActions(
                        ScaleTo::create(0.4, 2), FadeOut::create(0.4)
                    )
                ),
                CallFunc::create([this] () {
                    for (int i = 0; i < _gig.getMusicianCount(); i++)
                        _gig.getMusician(i)->setIsAutoplay(_modPanel->isAutoplay(i));
                    _gig.startPlay();
                })
            ));
            _layerStart->runAction(FadeOut::create(0.4));
        } else if (_playState == 0 && keyCode == EventKeyboard::KeyCode::KEY_TAB) {
            this->addChild(_modPanel, 9999);
            _modPanel->runAction(EaseQuadraticActionOut::create(
                Spawn::createWithTwoActions(
                    MoveTo::create(0.3, Vec2(0, 0)),
                    FadeIn::create(0.3)
                )
            ));
            event->stopPropagation();
        } else if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {
            GO_BACK_SCENE();
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

Gameplay::~Gameplay()
{
    if (_modPanel) _modPanel->release();
}

void Gameplay::load(const std::string &path)
{
    Gig::FileReadResult r = _gig.init(path);
    if (!r.succeeded) {
        auto label = Label::createWithTTF(r.errMsg, "OpenSans-Light.ttf", 42);
        label->setMaxLineWidth(WIN_W * 2 / 3);
        label->enableWrap(true);
        label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        label->setPosition(WIN_SIZE / 2);
        label->setColor(Color3B(0, 0, 0));
        this->addChild(label);
        return;
    }

    auto layerStart = LayerColor::create(Color4B(240, 235, 230, 192), WIN_W / 2, WIN_H / 8);
    layerStart->setPosition((WIN_SIZE - Size(WIN_W / 2, WIN_H / 8)) / 2);
    this->addChild(layerStart, 9998);
    _layerStart = layerStart;

    auto labelStart = Label::createWithTTF("Press Enter", "OpenSans-Light.ttf", 42);
    labelStart->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    labelStart->setPosition(Size(WIN_W / 2, WIN_H / 8) / 2);
    labelStart->setColor(Color3B(64, 64, 64));
    layerStart->addChild(labelStart);
    _labelStart = labelStart;

    int numMusicians = _gig.getMusicianCount();

    auto modPanel = ModPanel::create();
    modPanel->setNumMusicians(numMusicians);
    modPanel->setContentSize(Size(WIN_W, WIN_H / 2));
    modPanel->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    modPanel->setPosition(Vec2(0, -WIN_H / 12));
    modPanel->setOpacity(0);
    modPanel->setCascadeOpacityEnabled(true);
    modPanel->retain();
    _modPanel = modPanel;

    Musician::Display *mus[numMusicians];
    for (int i = 0; i < numMusicians; i++) {
        mus[i] = _gig.getMusician(i)->createDisplay();
        mus[i]->setContentSize(Size(WIN_W * 0.23, WIN_H));
        mus[i]->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        mus[i]->setPosition(Vec2(WIN_W * (0.016 + 0.246 * i), 0));
        this->addChild(mus[i]);
    }

    // ZXCV
    // KL;'
    // QWER
    // 7890
    const int keyMapping[4][4] = {
        {149, 147, 126, 145},
        {134, 135, 87, 67},
        {140, 146, 128, 141},
        {83, 84, 85, 76}
    };

    for (int i = 0; i < 4 && i < _gig.getMusicianCount(); i++)
        ((MusicianNKeys<4> *)_gig.getMusician(i))->setKeyMapping(keyMapping[i]);

    this->scheduleUpdate();
    _playState = 0;
}

void Gameplay::update(float dt)
{
    _gig.tick(dt);
}


bool Gameplay::ModPanel::init()
{
    if (!LayerColor::initWithColor(Color4B(192, 240, 255, 230))) return false;

    auto labelAutoplay = Label::createWithTTF("Auto Play", "OpenSans-Light.ttf", 42);
    labelAutoplay->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    labelAutoplay->setPosition(Vec2(24, WIN_H / 2 - 40));
    labelAutoplay->setColor(Color3B::BLACK);
    this->addChild(labelAutoplay);

    char s[4];

    for (int i = 0; i < 4; i++) {
        _autoplay[i] = false;

        snprintf(s, sizeof s, "P%d", i + 1);
        auto label = Label::createWithTTF(s, "OpenSans-Light.ttf", 42);
        label->setPosition(Vec2(WIN_W * (0.4 + 0.15 * i), WIN_H / 2 - 40));
        label->setColor(Color3B(128, 128, 128));
        this->addChild(label);
        _autoplayLabel[i] = label;
    }

    auto listener = cocos2d::EventListenerKeyboard::create();
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_TAB ||
            keyCode == EventKeyboard::KeyCode::KEY_ESCAPE ||
            keyCode == EventKeyboard::KeyCode::KEY_ENTER)
        {
            this->runAction(Sequence::createWithTwoActions(
                EaseQuadraticActionOut::create(
                    Spawn::createWithTwoActions(
                        MoveTo::create(0.2, Vec2(0, -WIN_H / 12)),
                        FadeOut::create(0.2)
                    )
                ),
                CallFunc::create([this]() { this->removeFromParent(); })
            ));
        }

        if (keyCode >= EventKeyboard::KeyCode::KEY_1 &&
            keyCode <= EventKeyboard::KeyCode::KEY_4)
        {
            int idx = (int)keyCode - (int)EventKeyboard::KeyCode::KEY_1;
            _autoplay[idx] ^= 1;
            _autoplayLabel[idx]->runAction(Spawn::createWithTwoActions(
                TintTo::create(
                    0.2,
                    _autoplay[idx] ? Color3B(32, 64, 255) : Color3B(128, 128, 128)
                ),
                EaseElasticOut::create(ScaleTo::create(
                    0.6,
                    _autoplay[idx] ? 1.1 : 1
                ), 0.2)
            ));
        }

        event->stopPropagation();
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void Gameplay::ModPanel::setNumMusicians(int num)
{
    for (int i = 0; i < 4; i++) _autoplayLabel[i]->setVisible(i < num);
}
