cmake_minimum_required(VERSION 3.20)

project(FluentPlayer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QML_IMPORT_PATH ${CMAKE_BINARY_DIR}/FluentPlayer CACHE STRING "Qt Creator extra QML import paths" FORCE)

if(QT_VERSION VERSION_GREATER_EQUAL "6.3")
    qt_standard_project_setup()
else()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/bin/debug>)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/bin/release>)
endif()


if(WIN32)
    set(3RDPARTY_DIR_PATH ${CMAKE_SOURCE_DIR}/3rdparty/win32)
elseif(APPLE)
    set(3RDPARTY_DIR_PATH ${CMAKE_SOURCE_DIR}/3rdparty/macos)
else()
    set(3RDPARTY_DIR_PATH ${CMAKE_SOURCE_DIR}/3rdparty/linux)
endif()

link_directories(${3RDPARTY_DIR_PATH}/ffmpeg/lib)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick Multimedia)

#遍历所有Cpp文件
file(GLOB_RECURSE CPP_FILES *.cpp *.h)
foreach(filepath ${CPP_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND sources_files ${filename})
endforeach(filepath)

if(QT_VERSION VERSION_GREATER_EQUAL "6.2")
    #遍历所有qml文件
    file(GLOB_RECURSE QML_PATHS *.qml qmldir)
    foreach(filepath ${QML_PATHS})
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
        if(${filepath} MATCHES "qml-Qt6")
            string(REPLACE "qml-Qt6" "qml" filealias ${filename})
            set_source_files_properties(${filename} PROPERTIES QT_RESOURCE_ALIAS ${filealias})
            if(${filename} MATCHES "qmldir")
                list(APPEND resource_files ${filename})
            else()
                list(APPEND qml_files ${filename})
            endif()
        endif()
    endforeach(filepath)

    #遍历所有资源文件
    file(GLOB_RECURSE RES_PATHS *.png *.jpg *.svg *.ico *.ttf *.webp *.obj *.qsb)
    foreach(filepath ${RES_PATHS})
        message(${filepath})
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
        list(APPEND resource_files ${filename})
    endforeach(filepath)
endif()

add_executable(FluentPlayer
    ${sources_files}
)

if(WIN32)
    file(GLOB_RECURSE DLL_FILES ${CMAKE_SOURCE_DIR}/3rdparty/*.dll ${CMAKE_SOURCE_DIR}/3rdparty/*.exe)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${DLL_FILES}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
endif()

if(QT_VERSION VERSION_GREATER_EQUAL "6.2")
    qt_add_qml_module(FluentPlayer
        URI "FluentPlayer"
        VERSION 1.0
        QML_FILES ${qml_files}
        RESOURCES ${resource_files}
        RESOURCE_PREFIX "/"
    )
else()
    target_include_directories(FluentPlayer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
target_sources(FluentPlayer PRIVATE qml.qrc)
endif()

target_include_directories(FluentPlayer PRIVATE
    ${3RDPARTY_DIR_PATH}/ffmpeg/include
)

target_link_libraries(FluentPlayer PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Multimedia
    avformat
    avcodec
    avutil
    swscale
    swresample
    avdevice
    avfilter
)

set_target_properties(FluentPlayer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
