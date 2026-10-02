@ Change Edition with Optional Custom Protagonist Unit ID
@
@ Event format: 40 0D [Edition] [UnitID] [ASM+1]
@
@ Args:
@   [r3,#0x2] = Edition (0x1=Tutorial, 0x2=Eirika, 0x3=Ephraim)
@   [r3,#0x3] = Custom Protagonist Unit ID (0x00 = vanilla behavior)
@
@ The custom Unit ID is written to ProtagonistIDRam (defined below).
@ If you don't need this feature, always pass 0x00 as UnitID.
@
@ Author: 7743
@ Modified for custom protagonist support

.align 4
.macro blh to, reg=r3
  ldr \reg, =\to
  mov lr, \reg
  .short 0xf800
.endm

@ ============================================
@ CONFIG: Cambiá esta dirección si tu hack
@ usa otra zona de RAM para el Unit ID del
@ protagonista. Si no usás esta feature,
@ podés dejarla igual — solo escribe cuando
@ UnitID != 0x00.
@ ============================================
.equ ProtagonistIDRam, 0x0202BCF0 + 0x1C

.thumb
	push	{r4, lr}

	ldr  r3, [r0, #0x38]      @ Puntero al comando de evento actual
	ldrb r1, [r3, #0x2]       @ Arg1: byte de Edición
	ldrb r2, [r3, #0x3]       @ Arg2: Unit ID del protagonista custom

	@ Escribir edición en ChapterData+0x1B
	ldr  r0, =0x0202BCF0
	strb r1, [r0, #0x1B]

	@ Si se proporcionó un Unit ID custom, escribirlo en RAM
	cmp  r2, #0x00
	beq  SkipProtagWrite
	ldr  r4, =ProtagonistIDRam
	strb r2, [r4]
SkipProtagWrite:

	@ Código de retorno 0x17 (continuar ejecución del evento)
	mov  r0, #0x17

	pop	{r4, r1}
	bx	r1

.ltorg