cmake_minimum_required(VERSION 3.15)
project(hmi3 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Используем FetchContent для автоматической загрузки зависимостей
include(FetchContent)

# Загружаем и настраиваем SFML
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
)

# Устанавливаем опции для SFML
set(SFML_BUILD_WINDOW TRUE)
set(SFML_BUILD_GRAPHICS TRUE)
set(SFML_BUILD_NETWORK TRUE)
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_SYSTEM TRUE)
set(SFML_BUILD_DOC FALSE)
set(SFML_BUILD_EXAMPLES FALSE)
set(SFML_BUILD_TEST_SUITE FALSE)
set(SFML_INSTALL_PKGCONFIG_FILES FALSE)
set(SFML_INSTALL_CMAKE_MODULES FALSE)
set(SFML_MISC_INSTALL_PREFIX "")

FetchContent_MakeAvailable(sfml)

# Находим Google Test
find_package(GTest REQUIRED)

# Добавляем путь к заголовочным файлам
include_directories(include)

# Основная библиотека
add_library(hmi3_lib
    src/container.cpp
    src/command_receiver.cpp
)

# Подключаем SFML к библиотеке
target_link_libraries(hmi3_lib
    sfml-graphics
    sfml-window
    sfml-system
    sfml-network
)

# Демо-программа
add_executable(hmi3_demo examples/demo.cpp)
target_link_libraries(hmi3_demo 
    hmi3_lib
)

# Тесты
add_executable(hmi3_tests
    tests/test_container.cpp
    tests/test_command_receiver.cpp
)
target_link_libraries(hmi3_tests 
    hmi3_lib
    GTest::gtest
    GTest::gtest_main
)

enable_testing()
gtest_discover_tests(hmi3_tests)

# Информация о сборке
message(STATUS "HMI3 Project configured successfully!")
message(STATUS "  Build:    cmake --build build")
message(STATUS "  Run demo: ./build/hmi3_demo")
message(STATUS "  Test:     cd build && ctest --verbose")