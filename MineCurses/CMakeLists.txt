cmake_minimum_required(VERSION 2.6)
project(MineCurses)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_PATH})
find_library(CURSES_PANEL_LIBRARY panel)
find_library(CURSES_NCURSESXX_LIBRARY ncurses++)

find_path(minesweeper_INCLUDE_DIR minesweeper.h)
include_directories(${minesweeper_INCLUDE_DIR})

find_library(minesweeper_LIBRARY minesweeper)

option(MC_WITH_MOUSE "Build with mouse support." OFF)

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

configure_file("mc_conf.h.in" "${PROJECT_BINARY_DIR}/config/mc_conf.h")
include_directories("${PROJECT_BINARY_DIR}/config")

add_definitions(-std=c++11)

add_executable(minecurses mc_app.cpp mc_init_form.cpp)
target_link_libraries(minecurses ${minesweeper_LIBRARY} ${CURSES_NCURSESXX_LIBRARY} ${CURSES_FORM_LIBRARY} ${CURSES_PANEL_LIBRARY} ${CURSES_LIBRARY})
install(TARGETS minecurses DESTINATION bin)
