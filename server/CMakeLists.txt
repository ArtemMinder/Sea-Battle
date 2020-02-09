cmake_minimum_required(VERSION 3.14)
project(server)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5Network CONFIG
     PATHS D:/Programms/QT/5.12.6/mingw73_64/lib/cmake/Qt5Network
    NO_DEFAULT_PATH)

find_package(Qt5Core CONFIG
     PATHS D:/Programms/QT/5.12.6/mingw73_64/lib/cmake/Qt5Core
    NO_DEFAULT_PATH)
list(APPEND THIRD_PARTY_LIBS  Qt5::Network Qt5::Core)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp Application.cpp Application.h Server.cpp
    Server.h Client.cpp Client.h Field.cpp Field.h Constants.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
     Qt5::Network
     Qt5::Core
        )
