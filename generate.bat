@echo off

cd %~dp0

git submodule update --init --recursive
tools\premake5.exe %* vs2022