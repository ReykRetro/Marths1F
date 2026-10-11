/*
 * AumPick.c  --  El Aum como item con eleccion de a quien revivir.
 *
 * Al usar el bastón se abre una lista con los NOMBRES de los aliados caidos (tambien los
 * de capitulos anteriores), con scroll arriba/abajo:
 *      A  revive al elegido (misma logica que el Aum de SME: HP al maximo, casilla libre mas
 *         cercana a la unidad activa) y sigue con la experiencia, la animacion del baston y
 *         el gasto de un uso.
 *      B  cancela: no se revive a nadie y no se gasta ningun uso.
 *
 * Exporta (para la tabla de items, Item_Effect_Revamp):
 *      AumUsability   solo es usable si hay algun caido (copia del Aum de SME)
 *      AumEffect      entrada del efecto: llama a AumItemStart y sale por GoBackLoc (SME)
 *      FindDeadUnit   auxiliar de AumUsability (SME)
 *
 * Tambien, por si se quiere usar desde eventos (sin item):
 *      ASMC AumPickCountDead   slot 0xC = cuantos caidos hay
 *      ASMC AumPickStart       abre la lista; slot 0xC = 0 (B / nadie) o 1..62 = el elegido
 *                              (indice en el array de jugador + 1, el "Unit ID" del depurador)
 *
 * Creditos:
 *      Aum original (FindDeadUnit, AumUsability, AumEffect): SME.
 *      Esquema de la pantalla: Character Biographies de JesterWizard (MIT).
 *      Arranque bloqueante de la pantalla: Reclass de Vesly.
 *
 * Notas de implementacion:
 *      - No incluye gbafe.h: solo usa las direcciones de Definitions.s.
 *      - Sin globales (.bss) ni divisiones: lyn no resuelve __aeabi_uidiv.
 *      - En modo item NO se llama a LockGame: OnMain solo corre el arbol de procs 2 (donde
 *        cuelga PlayerPhase) con el bloqueo a cero, y el proc se congelaria a si mismo.
 */

typedef unsigned char  u8;
typedef signed char    s8;
typedef unsigned short u16;
typedef short          s16;
typedef unsigned int   u32;

struct Text
{
    u16 chr_position;
    u8  x;
    u8  colorId;
    u8  tile_width;
    s8  db_enabled;
    u8  db_id;
    u8  is_printing;
};

/* Comando de proc: { opcode, dataImm, dataPtr }. Opcodes del decomp de FE8:
 * 0x00 END, 0x01 NAME, 0x02 CALL, 0x03 REPEAT, 0x0E SLEEP,
 * 0x14 WHILE, 0x18 CALL_ARG (llama func(dataImm, proc)). */
struct ProcCmd
{
    short       opcode;
    short       dataImm;
    const void* dataPtr;
};

extern void* Proc_StartBlocking(const struct ProcCmd* script, void* parent);
extern void  Proc_Break(void* proc);

extern void LockGame(void);
extern void UnlockGame(void);
extern void NewFadeOut(int speed, void* parent);
extern void NewFadeIn(int speed, void* parent);
extern int  FadeOutExists(void);
extern int  FadeInExists(void);

extern void BMapDispSuspend(void);
extern void BMapDispResume(void);
extern void EndPlayerPhaseSideWindows(void);
extern void ClearBg0Bg1(void);
extern void SetupBackgrounds(u16* bgConfig);
extern void BG_Fill(void* dest, int value);
extern u16* BG_GetMapBuffer(int bg);
extern void BG_EnableSyncByMask(int mask);
extern void EndGreenText(void);
extern void ResetText(void);
extern void LoadUiFrameGraphics(void);
extern void LoadObjUIGfx(void);
extern void LoadIconPalettes(int n);
extern void DrawUiFrame2(int x, int y, int w, int h, int style);

extern void ResetSysHandCursor(void* parent);
extern void ShowSysHandCursor(int x, int y, int a, int b);
extern void HideSysHandCursor(void);

extern void        InitText(struct Text* th, int tileWidth);
extern void        ClearText(struct Text* th);
extern void        PutDrawText(struct Text* th, u16* dest, int colorId, int x, int tileWidth, const char* str);
extern const char* GetStringFromIndex(int msgId);

extern void ResetUnitSprites(void);
extern void RefreshBMapGraphics(void);
extern void SyncUnitSpriteSheet(void);
extern void RefreshUnitSprites(void);
extern void InitPlayerPhaseInterface(void);
extern void SetAllUnitNotBackSprite(void);
extern void ResetUnitSpriteHover(void);

extern void HideMoveRangeGraphicsWrapper(void);
extern void BmMapFill(u8** map, int value);
extern int  GetRescueStaffeePosition(void* actor, void* target, int* outX, int* outY);
extern void BattleInitItemEffect(void* actor, int slot);
extern void BattleInitItemEffectTarget(void* target);
extern void BattleApplyItemEffect(void* proc);
extern void BeginBattleAnimations(void);
extern void UnitUpdateUsedItem(void* unit, int slot);

extern u8   gActionData[];        /* 0x0203A958: itemSlotIndex en +0x12                 */
extern u8*  gActiveUnit;          /* 0x03004E50: puntero a la unidad activa             */
extern u8** gBmMapRange;          /* 0x0202E4E4                                         */
extern u8** gBmMapMovement;       /* 0x0202E4E0                                         */

extern u8   gStatScreen[];        /* 0x02003BFC: su array de Text (+0x18) se reutiliza */
extern u8*  gKeyStatusPtr;        /* puntero a los datos de teclas (newKeys en +0x08)   */
extern u32  gEventSlots[];        /* 0x030004B8                                         */
extern u8   gUnitArrayBlue[];     /* 0x0202BE4C                                         */
extern u16  gBG0TilemapBuffer[];  /* 0x02022CA8, 32x32 entradas                         */

/* ------------------------------------------------------------------------ */

#define UNIT_SIZE    0x48
#define BLUE_SLOTS   62          /* el array de unidades de jugador tiene 62 slots */
#define US_DEAD      0x00000004

#define VISIBLE      5           /* filas visibles de la lista */

#define KEY_A        0x0001
#define KEY_B        0x0002
#define KEY_UP       0x0040
#define KEY_DOWN     0x0080

#define TEXT_WHITE   0
#define TEXT_GOLD    3

#define TEXT(i)      ((struct Text*)(gStatScreen + 0x18 + (i) * 8))
#define ROW_TEXT(i)  TEXT(2 + (i))
#define TM(x, y)     (gBG0TilemapBuffer + (y) * 32 + (x))

/* Datos propios del proc: a partir de +0x2C, como en el resto de procs. */
#define P_CUR(p)     (*(s16*)((u8*)(p) + 0x2C))
#define P_TOP(p)     (*(s16*)((u8*)(p) + 0x2E))
#define P_COUNT(p)   (*(s16*)((u8*)(p) + 0x30))
#define P_MODE(p)    (*(s16*)((u8*)(p) + 0x32))   /* 0 = ASMC de prueba, 1 = efecto del item */
#define P_PICK(p)    (*(s16*)((u8*)(p) + 0x34))   /* slot elegido + 1 (0 = nadie)            */
#define P_PARENT(p)  (*(void**)((u8*)(p) + 0x38)) /* proc de la accion que espera            */

#define MODE_ASMC    0
#define MODE_ITEM    1

#define US_HIDDEN    0x00000001

/* ID del texto del titulo ("Revive who?"). Vale 0 en el codigo; Installer.event lo rellena con
 * el ID que text-process le asigna a "AumPickReviveText" (Text/Items_text.txt). Si fuera 0,
 * se usa el texto de reserva de abajo. Se lee con volatile para que el compilador no lo
 * sustituya por la constante 0. */
const u16 AumPickTextId __attribute__((used)) = 0;

#define TITLE_FALLBACK "Revive who?"

static const char* GetTitle(void)
{
    u16 id = *(volatile const u16*)&AumPickTextId;

    return id ? GetStringFromIndex(id) : TITLE_FALLBACK;
}

/* ------------------------------------------------------------------------ */
/* Datos: buscar caidos en el array de unidades de jugador                  */
/* ------------------------------------------------------------------------ */

static int IsDead(int slot)
{
    u8* unit = gUnitArrayBlue + slot * UNIT_SIZE;

    if (*(u32*)(unit + 0x00) == 0)          /* slot vacio (sin pCharacterData) */
        return 0;

    return (*(u32*)(unit + 0x0C) & US_DEAD) != 0;
}

static int CountDead(void)
{
    int i, n = 0;

    for (i = 0; i < BLUE_SLOTS; i++)
        if (IsDead(i))
            n++;

    return n;
}

/* Devuelve el slot (0..61) del n-esimo caido, o -1 si no existe. */
static int NthDeadSlot(int n)
{
    int i;

    for (i = 0; i < BLUE_SLOTS; i++)
    {
        if (!IsDead(i))
            continue;

        if (n == 0)
            return i;

        n--;
    }

    return -1;
}

/* ------------------------------------------------------------------------ */
/* Dibujo                                                                    */
/* ------------------------------------------------------------------------ */

/* Escribe un numero (0..99) sin usar divisiones: el compilador convertia un bucle
 * de restas en una llamada a __aeabi_uidiv, que lyn no sabe resolver. */
static char* PutNum(char* p, int v)
{
    int tens = (v >= 10) + (v >= 20) + (v >= 30) + (v >= 40) + (v >= 50)
             + (v >= 60) + (v >= 70) + (v >= 80) + (v >= 90);

    v -= tens * 10;

    if (tens)
        *p++ = (char)('0' + tens);

    *p++ = (char)('0' + v);
    return p;
}

/* Revive al caido del slot dado, igual que el Aum original de SME: quita los estados
 * "muerto" y "oculto", le pone la vida al maximo y lo coloca en la casilla libre mas
 * cercana a la unidad activa. */
static void ReviveSlot(int slot)
{
    u8* unit = gUnitArrayBlue + slot * UNIT_SIZE;
    int x = 0, y = 0;

    *(u32*)(unit + 0x0C) &= ~(US_DEAD | US_HIDDEN);
    unit[0x13] = unit[0x12];                       /* HP actual = HP maximo */

    GetRescueStaffeePosition(gActiveUnit, unit, &x, &y);

    unit[0x10] = (u8)x;
    unit[0x11] = (u8)y;
}

static void DrawList(void* proc)
{
    int  i;
    int  top   = P_TOP(proc);
    int  cur   = P_CUR(proc);
    int  count = P_COUNT(proc);
    char buf[12];
    char* p;

    for (i = 0; i < VISIBLE; i++)
    {
        struct Text* th = ROW_TEXT(i);
        int slot;

        ClearText(th);

        if (top + i >= count)
            continue;

        slot = NthDeadSlot(top + i);

        if (slot < 0)
            continue;

        {
            u8*  unit   = gUnitArrayBlue + slot * UNIT_SIZE;
            u8*  chara  = (u8*)*(u32*)(unit + 0x00);      /* pCharacterData */
            int  nameId = *(u16*)chara;                    /* nameTextId     */

            PutDrawText(th, TM(8, 8 + i * 2), TEXT_WHITE, 0, 0, GetStringFromIndex(nameId));
        }
    }

    /* "n/total" arriba a la derecha */
    p = buf;
    p = PutNum(p, cur + 1);
    *p++ = '/';
    p = PutNum(p, count);
    *p = 0;

    ClearText(TEXT(1));
    PutDrawText(TEXT(1), TM(19, 5), TEXT_GOLD, 0, 0, buf);

    BG_EnableSyncByMask(1);
}

/* ------------------------------------------------------------------------ */
/* Pasos del proc                                                            */
/* ------------------------------------------------------------------------ */

static void AumPick_Begin(void* proc)
{
    LockGame();
}

/* Modo item: NO se bloquea el juego. OnMain solo corre el arbol de procs 2 (donde cuelga
 * PlayerPhase, y por tanto este proc) cuando GetGameLock() == 0; si lo bloqueamos, el
 * proc se congelaria a si mismo. El padre ya queda detenido por Proc_StartBlocking. */
static void AumPick_BeginItem(void* proc)
{
}

static void AumPick_Suspend(void* proc)
{
    BMapDispSuspend();
}

static void AumPick_Init(void* proc)
{
    int i;

    P_CUR(proc)   = 0;
    P_TOP(proc)   = 0;
    P_COUNT(proc) = (s16)CountDead();

    EndPlayerPhaseSideWindows();

    ClearBg0Bg1();
    SetupBackgrounds(0);

    BG_Fill(BG_GetMapBuffer(0), 0);
    BG_Fill(BG_GetMapBuffer(1), 0);
    BG_Fill(BG_GetMapBuffer(2), 0);
    BG_Fill(BG_GetMapBuffer(3), 0);

    /* vaciar los sprites de unidades del mapa ANTES de pisar la VRAM de OBJ */
    ResetUnitSprites();

    EndGreenText();
    ResetText();
    LoadUiFrameGraphics();
    LoadObjUIGfx();
    LoadIconPalettes(4);

    DrawUiFrame2(6, 7, 18, 12, 0);
    BG_EnableSyncByMask(7);

    ResetSysHandCursor(proc);
    ShowSysHandCursor(56, 64, 0, 0x800);

    InitText(TEXT(0), 12);
    InitText(TEXT(1), 4);

    for (i = 0; i < VISIBLE; i++)
        InitText(ROW_TEXT(i), 14);

    PutDrawText(TEXT(0), TM(6, 5), TEXT_WHITE, 0, 0, GetTitle());

    DrawList(proc);
    BG_EnableSyncByMask(7);
}

static void AumPick_Loop(void* proc)
{
    u16 keys  = *(u16*)(gKeyStatusPtr + 0x08);      /* newKeys */
    int cur   = P_CUR(proc);
    int top   = P_TOP(proc);
    int count = P_COUNT(proc);
    int moved = 0;

    if (keys & KEY_A)
    {
        int slot = NthDeadSlot(cur);

        if (P_MODE(proc) == MODE_ITEM)
        {
            if (slot >= 0)
            {
                ReviveSlot(slot);           /* antes de redibujar el mapa en Restore */
                P_PICK(proc) = (s16)(slot + 1);
            }
        }
        else
        {
            gEventSlots[0xC] = (slot >= 0) ? (u32)(slot + 1) : 0;
        }

        Proc_Break(proc);
        return;
    }

    if (keys & KEY_B)
    {
        if (P_MODE(proc) != MODE_ITEM)
            gEventSlots[0xC] = 0;

        P_PICK(proc) = 0;
        Proc_Break(proc);
        return;
    }

    if ((keys & KEY_UP) && cur > 0)
    {
        cur--;

        if (cur < top)
            top--;

        moved = 1;
    }

    if ((keys & KEY_DOWN) && cur < count - 1)
    {
        cur++;

        if (cur >= top + VISIBLE)
            top++;

        moved = 1;
    }

    if (moved)
    {
        P_CUR(proc) = (s16)cur;
        P_TOP(proc) = (s16)top;

        ShowSysHandCursor(56, 64 + (cur - top) * 16, 0, 0x800);
        DrawList(proc);
    }
}

static void AumPick_Restore(void* proc)
{
    HideSysHandCursor();

    ResetUnitSprites();
    RefreshBMapGraphics();
    SyncUnitSpriteSheet();
    RefreshUnitSprites();

    if (P_MODE(proc) != MODE_ITEM)          /* en medio de una accion no se reabre */
        InitPlayerPhaseInterface();

    SetAllUnitNotBackSprite();
    ResetUnitSpriteHover();

    BMapDispResume();

    if (P_MODE(proc) != MODE_ITEM)          /* en modo item nunca se bloqueo */
        UnlockGame();
}

/* Solo en modo item: tras volver al mapa, lo mismo que hacia el final de AumEffect de SME
 * (experiencia, animacion del baston y gasto de un uso). Si se cancelo, no hace nada. */
static void AumPick_Finish(void* proc)
{
    int slot;
    int itemSlot;

    if (P_MODE(proc) != MODE_ITEM || P_PICK(proc) == 0)
        return;

    slot     = P_PICK(proc) - 1;
    itemSlot = gActionData[0x12];

    BattleInitItemEffect(gActiveUnit, itemSlot);
    BattleInitItemEffectTarget(gUnitArrayBlue + slot * UNIT_SIZE);
    BattleApplyItemEffect(P_PARENT(proc));
    BeginBattleAnimations();

    UnitUpdateUsedItem(gActiveUnit, itemSlot);
}

/* ------------------------------------------------------------------------ */
/* Script del proc                                                           */
/* ------------------------------------------------------------------------ */

static const struct ProcCmd ProcScr_AumPick[] =
{
    { 0x01, 0,  "AumPick" },              /* PROC_NAME                         */
    { 0x02, 0,  AumPick_Begin },          /* PROC_CALL: LockGame               */
    { 0x18, 16, NewFadeOut },             /* PROC_CALL_ARG(NewFadeOut, 16)     */
    { 0x14, 0,  FadeOutExists },          /* PROC_WHILE(FadeOutExists)         */
    { 0x02, 0,  AumPick_Suspend },        /* PROC_CALL: BMapDispSuspend        */
    { 0x0E, 0,  0 },                      /* PROC_YIELD                        */
    { 0x02, 0,  AumPick_Init },           /* PROC_CALL: arma la pantalla       */
    { 0x18, 16, NewFadeIn },
    { 0x14, 0,  FadeInExists },
    { 0x03, 0,  AumPick_Loop },           /* PROC_REPEAT: hasta A o B          */
    { 0x18, 16, NewFadeOut },
    { 0x14, 0,  FadeOutExists },
    { 0x02, 0,  AumPick_Restore },        /* devuelve el mapa                  */
    { 0x18, 16, NewFadeIn },
    { 0x14, 0,  FadeInExists },
    { 0x02, 0,  AumPick_Finish },         /* PROC_CALL: efecto del item        */
    { 0x00, 0,  0 },                      /* PROC_END                          */
};

/* Igual que el anterior pero sin LockGame (uso como objeto, ver AumPick_BeginItem). */
static const struct ProcCmd ProcScr_AumPickItem[] =
{
    { 0x01, 0,  "AumPick" },              /* PROC_NAME                         */
    { 0x02, 0,  AumPick_BeginItem },      /* PROC_CALL: sin LockGame           */
    { 0x18, 16, NewFadeOut },             /* PROC_CALL_ARG(NewFadeOut, 16)     */
    { 0x14, 0,  FadeOutExists },          /* PROC_WHILE(FadeOutExists)         */
    { 0x02, 0,  AumPick_Suspend },        /* PROC_CALL: BMapDispSuspend        */
    { 0x0E, 0,  0 },                      /* PROC_YIELD                        */
    { 0x02, 0,  AumPick_Init },           /* PROC_CALL: arma la pantalla       */
    { 0x18, 16, NewFadeIn },
    { 0x14, 0,  FadeInExists },
    { 0x03, 0,  AumPick_Loop },           /* PROC_REPEAT: hasta A o B          */
    { 0x18, 16, NewFadeOut },
    { 0x14, 0,  FadeOutExists },
    { 0x02, 0,  AumPick_Restore },        /* devuelve el mapa                  */
    { 0x18, 16, NewFadeIn },
    { 0x14, 0,  FadeInExists },
    { 0x02, 0,  AumPick_Finish },         /* PROC_CALL: efecto del item        */
    { 0x00, 0,  0 },                      /* PROC_END                          */
};

/* ------------------------------------------------------------------------ */
/* Puntos de entrada (ASMC)                                                  */
/* ------------------------------------------------------------------------ */

void AumPickCountDead(void* parent)
{
    gEventSlots[0xC] = (u32)CountDead();
}

void AumPickStart(void* parent)
{
    if (CountDead() == 0)
    {
        gEventSlots[0xC] = 0;
        return;
    }

    Proc_StartBlocking(ProcScr_AumPick, parent);
}

/* ------------------------------------------------------------------------ */
/* Aum como ITEM: efecto + usabilidad                                        */
/* ------------------------------------------------------------------------ */

/* Lo llama AumEffect (abajo). Sustituye al "FindDeadUnit + revivir" de SME:
 * abre la lista dentro de la accion del item; al terminar, revive al elegido y
 * lanza la experiencia/animacion/gasto de uso (AumPick_Finish).
 *   parent = proc de la accion (r6 al entrar a AumEffect)
 *   action = gActionData (r4), no se usa: se lee gActionData directamente.        */
__attribute__((used, noinline))
void AumItemStart(void* parent, void* action)
{
    void* p;

    if (CountDead() == 0)
        return;

    /* igual que SME: limpiar mapas de rango/movimiento y quitar el rango de pantalla */
    BmMapFill(gBmMapRange, 0);
    BmMapFill(gBmMapMovement, 0xFF);
    HideMoveRangeGraphicsWrapper();

    p = Proc_StartBlocking(ProcScr_AumPickItem, parent);

    P_MODE(p)   = MODE_ITEM;
    P_PICK(p)   = 0;
    P_PARENT(p) = parent;

}

/* El GCC de devkitARM genera Thumb con sintaxis CLASICA (and/add/mov); clang solo acepta
 * la UNIFICADA (ands/adds). S(unificada, clasica) elige segun el compilador. */
#ifdef __clang__
#  define S(u, d) u
#else
#  define S(u, d) d
#endif

/* AumUsability y FindDeadUnit: copia literal del Aum de SME (con la sintaxis clasica de SME).
 * AumEffect conserva su entrada/salida original: el motor lo llama con r4 = accion,
 * r6 = proc, y termina saltando a GoBackLoc (0x0802FF77). */
__asm__(
    ".thumb\n"
    ".align 2\n"

    ".global FindDeadUnit\n"
    ".type FindDeadUnit, %function\n"
    ".thumb_func\n"
    "FindDeadUnit:\n"
    "    push {r4-r7, lr}\n"
    "    movs r3, #0\n"
    "    ldr  r2, =0x0202BE4C\n"
    "    movs r1, #4\n"
    "1:\n"
    "    ldr  r0, [r2, #0xC]\n"
    S("ands r0, r1", "and r0, r1") "\n"
    "    cmp  r0, #0\n"
    "    bne  2f\n"
    "    cmp  r3, #0x40\n"
    "    bge  3f\n"
    S("adds r2, #0x48", "add r2, #0x48") "\n"
    S("adds r3, #1", "add r3, #1") "\n"
    "    b    1b\n"
    "2:\n"
    S("adds r0, r2, #0", "mov r0, r2") "\n"
    "    b    4f\n"
    "3:\n"
    "    movs r0, #0\n"
    "4:\n"
    "    pop  {r4-r7}\n"
    "    pop  {r1}\n"
    "    bx   r1\n"
    "    .ltorg\n"
    "    .align 2\n"

    ".global AumUsability\n"
    ".type AumUsability, %function\n"
    ".thumb_func\n"
    "AumUsability:\n"
    "    bl   FindDeadUnit\n"
    "    cmp  r0, #0\n"
    "    beq  5f\n"
    "    movs r0, #1\n"
    "5:\n"
    "    pop  {r4, r5}\n"
    "    pop  {r1}\n"
    "    bx   r1\n"
    "    .ltorg\n"
    "    .align 2\n"

    ".global AumEffect\n"
    ".type AumEffect, %function\n"
    ".thumb_func\n"
    "AumEffect:\n"
    "    push {r4-r7}\n"
    S("adds r0, r6, #0", "mov r0, r6") "\n"
    S("adds r1, r4, #0", "mov r1, r4") "\n"
    "    bl   AumItemStart\n"
    "    pop  {r4-r7}\n"
    "    ldr  r0, =0x0802FF77\n"
    "    bx   r0\n"
    "    .ltorg\n"
    "    .align 2\n"
);
