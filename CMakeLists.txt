cmake_minimum_required(VERSION 3.16)

project(DriverStation VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Qml)

# Match Qt's upstream fix: AGL is unused and was removed from the macOS 26 SDK.
# https://github.com/qt/qtbase/commit/42b0903c34bd049729f85fcd6236e64f28898b34
if(APPLE AND TARGET WrapOpenGL::WrapOpenGL)
    get_target_property(qt_opengl_libraries WrapOpenGL::WrapOpenGL INTERFACE_LINK_LIBRARIES)
    list(FILTER qt_opengl_libraries EXCLUDE REGEX "(^-framework AGL$|/AGL\\.framework(/|$))")
    set_property(TARGET WrapOpenGL::WrapOpenGL PROPERTY
        INTERFACE_LINK_LIBRARIES "${qt_opengl_libraries}")
endif()

qt_policy(SET QTP0001 NEW)

qt_add_executable(appDriverStation
    src/main.cpp
    src/SystemMonitor.cpp
    src/TimeProvider.cpp
    include/SystemMonitor.h
    include/TimeProvider.h
)
qt_add_qml_module(appDriverStation
    URI DriverStation
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/BatteryIndicator.qml
)

qt_add_resources(appDriverStation "resources"
    PREFIX "/"
    FILES
        images/chauffer.png
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appDriverStation PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDriverStation
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appDriverStation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(appDriverStation
    PRIVATE Qt6::Quick Qt6::QuickControls2
)

option(BUILD_TESTING "Build regression tests" OFF)
include(CTest)
if(BUILD_TESTING)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    add_executable(systemMonitorTests
        tests/SystemMonitorTest.cpp
        src/SystemMonitor.cpp
        include/SystemMonitor.h
    )
    target_include_directories(systemMonitorTests PRIVATE include)
    target_link_libraries(systemMonitorTests PRIVATE Qt6::Test)
    add_test(NAME SystemMonitorTest COMMAND systemMonitorTests)
endif()

include(GNUInstallDirs)
install(TARGETS appDriverStation
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
