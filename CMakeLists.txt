cmake_minimum_required(VERSION 3.16)
project(musiqer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

# --- Enable moc, uic, rcc automatically ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Add executable
add_executable(musiqer
  src/main.cpp
  src/mainwindow.cpp
  src/mainwindow.h
  src/utils.cpp
  src/utils.h
  src/settingswindow.cpp
  src/settingswindow.h
)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Multimedia)
target_link_libraries(musiqer PRIVATE Qt6::Widgets Qt6::Core Qt6::Multimedia)
