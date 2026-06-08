@echo off
set BANKOMP_KEY=IME_SE9_SECRET_KEY_2026
g++ -std=c++17 main.cpp Control/Bank.cpp Interface/InterfaceConsole.cpp Model/Account.cpp Model/Client.cpp Model/CurrentAccount.cpp Model/SavingAccount.cpp Storage/Cryptography.cpp Storage/DataManager.cpp -o bankomp.exe
if %errorlevel% equ 0 (
    bankomp.exe
)
pause