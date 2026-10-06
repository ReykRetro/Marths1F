#include "gbafe.h"

/*
 * ASMC: cuenta las unidades del jugador que estan EN EL MAPA ahora mismo:
 * vivas y desplegadas. Es la contraparte de CHECK_ENEMIES / CHECK_OTHERS.
 * Deja el resultado en el slot de memoria 0xC (igual que ellos).
 *
 * Uso en eventos:
 *     CHECK_PLAYERS        (o ASMC PlayerCount)
 *     // ahora el slot 0xC tiene el numero de unidades en el mapa
 *
 * Para contar el grupo completo (incluidas las no desplegadas) usa
 * CHECK_PARTY (PartyCount).
 */

// Si pones 1, tambien se excluyen las unidades ocultas (estado "hidden").
// Por defecto 0: las ocultas cuentan, porque siguen en el mapa.
#define EXCLUDE_HIDDEN 0

// Bits de estado de la unidad (FE8). Se definen aqui con nombre propio para
// no depender de que tu gbafe.h los llame igual o los tenga.
#define PC_STATE_HIDDEN       0x00000001
#define PC_STATE_DEAD         0x00000004
#define PC_STATE_NOT_DEPLOYED 0x00000008

// gEventSlot de FE8U. Slot 0xC = valor de retorno de los CHECK_*.
#define EVENT_SLOTS ((volatile unsigned int*)0x030004B8)

void PlayerCount(void)
{
    int i;
    unsigned int count = 0;

    // Las unidades del jugador ocupan los IDs 1 .. 0x3F.
    for (i = 1; i < 0x40; i++)
    {
        struct Unit* unit = GetUnit(i);

        if (unit == 0 || unit->pCharacterData == 0)
            continue; // slot vacio

        if (unit->state & PC_STATE_DEAD)
            continue; // muerta

        if (unit->state & PC_STATE_NOT_DEPLOYED)
            continue; // guardada, no esta en el mapa

#if EXCLUDE_HIDDEN
        if (unit->state & PC_STATE_HIDDEN)
            continue;
#endif

        count++; // viva y en el mapa (las rescatadas tambien cuentan)
    }

    EVENT_SLOTS[0xC] = count;
}