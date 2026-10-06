#include "gbafe.h"

int GetUnitPortraitId_Hook(struct Unit* unit) {
    if (unit->pCharacterData->portraitId) {
        int id;

        if (gPlaySt.chapterIndex == 0x22 && unit->pCharacterData->portraitId == 0x4A)
            id = 0x46;
        else
            id = unit->pCharacterData->portraitId;

        if (unit->state & 0x00800000)
            id++;

        return id;
    }

    if (unit->pClassData->defaultPortraitId)
        return unit->pClassData->defaultPortraitId;

    return 0;
}