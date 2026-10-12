@ Definitions.s -- direcciones de FE8U tomadas del fe8.s de FE-CLib (generado, no editar a mano).
@ Al existir este archivo, "Assemble Lyn.bat" lo usa en lugar del archivo de referencia completo.

.macro SET_FUNC name, value
	.global \name
	.type   \name, function
	.set    \name, \value
.endm

.macro SET_DATA name, value
	.global \name
	.type   \name, object
	.set    \name, \value
.endm

SET_FUNC Proc_StartBlocking, 0x08002CE1
SET_FUNC Proc_Break, 0x08002E95
SET_FUNC LockGame, 0x08015361
SET_FUNC UnlockGame, 0x08015371
SET_FUNC NewFadeOut, 0x080AE2F5
SET_FUNC NewFadeIn, 0x080AE2D1
SET_FUNC FadeOutExists, 0x080AE2B9
SET_FUNC FadeInExists, 0x080AE2A1
SET_FUNC BMapDispSuspend, 0x08030185
SET_FUNC BMapDispResume, 0x080301B9
SET_FUNC EndPlayerPhaseSideWindows, 0x0808D151
SET_FUNC ClearBg0Bg1, 0x0804E885
SET_FUNC SetupBackgrounds, 0x08001B59
SET_FUNC BG_Fill, 0x08001221
SET_FUNC BG_GetMapBuffer, 0x08001C4D
SET_FUNC BG_EnableSyncByMask, 0x08001FAD
SET_FUNC EndGreenText, 0x080049D1
SET_FUNC ResetText, 0x08003C95
SET_FUNC LoadUiFrameGraphics, 0x0804EB69
SET_FUNC LoadObjUIGfx, 0x08015681
SET_FUNC LoadIconPalettes, 0x080035BD
SET_FUNC DrawUiFrame2, 0x0804E369
SET_FUNC StartUiCursorHand, 0x080AC9C1
SET_FUNC ResetSysHandCursor, 0x080AD47D
SET_FUNC ShowSysHandCursor, 0x080AD51D
SET_FUNC HideSysHandCursor, 0x080AD565
SET_FUNC InitText, 0x08003D5D
SET_FUNC ClearText, 0x08003DC9
SET_FUNC PutDrawText, 0x0800443D
SET_FUNC GetStringFromIndex, 0x0800A241
SET_FUNC ResetUnitSprites, 0x08026689
SET_FUNC RefreshBMapGraphics, 0x080311A9
SET_FUNC SyncUnitSpriteSheet, 0x08026F2D
SET_FUNC RefreshUnitSprites, 0x080271A1
SET_FUNC InitPlayerPhaseInterface, 0x0808CFC5
SET_FUNC SetAllUnitNotBackSprite, 0x0801895D
SET_FUNC ResetUnitSpriteHover, 0x08027A41
SET_DATA gStatScreen, 0x02003BFC
SET_DATA gKeyStatusPtr, 0x0858791C
SET_DATA gEventSlots, 0x030004B8
SET_DATA gUnitArrayBlue, 0x0202BE4C
SET_DATA gBG0TilemapBuffer, 0x02022CA8
SET_FUNC BmMapFill, 0x080197E5
SET_FUNC HideMoveRangeGraphicsWrapper, 0x08022C99
SET_FUNC GetRescueStaffeePosition, 0x0802ECD1
SET_FUNC BattleInitItemEffect, 0x0802CB25
SET_FUNC BattleInitItemEffectTarget, 0x0802CBC9
SET_FUNC BattleApplyItemEffect, 0x0802CC55
SET_FUNC BeginBattleAnimations, 0x0802CA15
SET_FUNC UnitUpdateUsedItem, 0x08018995
SET_DATA gActionData, 0x0203A958
SET_DATA gActiveUnit, 0x03004E50
SET_DATA gBmMapRange, 0x0202E4E4
SET_DATA gBmMapMovement, 0x0202E4E0
SET_FUNC m4aSongNumStart, 0x080D01FD
SET_FUNC UnpackUiVArrowGfx, 0x080B53BD
SET_FUNC DisplayUiVArrow, 0x080B53F9
