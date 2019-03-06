@echo off

if exist %~dp0Release\amanda.tests.exe (
    %~dp0Release\amanda.tests.exe
) else (
    echo Please, rebuild tests.
)
