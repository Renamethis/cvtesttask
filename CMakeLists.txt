cmake_minimum_required(VERSION 3.0.0)
project(NPOREGIONTESTTASK VERSION 0.1.0)
find_package( OpenCV REQUIRED )
find_package(nlohmann_json 3.2.0 REQUIRED)
include_directories( 
    ${OpenCV_INCLUDE_DIRS},
    ${CMAKE_SOURCE_DIR}/lib
)
add_executable(NPOREGIONTESTTASK main.cpp)
target_link_libraries(NPOREGIONTESTTASK ${OpenCV_LIBS})
target_link_libraries(NPOREGIONTESTTASK nlohmann_json::nlohmann_json)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
