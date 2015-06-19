cmake_minimum_required(VERSION 2.6)
project(libminesweeper)

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

add_definitions(-std=c++11)

add_library(minesweeper STATIC minesweeper.cpp)
install(TARGETS minesweeper DESTINATION lib)
install(FILES minesweeper.h DESTINATION include)
