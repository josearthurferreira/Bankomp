#!/bin/bash
export BANKOMP_KEY="IME_SE9_SECRET_KEY_2026"
g++ -std=c++17 main.cpp Control/*.cpp Interface/*.cpp Model/*.cpp Storage/*.cpp -o bankomp
if [ $? -eq 0 ]; then
    ./bankomp
fi