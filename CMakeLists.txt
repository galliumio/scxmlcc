cmake_minimum_required(VERSION 3.10)
project(scxmlcc CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules )

add_subdirectory(src)

if( DEFINED DEVELOPER )
   add_subdirectory(src/examples)
   # Unit tests download googletest at configure time; upstream does not support this on Windows.
   if( NOT WIN32 )
      enable_testing()
      add_subdirectory(src/test)
   endif()
endif()
