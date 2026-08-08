@echo off
setlocal

rem Go to repository root
pushd "%~dp0.." || exit /b 2

for /r "src" %%F in (*.h *.cpp) do (
    clang-format --verbose -i "%%F"
    if errorlevel 1 goto :error
)

for /r "test" %%F in (*.h *.cpp) do (
    clang-format --verbose -i "%%F"
    if errorlevel 1 goto :error
)

popd
exit /b 0

:error
set "exit_code=%errorlevel%"
popd
exit /b %exit_code%
