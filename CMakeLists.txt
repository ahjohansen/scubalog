cmake_minimum_required(VERSION 3.19.0)

project(ScubaLog
        VERSION 0.4)

set(CMAKE_AUTOMOC ON)

find_package(KF5XmlGui REQUIRED)
find_package(KF5I18n REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)

add_subdirectory(scubalog)
