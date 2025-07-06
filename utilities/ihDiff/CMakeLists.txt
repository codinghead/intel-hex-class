cmake_minimum_required(VERSION 3.25)
project(ihDiff)

# Project Files
set(SOURCE_FILES
    ihDiff.cpp
    ../../intelhex_class/intelhexclass.cpp)
    
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
