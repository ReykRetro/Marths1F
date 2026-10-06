#include "gbafe.h"

/*
 * ASMC: cuenta TODAS las unidades del grupo (desplegadas y no desplegadas),
 * excepto las muertas. Deja el resultado en el slot de memoria 0xC,
 * igual que los demas CHECK_ de eventos.
 *
 * Uso en eventos:
 *     ASMC PartyCount     (o CHECK_PARTY)
 *     // ahora el slot 0xC tiene el numero de unidades
 */

// Si pones 1, tambien se excluyen las unidades con el bit 16 de estado.
// Por defecto 0: solo se excluyen las muertas.
#define EXCLUDE_BIT16 0

#define EVENT_SLOTS ((volatile unsigned int*)0x030004B8)

void PartyCount(void)
{
    int i;
    unsigned int count = 0;

    for (i = 1; i < 0x40; i++)
    {
        struct Unit* unit = GetUnit(i);

        if (unit == 0 || unit->pCharacterData == 0)
            continue; // slot vacio

        if (unit->state & US_DEAD)
            continue; // muerta

#if EXCLUDE_BIT16
        if (unit->state & (1 << 16))
            continue;
#endif

        count++;
    }

    EVENT_SLOTS[0xC] = count;
}