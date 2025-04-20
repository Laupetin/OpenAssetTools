@echo off

set PREMAKE_URL="https://github.com/premake/premake-core/releases/download/v5.0.0-beta6/premake-5.0.0-beta6-windows.zip"
set PREMAKE_HASH="c34a6e0b15f119f6284886298fdd8df543af87ad16f3ce5f4d0a847be2a88343"

@REM The following variables can be set:
@REM    PREMAKE_NO_GLOBAL - Ignore premake5 executable from path
@REM    PREMAKE_NO_PROMPT - Download premake5 without prompting

goto start

:downloadpremake

if not exist "build" mkdir "build"

where /q "pwsh"
IF NOT ERRORLEVEL 1 (
    set POWERSHELL_BIN="pwsh"
) else (
    set POWERSHELL_BIN="powershell"
)

echo Downloading...
%POWERSHELL_BIN% -NoProfile -NonInteractive -Command "Invoke-WebRequest %PREMAKE_URL% -OutFile build/premake.zip"
IF ERRORLEVEL 1 (
    echo Download failed >&2
    exit 2
)

echo Extracting...
%POWERSHELL_BIN% -NoProfile -NonInteractive -Command "Expand-Archive -LiteralPath build/premake.zip -DestinationPath build"
IF ERRORLEVEL 1 (
    echo Extraction failed >&2
    exit 2
)

rm build/premake.zip

echo Verifying hash...
%POWERSHELL_BIN% -NoProfile -NonInteractive -Command "if ((Get-FileHash -LiteralPath build/premake5.exe -Algorithm SHA256).Hash -eq \"%PREMAKE_HASH%\") { exit 0 } else { exit 1 }"
IF ERRORLEVEL 1 (
    echo Hash verification failed >&2
    rm build/premake5.exe
    exit 2
)

exit /B 0

cd %~dp0

:start

IF "%PREMAKE_NO_GLOBAL%" EQU "" (
    where /Q "premake5.exe"
    IF NOT ERRORLEVEL 1 (
        set PREMAKE_BIN="premake5.exe"
        goto runpremake
    )
)

IF EXIST build/premake5.exe (
    set PREMAKE_BIN="build/premake5.exe"
    goto runpremake
)

if "%PREMAKE_NO_PROMPT%" NEQ "" (
    call:downloadpremake
    set PREMAKE_BIN="build/premake5.exe"
    goto runpremake
)

echo Could not find premake5. You can either install it yourself or this script download it for you.
set /p choice="Do you wish to download it automatically? [y/N]> "
if /i "%choice%" == "y" (
    call:downloadpremake
    set PREMAKE_BIN="build/premake5.exe"
    goto runpremake
)

echo Please install premake5 and try again
exit 1

:runpremake
git submodule update --init --recursive
%PREMAKE_BIN% %* vs2022
