cmake_minimum_required(VERSION 3.10)
project(asterix-tcp VERSION 1.0.0 LANGUAGES CXX)

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

# Common/shared source files
set(COMMON_SOURCES
    src/AsterixMessage.cpp
    src/HexDumper.cpp
    src/TimingStats.cpp
)

set(COMMON_HEADERS
    src/AsterixMessage.h
    src/HexDumper.h
    src/TimingStats.h
)

# Client-specific files
set(CLIENT_SOURCES
    src/main.cpp
    src/TcpTestClient.cpp
)

set(CLIENT_HEADERS
    src/TcpTestClient.h
)

# Server-specific files
set(SERVER_SOURCES
    src/server-main.cpp
    src/TcpServer.cpp
    src/AsterixDecoder.cpp
)

set(SERVER_HEADERS
    src/TcpServer.h
    src/AsterixDecoder.h
)

# Create client executable
add_executable(asterix-tcp-client
    ${CLIENT_SOURCES}
    ${CLIENT_HEADERS}
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
)

# Create server executable
add_executable(asterix-tcp-server
    ${SERVER_SOURCES}
    ${SERVER_HEADERS}
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
)

# Create simple client executable (no interactive menu)
add_executable(simple-client
    src/simple-client.cpp
    src/TcpTestClient.cpp
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
)

# Create bare socket client (minimal, no extra classes)
add_executable(bare-socket-client
    src/bare-socket-client.cpp
)

# Link Qt libraries to client
target_link_libraries(asterix-tcp-client PRIVATE
    Qt5::Core
    Qt5::Network
)

# Link Qt libraries to server
target_link_libraries(asterix-tcp-server PRIVATE
    Qt5::Core
    Qt5::Network
)

# Link Qt libraries to simple client
target_link_libraries(simple-client PRIVATE
    Qt5::Core
    Qt5::Network
)

# Link Qt libraries to bare socket client
target_link_libraries(bare-socket-client PRIVATE
    Qt5::Core
    Qt5::Network
)

# Include directories
target_include_directories(asterix-tcp-client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(asterix-tcp-server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(simple-client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Output directory
set_target_properties(asterix-tcp-client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(asterix-tcp-server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(simple-client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(bare-socket-client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
