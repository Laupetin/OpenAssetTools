@echo off
setlocal

rem Go to repository root
pushd "%~dp0.." || exit /b 2

if not defined CLANG_FORMAT_BIN set "CLANG_FORMAT_BIN=clang-format"

for /r "src" %%F in (*.h *.cpp) do (
    "%CLANG_FORMAT_BIN%" -Werror -ferror-limit=1 --dry-run "%%F"
    if errorlevel 1 goto :error
)

for /r "test" %%F in (*.h *.cpp) do (
    "%CLANG_FORMAT_BIN%" -Werror -ferror-limit=1 --dry-run "%%F"
    if errorlevel 1 goto :error
)

popd
exit /b 0

:error
set "exit_code=%errorlevel%"
popd
exit /b %exit_code%
