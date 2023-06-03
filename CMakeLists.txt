project(Recipes LANGUAGES CXX)

cmake_minimum_required(VERSION 3.19.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

file(GLOB sources "src/*.cpp" "src/*.h" "src/*.ui" "res/*.qrc")

add_executable(Recipes WIN32 ${sources})

target_link_libraries(Recipes Qt5::Widgets)

find_program(windeployqt_executable windeployqt)

add_custom_command(TARGET Recipes POST_BUILD COMMAND "${windeployqt_executable}"
    --no-translations --no-webkit2 --no-angle --no-opengl-sw
    $<TARGET_FILE_DIR:Recipes>
    COMMENT "Running windeployqt...")
