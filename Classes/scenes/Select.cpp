#include "Select.h"
#include "Global.h"
#include "gig/Gig.h"
using namespace cocos2d;

template <typename K1, typename V1, typename K2, typename V2>
static inline V1 findOrDefault(
    const std::unordered_map<K1, V1> &map, const K2 &key, const V2 &dfault
) {
    auto it = map.find(key);
    return (it == map.end() ? dfault : it->second);
}

bool Select::init()
{
    if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255))) return false;

    _songListContainer = Node::create();
    _songListContainer->setPosition(Vec2(0, WIN_H - WIN_H / 5));
    this->addChild(_songListContainer);

    auto files = FileUtils::getInstance()->listFiles("songs");

    // XXX: This includes ./ and ../, though it should not be much of a problem
    for (const std::string &file : files) {
        if (FileUtils::getInstance()->isDirectoryExist(file) &&
            FileUtils::getInstance()->isFileExist(file + "map.txt"))
        {
            Gig gig;
            Gig::FileReadResult result = gig.peek(file + "map.txt");

            auto display = LayerColor::create(
                Color4B(240, 235, 230, 192), WIN_W, WIN_H / 5);
            display->setPosition(Vec2(0, -WIN_H / 5 * (int)_songList.size()));
            _songListContainer->addChild(display);

            auto labelTitle = Label::createWithTTF(
                findOrDefault(gig.getMetadata(), "Title", "<Unknown>"),
                "fonts/arial.ttf", 42
            );
            labelTitle->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
            labelTitle->setPosition(Vec2(24, WIN_H * 13 / 100));
            labelTitle->setColor(Color3B(0, 0, 0));
            display->addChild(labelTitle);

            auto labelArtist = Label::createWithTTF(
                findOrDefault(gig.getMetadata(), "Artist", "<Unknown>"),
                "fonts/arial.ttf", 28
            );
            labelArtist->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
            labelArtist->setPosition(Vec2(24, WIN_H * 6 / 100));
            labelArtist->setColor(Color3B(128, 128, 128));
            display->addChild(labelArtist);

            _songList.push_back((SongListEntry){
                file, gig.getMetadata(), result.succeeded, display
            });
        }
    }

    // Fade in on startup
    auto cover = LayerColor::create(Color4B::BLACK);
    this->addChild(cover, INT32_MAX);
    cover->runAction(FadeOut::create(0.15f));

    return true;
}
