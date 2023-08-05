cmake_minimum_required(VERSION 3.20)

project(IMClient)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
 
#????qt?????uic??moc??rcc
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#?څc++???
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#???qt??????
if(WIN32)
    set(CMAKE_PREFIX_PATH "E:/qt/5.15.2/mingw81_64") # Qt Kit Dir
elseif(UNIX)
    set(CMAKE_PREFIX_PATH "/home/qin/app/qt5.15/5.15.2/gcc_64")
    #?????????????????��??
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    SET(CMAKE_INSTALL_RPATH "\${ORIGIN}:\${ORIGIN}/../lib")
endif()

find_package(Qt5QuickWidgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5 COMPONENTS Widgets Core Gui Network Quick Qml Sql REQUIRED) # Qt COMPONENTS

file(GLOB_RECURSE srcs1 ${CMAKE_CURRENT_SOURCE_DIR}/module/*.cpp)
file(GLOB_RECURSE srcs2 ${CMAKE_CURRENT_SOURCE_DIR}/protocol/*.cpp)
file(GLOB_RECURSE srcs3 ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.cpp)

set(qrc ${CMAKE_CURRENT_SOURCE_DIR}/ui/res/LogInWidget.qrc)
#????????
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

#?????????��??
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/ui)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories("E:/boost_1_75_0")
#?????????????��??
link_directories(${CMAKE_SOURCE_DIR}/lib)

aux_source_directory(./ srcs4)

add_executable(${PROJECT_NAME} ${srcs1} ${srcs2} ${srcs3} ${srcs4} ${qrc} ${uiFile})

#????????
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Network Qt5::Qml Qt5::Quick 
    Qt5::QuickWidgets Qt5::Sql Qt5::Multimedia pthread log4cxx) 