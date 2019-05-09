#ifndef __WanderingMelody__MusicianNKeys_h__
#define __WanderingMelody__MusicianNKeys_h__

#include "gig/Musician.h"

template <int N> class MusicianNKeys : public Musician {
public:
    MusicianNKeys() { }

    virtual void parseGrid(int32_t time, const char *grid) override;
    virtual int getWidth() override { return N; }

    void setKeyMapping(const int keyCodes[N]);

    virtual void startPlay() override;
    virtual void sendEvent(int message) override;

    class Display : public Musician::Display {
    public:
        Display() : _mus(nullptr), _drawNode(nullptr) { }
        virtual bool init(MusicianNKeys *mus);
        static Display *create(MusicianNKeys *mus);

        virtual void update(float dt) override;

    protected:
        cocos2d::DrawNode *_drawNode;
        MusicianNKeys *_mus;
    };

    virtual Musician::Display *createDisplay() override {
        return Display::create(this);
    }

protected:
    void triggerNote(uint8_t trackIdx);
    int _keyMapping[N];
    bool _isKeyDown[N];
};

#endif
