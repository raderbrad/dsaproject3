cmake_minimum_required(VERSION 3.20)
project(Project3)

set(CMAKE_CXX_STANDARD 11)
set(SFML_DIR "C:/Libraries/SFML-2.5.1/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
add_executable(Project3 main.cpp)
target_link_libraries(Project3 sfml-graphics sfml-audio)
