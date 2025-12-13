# HMI3 - Component System for XSmall-HMI SCADA

Система компонентов и контейнеров для SCADA системы XSmall-HMI.

## Особенности

- **Container** - компоновщик для управления UI компонентами
- **Component** - абстрактный базовый класс для всех элементов
- **Command System** - абстрактный и конкретный классы для приема сетевых команд
- **Сетевое взаимодействие** - прием команд по TCP на порту 8080
- **Модульное тестирование** - Google Test для unit-тестов

## Требования

- C++17 компилятор
- CMake
- SFML

## Сборка

## Windows
```bash
mkdir build
cd build
cmake ..
cmake --build .
./hmi3_demo.exe
./hmi3_tests.exe

## Linux
1. Установите системные зависимости для графики:
Ubuntu/Debian: sudo apt update && sudo apt install -y libx11-dev libxrandr-dev libxcursor-dev libxi-dev cmake g++ git
Fedora/RHEL: sudo dnf install libX11-devel libXrandr-devel libXcursor-devel libXi-devel cmake gcc-c++ git
Arch: sudo pacman -S libx11 libxrandr libxcursor libxi cmake gcc git

2. Соберите:
cmake -B build
cmake --build build

3. Запустите:
./build/hmi3_demo
./build/hmi3_tests