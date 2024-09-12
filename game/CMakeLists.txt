cmake_minimum_required(VERSION 3.10)

project(CentipedeGame VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(CentipedeGame ${SOURCES})

target_include_directories(CentipedeGame PRIVATE ${PROJECT_SOURCE_DIR}/include)

include_directories(${PROJECT_SOURCE_DIR}/../SFML/include)

link_directories(${PROJECT_SOURCE_DIR}/../SFML/lib)

target_link_libraries(CentipedeGame PUBLIC sfml-graphics sfml-system sfml-window)

set_target_properties(
    CentipedeGame PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${COMMON_OUTPUT_DIR}/bin"
)

file(COPY ${PROJECT_SOURCE_DIR}/assets
    DESTINATION "${COMMON_OUTPUT_DIR}/bin")