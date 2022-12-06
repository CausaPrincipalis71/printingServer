cmake_minimum_required(VERSION 3.14)

project(PrintingServer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Network)


add_executable(PrintingServer
  main.cpp
  printingserver.h printingserver.cpp
  muser.h muser.cpp
  mtcpsocket.h mtcpsocket.cpp
)
target_link_libraries(PrintingServer Qt${QT_VERSION_MAJOR}::Core Qt6::Network)

install(TARGETS PrintingServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
