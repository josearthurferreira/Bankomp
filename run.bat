@echo off
set BANKOMP_KEY=IME_SE9_SECRET_KEY_2026
g++ -std=c++17 main.cpp Control/*.cpp Interface/*.cpp Model/*.cpp Storage/*.cpp -o bankomp.exe
if %errorlevel% equ 0 (
    cls
    bankomp.exe
) else (
    echo.
    echo [ERRO] Falha na compilacao do projeto.
    echo.
)
pause