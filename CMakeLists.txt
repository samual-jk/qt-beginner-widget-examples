cmake_minimum_required(VERSION 3.16)

project(NotepadPro LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt MOC/UIC/RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
)
qt_standard_project_setup()

qt_add_executable(NotepadPro
    main.cpp

    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
    src/ui/MainWindow.ui
    src/ui/EditorWidget.h
    src/ui/EditorWidget.cpp
    src/ui/FindReplaceDialog.ui
    src/ui/FindReplaceDialog.h
    src/ui/FindReplaceDialog.cpp
    src/ui/SignupDialog.ui
    src/ui/SignupDialog.h
    src/ui/SignupDialog.cpp
    src/ui/LoginDialog.ui
    src/ui/LoginDialog.h
    src/ui/LoginDialog.cpp
    src/ui/CustomToolbar.ui
    src/ui/CustomToolbar.h
    src/ui/CustomToolbar.cpp

    src/core/Document.h
    src/core/Document.cpp
    src/core/UserManager.h
    src/core/UserManager.cpp
    src/core/FileManager.h
    src/core/FileManager.cpp
    src/core/SettingsManager.h
    src/core/SettingsManager.cpp
)

target_include_directories(NotepadPro PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
)
