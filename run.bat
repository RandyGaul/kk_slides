#/bin/bash 2>nul || goto :windows

# bash
gcc src/main.c -o main.exe && ./main.exe
exit

:windows
@echo off
cls
tcc\tcc src\main.c && main.exe
