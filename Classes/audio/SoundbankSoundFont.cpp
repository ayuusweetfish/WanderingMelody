#include "audio/SoundbankSoundFont.h"

void SoundbankSoundFont::sendNote(const MusicNote &note)
{
    printf("trigger %d\n", (int)note.note);
}
