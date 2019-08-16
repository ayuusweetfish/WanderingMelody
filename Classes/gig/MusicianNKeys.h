#ifndef __WanderingMelody__MusicianNKeys_h__
#define __WanderingMelody__MusicianNKeys_h__

#include "gig/Musician.h"

template <int N> class MusicianNKeys : public Musician {
public:
    MusicianNKeys() {
        for (int i = 0; i < N; i++) _isKeyDown[i] = false;
    }

    virtual void parseGrid(int32_t time, const char *grid) override;
    virtual int getWidth() override { return N; }

    void setKeyMapping(const int keyCodes[N]);

    virtual void stopPlay() override;
    virtual void sendEvent(int message) override;

    class Display : public Musician::Display {
    public:
        Display() : _mus(nullptr), _drawNode(nullptr) { }
        virtual bool init(MusicianNKeys *mus);
        static Display *create(MusicianNKeys *mus);

        virtual void refresh() override;

    protected:
        MusicianNKeys *_mus;

        cocos2d::DrawNode *_drawNode;
        std::vector<cocos2d::Label *> _labels;
    };

    virtual Musician::Display *createDisplay() override {
        return (_display = Display::create(this));
    }

protected:
    void triggerNote(uint8_t trackIdx);
    int _keyMapping[N];
    bool _isKeyDown[N];
};

#endif
