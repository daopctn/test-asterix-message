cmake_minimum_required(VERSION 3.10)
project(asterix-tcp-client VERSION 1.0.0 LANGUAGES CXX)

# C++11 Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt Configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Network)

# Source files
set(SOURCES
    src/main.cpp
    src/TcpTestClient.cpp
    src/AsterixMessage.cpp
    src/HexDumper.cpp
    src/TimingStats.cpp
)

set(HEADERS
    src/TcpTestClient.h
    src/AsterixMessage.h
    src/HexDumper.h
    src/TimingStats.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Network
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
