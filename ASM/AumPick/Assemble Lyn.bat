@echo off
setlocal

REM ============ CONFIGURACION (ajusta estas rutas) ============
SET "startDir=C:\devkitPro\devkitARM\bin\"
SET "as=%startDir%arm-none-eabi-as.exe"
SET "gcc=%startDir%arm-none-eabi-gcc.exe"
SET "LYN=C:\devkitPro\lyn.exe"

REM Solo para archivos .c:
REM Carpeta con los headers de FE-CLib (donde esta gbafe.h)
SET "CLIB_INCLUDE=C:\devkitPro\FE-CLib_2025\include"
REM Archivo de referencia FE8U de FE-CLib (.s). Se usa si no hay Definitions.s
SET "REF=C:\devkitPro\FE-CLib_2025\fe8.s"
REM =============================================================

if "%~1"=="" (
	echo Arrastra un archivo .s, .asm o .c sobre este .bat
	pause
	exit /b 1
)

REM Trabajar en la carpeta del archivo arrastrado
cd /d "%~dp1"
set "NAME=%~n1"
set "EXT=%~x1"

REM Comprobar que existen las herramientas
if not exist "%as%" (
	echo No encuentro el ensamblador: %as%
	goto error
)
if not exist "%LYN%" (
	echo No encuentro lyn: %LYN%
	goto error
)
if /i "%EXT%"==".c" (
	if not exist "%gcc%" (
		echo No encuentro gcc: %gcc%
		goto error
	)
	if not exist "%CLIB_INCLUDE%\gbafe.h" (
		echo No encuentro gbafe.h en: %CLIB_INCLUDE%
		goto error
	)
)

REM Borrar salida vieja para no confundirla con una nueva
if exist "%NAME%.lyn.event" del /q "%NAME%.lyn.event"

if /i "%EXT%"==".c" goto compile_c

:assemble_s
%as% -g -mcpu=arm7tdmi -mthumb-interwork "%~1" -o "%NAME%.elf"
if errorlevel 1 goto error
goto link

:compile_c
"%gcc%" -std=gnu99 -mcpu=arm7tdmi -mthumb -mthumb-interwork -mlong-calls -fomit-frame-pointer -ffreestanding -fno-builtin -O2 -Wall -I"%CLIB_INCLUDE%" -I"%~dp1." -c "%~1" -o "%NAME%.elf"
if errorlevel 1 goto error
goto link

:link
set "REFELF="
if exist "Definitions.s" (
	"%as%" -g -mcpu=arm7tdmi -mthumb-interwork "Definitions.s" -o "Definitions.elf"
	if errorlevel 1 goto error
	set "REFELF=Definitions.elf"
	goto run_lyn
)
if /i "%EXT%"==".c" (
	if not exist "%REF%" (
		echo No encuentro el archivo de referencia: %REF%
		goto error
	)
	"%as%" -g -mcpu=arm7tdmi -mthumb-interwork "%REF%" -o "Reference.elf"
	if errorlevel 1 goto error
	set "REFELF=Reference.elf"
)

:run_lyn
if defined REFELF (
	"%LYN%" "%NAME%.elf" "%REFELF%" > "%NAME%.lyn.event"
) else (
	"%LYN%" "%NAME%.elf" > "%NAME%.lyn.event"
)
if errorlevel 1 goto error

REM Si lyn no escribio nada, algo fallo aunque no devolviera error
for %%F in ("%NAME%.lyn.event") do if %%~zF EQU 0 (
	echo lyn genero un archivo vacio.
	goto error
)

if exist "%NAME%.elf" del /q "%NAME%.elf"
if exist "Definitions.elf" del /q "Definitions.elf"
if exist "Reference.elf" del /q "Reference.elf"

echo.
echo Listo: %NAME%.lyn.event
pause
exit /b 0

:error
echo.
echo ERROR: algo fallo (revisa los mensajes de arriba y las rutas de CONFIGURACION).
if exist "%NAME%.lyn.event" del /q "%NAME%.lyn.event"
if exist "%NAME%.elf" del /q "%NAME%.elf"
if exist "Definitions.elf" del /q "Definitions.elf"
if exist "Reference.elf" del /q "Reference.elf"
pause
exit /b 1
