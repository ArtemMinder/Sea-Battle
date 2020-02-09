cmake_minimum_required(VERSION 3.14)
project(client)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5Network CONFIG
    PATHS D:/Programms/QT/5.12.6/mingw73_64/lib/cmake/Qt5Network
    NO_DEFAULT_PATH
    )
find_package(Qt5Widgets CONFIG
         PATHS D:/Programms/QT/5.12.6/mingw73_64/lib/cmake/Qt5Widgets
         Qt5Network CONFIG
      )
list(APPEND THIRD_PARTY_LIBS Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Network)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp Connect.h Connect.cpp Field.h
    Field.cpp Game.h Game.cpp Logic.h Logic.cpp mainwindow.h mainwindow.cpp
Textures.h Textures.cpp mainwindow.ui resources.qrc)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        Qt5::Network
        )
