@echo off
setlocal

REM ============ CONFIGURACION (ajusta estas rutas) ============
SET startDir=C:\devkitPro\devkitARM\bin\
SET as="%startDir%arm-none-eabi-as"
SET gcc="%startDir%arm-none-eabi-gcc"
SET LYN="C:\devkitPro\lyn.exe"

REM Solo para archivos .c:
REM Carpeta con los headers de FE-CLib (donde esta gbafe.h)
SET CLIB_INCLUDE=C:\devkitPro\FE-CLib_2025\include
REM Archivo de referencia FE8U de FE-CLib (.s). Se usa si no hay Definitions.s
SET REF=C:\devkitPro\FE-CLib_2025\fe8.s
REM =============================================================

if "%~1"=="" (
	echo Arrastra un archivo .s, .asm o .c sobre este .bat
	pause
	exit /b 1
)

REM Trabajar en la carpeta del archivo arrastrado
cd /d "%~dp1"

if /i "%~x1"==".c" goto compile_c

:assemble_s
@rem Assemble into an elf
%as% -g -mcpu=arm7tdmi -mthumb-interwork "%~1" -o "%~n1.elf"
if errorlevel 1 goto error
goto link

:compile_c
@rem Compile C into an elf
%gcc% -std=gnu99 -mcpu=arm7tdmi -mthumb -mthumb-interwork -mlong-calls -fomit-frame-pointer -ffreestanding -fno-builtin -O2 -I"%CLIB_INCLUDE%" -I"%~dp1." -c "%~1" -o "%~n1.elf"
if errorlevel 1 goto error
goto link

:link
set "REFELF="
if exist "Definitions.s" (
	@rem Assemble definitions into a .elf if exists
	%as% -g -mcpu=arm7tdmi -mthumb-interwork "Definitions.s" -o "Definitions.elf"
	set "REFELF=Definitions.elf"
) else if /i "%~x1"==".c" (
	if exist "%REF%" (
		%as% -g -mcpu=arm7tdmi -mthumb-interwork "%REF%" -o "Reference.elf"
		set "REFELF=Reference.elf"
	)
)

if defined REFELF (
	%LYN% "%~n1.elf" "%REFELF%" > "%~n1.lyn.event"
) else (
	%LYN% "%~n1.elf" > "%~n1.lyn.event"
)
if errorlevel 1 goto error

if exist "%~n1.elf" del /q "%~n1.elf"
if exist "Definitions.elf" del /q "Definitions.elf"
if exist "Reference.elf" del /q "Reference.elf"

echo.
echo Listo: %~n1.lyn.event
pause
exit /b 0

:error
echo.
echo ERROR: algo fallo (revisa los mensajes de arriba y las rutas de CONFIGURACION).
if exist "%~n1.elf" del /q "%~n1.elf"
if exist "Definitions.elf" del /q "Definitions.elf"
if exist "Reference.elf" del /q "Reference.elf"
pause
exit /b 1
