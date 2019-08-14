#include "Gameplay.h"
#include "Global.h"

#include "gig/Musician.h"
#include "gig/MusicianNKeys.h"
#include "scenes/widgets/KeyHintLabel.h"
using namespace cocos2d;

#include <atomic>
#include <chrono>

class Gameplay::ModPanel : public cocos2d::LayerColor
{
public:
    virtual bool init() override;
    CREATE_FUNC(ModPanel);

    enum SpeedMode {
        FREESTYLE = 0,
        COOPERATE = 1,
        CONDUCTOR = 2,
        METRONOME = 10,
        PAST_THE_END
    };

    static const char *getSpeedModeName(SpeedMode mode);
    static Color3B getSpeedModeColor(SpeedMode mode);

    void setNumMusicians(int num);
    inline bool isAutoplay(int i) { return _autoplay[i]; }
    inline SpeedMode getSpeedMode() { return _speedMode; }

protected:
    int _numMusicians;

    bool _autoplay[4];
    cocos2d::Label *_autoplayLabel[4];

    SpeedMode _speedMode;
    cocos2d::Label *_speedModeLabel;
};

bool Gameplay::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255))) return false;

    _playState = -1;
    _gig = new Gig();

    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        if (_playState == 0 && keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
            _playState = 1;
            _layerStart->runAction(Sequence::createWithTwoActions(
                EaseQuadraticActionIn::create(
                    MoveBy::create(0.3, Vec2(0, WIN_H / 10))
                ),
                CallFunc::create([this] () {
                    _layerStart->removeFromParent();

                    for (int i = 0; i < _gig->getMusicianCount(); i++)
                        _gig->getMusician(i)->setIsAutoplay(_modPanel->isAutoplay(i));

                    ModPanel::SpeedMode mode = _modPanel->getSpeedMode();
                    if (mode == ModPanel::COOPERATE) {
                        for (int i = 0; i < _gig->getMusicianCount(); i++)
                            _gig->getMusician(i)->setIsCooperative(true);
                    } if (mode >= ModPanel::CONDUCTOR && mode < ModPanel::CONDUCTOR + 4) {
                        for (int i = 0; i < _gig->getMusicianCount(); i++)
                            _gig->getMusician(i)->setIsAutoscroll(i != (int)mode - ModPanel::CONDUCTOR);
                    } else if (mode == ModPanel::METRONOME) {
                        for (int i = 0; i < _gig->getMusicianCount(); i++)
                            _gig->getMusician(i)->setIsAutoscroll(true);
                    }

                    _gig->startPlay();
                })
            ));
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
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    auto controllerListener = EventListenerController::create();
    controllerListener->onConnected = [](Controller *controller, Event *event) {
        printf("connected %s\n", controller->getDeviceName().c_str());
    };
    controllerListener->onKeyDown = [](Controller *controller, int keyCode, Event *event) {
        printf("down %d\n", keyCode);
    };
    controllerListener->onKeyUp = [](Controller *controller, int keyCode, Event *event) {
        printf("up %d\n", keyCode);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(controllerListener, this);

    return true;
}

Gameplay::~Gameplay()
{
    if (_modPanel) _modPanel->release();
    std::lock_guard<std::mutex> guard(*_gigMutex);
    _gigStopSignal->store(true);
}

void Gameplay::load(const std::string &path)
{
    Gig::FileReadResult r = _gig->init(path);
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

    auto layerStart = LayerColor::create(Color4B(240, 235, 230, 192), WIN_W, WIN_H / 10);
    layerStart->setPosition(Vec2(0, WIN_H * 9 / 10));
    this->addChild(layerStart, 9998);
    _layerStart = layerStart;

    auto labelStart = KeyHintLabel::create(Color4B(240, 235, 230, 192), {
        {Config::getKeyConfirm(0), "Start"},
        {Config::getKeyConfirm(0), "Edit/Rehearse"}
    });
    //Label::createWithTTF("Press Enter", "OpenSans-Light.ttf", 42);
    labelStart->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    labelStart->setPosition(Size(WIN_W, WIN_H / 10) / 2);
    labelStart->setColor(Color3B(64, 64, 64));
    layerStart->addChild(labelStart);
    _labelStart = labelStart;

    int numMusicians = _gig->getMusicianCount();

    auto modPanel = ModPanel::create();
    modPanel->setNumMusicians(numMusicians);
    modPanel->setContentSize(Size(WIN_W, WIN_H / 2));
    modPanel->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    modPanel->setPosition(Vec2(0, -WIN_H / 12));
    modPanel->setOpacity(0);
    modPanel->setCascadeOpacityEnabled(true);
    modPanel->retain();
    _modPanel = modPanel;

    Musician::Display *mus[4];
    for (int i = 0; i < numMusicians; i++) {
        mus[i] = _gig->getMusician(i)->createDisplay();
        mus[i]->setContentSize(Size(WIN_W * 0.23, WIN_H));
        mus[i]->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        mus[i]->setPosition(Vec2(WIN_W * (0.016 + 0.246 * i), 0));
        this->addChild(mus[i]);
    }

    int keyMapping[4];
    for (int i = 0; i < 4 && i < _gig->getMusicianCount(); i++) {
        keyMapping[0] = Config::getKey4K_Key1(i);
        keyMapping[1] = Config::getKey4K_Key2(i);
        keyMapping[2] = Config::getKey4K_Key3(i);
        keyMapping[3] = Config::getKey4K_Key4(i);
        ((MusicianNKeys<4> *)_gig->getMusician(i))->setKeyMapping(keyMapping);
    }

    this->scheduleUpdate();
    _playState = 0;

    _gigMutex = new std::mutex();
    _gigStopSignal = new std::atomic<bool>(false);
    auto mutex = _gigMutex;
    auto signal = _gigStopSignal;
    auto gig = _gig;
    auto gigUpdateThread = std::thread([mutex, signal, gig] {
        std::chrono::steady_clock clock;
        auto last = clock.now();
        while (!signal->load()) {
            auto now = clock.now();
            auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last);
            int64_t dtNanos = dt.count();
            std::lock_guard<std::mutex> guard(*mutex);
            gig->tick((double)dtNanos / 1e9);
            /*if (gig->getMusician(0)->getCurTick() >= 240 * 32) {
                gig->jump(240 * 16);
                gig->clearTriggered();
            }*/
            last = now;
            std::this_thread::sleep_for(
                std::chrono::milliseconds(Config::getPlaybackResolution())
            );
        }
        delete mutex;
        delete signal;
        delete gig;
    });
    gigUpdateThread.detach();
}

void Gameplay::update(float dt)
{
    _gig->refresh();
}


bool Gameplay::ModPanel::init()
{
    if (!LayerColor::initWithColor(Color4B(192, 240, 255, 230))) return false;

    _numMusicians = 4;

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

    auto labelSpeedMode = Label::createWithTTF("Speed Mode", "OpenSans-Light.ttf", 42);
    labelSpeedMode->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    labelSpeedMode->setPosition(Vec2(24, WIN_H / 2 - 100));
    labelSpeedMode->setColor(Color3B::BLACK);
    this->addChild(labelSpeedMode);

    _speedMode = FREESTYLE;
    auto label = Label::createWithTTF(
        ModPanel::getSpeedModeName(_speedMode), "OpenSans-Light.ttf", 42);
    label->setPosition(Vec2(WIN_W * 0.667, WIN_H / 2 - 100));
    label->setColor(Color3B(128, 128, 128));
    this->addChild(label);
    _speedModeLabel = label;

    auto listener = EventListenerKeyboard::create();
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

        if (keyCode == EventKeyboard::KeyCode::KEY_Q) {
            _speedMode = (SpeedMode)((int)_speedMode + 1);
            if (_speedMode == CONDUCTOR + _numMusicians)
                _speedMode = METRONOME;
            if (_speedMode == PAST_THE_END)
                _speedMode = FREESTYLE;
            _speedModeLabel->setString(getSpeedModeName(_speedMode));
            _speedModeLabel->runAction(Spawn::createWithTwoActions(
                TintTo::create(0.2, getSpeedModeColor(_speedMode)),
                Sequence::createWithTwoActions(
                    ScaleTo::create(0, 0.91),
                    EaseElasticOut::create(ScaleTo::create(0.5, 1), 0.3)
                )
            ));
        }

        event->stopPropagation();
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

const char *Gameplay::ModPanel::getSpeedModeName(SpeedMode mode)
{
    static char s[16];
    if (mode == FREESTYLE) return "Freestyle";
    if (mode == COOPERATE) return "Co-operative";
    if (mode >= CONDUCTOR && mode < CONDUCTOR + 4) {
        sprintf(s, "Conductor P%d", (int)mode - CONDUCTOR + 1);
        return s;
    }
    if (mode == METRONOME) return "Metronome";
    return nullptr;
}

Color3B Gameplay::ModPanel::getSpeedModeColor(SpeedMode mode)
{
    if (mode == FREESTYLE) return Color3B(128, 128, 128);
    if (mode == COOPERATE) return Color3B(64, 192, 128);
    if (mode >= CONDUCTOR && mode < CONDUCTOR + 4) return Color3B(255, 128, 128);
    if (mode == METRONOME) return Color3B(128, 128, 255);
    return Color3B(128, 128, 128);
}

void Gameplay::ModPanel::setNumMusicians(int num)
{
    _numMusicians = num;
    for (int i = 0; i < 4; i++) _autoplayLabel[i]->setVisible(i < num);
}
