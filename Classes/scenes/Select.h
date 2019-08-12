#ifndef __WanderingMelody__Select_h__
#define __WanderingMelody__Select_h__

#include "cocos2d.h"
#include "Global.h"

#include <string>
#include <unordered_map>
#include <vector>

class Select : public cocos2d::LayerColor
{
public:
    bool init() override;
    CREATE_FUNC(Select);
    SCENE_FUNC(Select);

protected:
    struct SongListEntry {
        std::string path;
        std::string title;
        std::unordered_map<std::string, std::string> metadata;
        bool succeeded;
        cocos2d::LayerColor *display;
    };

    std::vector<SongListEntry> _songList;
    cocos2d::Node *_songListContainer;

    int _selIdx;

    void updateSongListPos();
};

#endif
