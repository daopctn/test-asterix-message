# ASTERIX TCP Client & Server

A complete Qt-based console application suite for testing ASTERIX message transmission. Includes both a **client** for sending messages and a **server** for receiving and decoding them.

## Components

### 1. ASTERIX TCP Client (`asterix-tcp-client`)
Interactive console application for sending ASTERIX messages to a TCP server.

### 2. ASTERIX TCP Server (`asterix-tcp-server`)
TCP server that listens for incoming connections, receives ASTERIX messages, and displays decoded message details.

## Features

### Client Features
- **Send ASTERIX CAT messages** to TCP server (localhost:3000)
- **Hex dump display** of sent data with offset, hex, and ASCII columns
- **Timing measurements** for connection establishment and transmission
- **Interactive testing** with single-send and batch-send modes
- **Detailed statistics reporting** with min/max/average timings
- **Multiple sample messages** (CAT 001, CAT 034, CAT 048)
- **Customizable message types** for different testing scenarios

### Server Features
- **Listen on localhost:3000** for incoming ASTERIX messages
- **Decode ASTERIX messages** (CAT/LEN/FSPEC/DATA format)
- **Display decoded information** with category name and details
- **Binary FSPEC visualization** showing field presence indicators
- **Hex dump display** of received messages
- **Connection tracking** with client address information
- **Statistics** showing total messages and bytes received

## Requirements

- **Qt 5.14 or later** (Qt5 Core and Network modules)
- **CMake 3.10 or later**
- **C++11 compatible compiler** (GCC, Clang, or MSVC)

## Building

### Linux/macOS

```bash
# Install Qt5 (Ubuntu/Debian)
sudo apt-get install qtbase5-dev qt5-qmake libqt5network5

# Create build directory and compile
mkdir build && cd build
cmake ..
make

# Two executables will be created:
# - build/bin/asterix-tcp-client (the client)
# - build/bin/asterix-tcp-server (the server)
```

### Windows (Visual Studio)

```cmd
mkdir build && cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release
```

## Usage

### Running Both Applications in Parallel Terminals

The typical usage is to run the server in one terminal and the client in another:

**Terminal 1 - Start the Server:**
```bash
./build/bin/asterix-tcp-server
```

Output:
```
╔════════════════════════════════════════════╗
║     ASTERIX TCP Server v1.0                ║
║     Listening for ASTERIX messages         ║
╚════════════════════════════════════════════╝
Qt Version: 5.15.13

[SERVER STARTED]
  Address: 127.0.0.1
  Port:    3000

Waiting for ASTERIX messages...
(Press Ctrl+C to stop)
```

**Terminal 2 - Start the Client:**
```bash
./build/bin/asterix-tcp-client
```

The client will present an interactive menu:

```
=== ASTERIX TCP Test Client v1.0 ===
Qt Version: 5.15.13
Target: 127.0.0.1:3000

╔═══════════════════════════════════════════╗
║     ASTERIX TCP Test Client - Menu        ║
╠═══════════════════════════════════════════╣
║  [1] Send single test message             ║
║  [2] Send N messages (batch test)         ║
║  [3] Show last message hex dump           ║
║  [4] Show timing statistics               ║
║  [5] Reset statistics                     ║
║  [6] Test connection                      ║
║  [7] Change message (select sample)       ║
║  [0] Exit                                 ║
╚═══════════════════════════════════════════╝
Enter choice:
```

### Menu Options

1. **Send single test message**: Connects to the server, sends one message, displays timing and hex dump
2. **Send N messages (batch test)**: Send multiple messages with option to keep connection open
3. **Show last message hex dump**: Display the hex dump of the last sent message
4. **Show timing statistics**: View aggregate statistics (min/max/avg) for all sent messages
5. **Reset statistics**: Clear all collected timing statistics
6. **Test connection**: Test if the server is reachable without sending data
7. **Change message**: Switch between different sample ASTERIX messages

### Example Session

**Terminal 1 - Server Output:**
```bash
./build/bin/asterix-tcp-server

[CLIENT CONNECTED] 127.0.0.1:45678
────────────────────────────────────────────

╔════════════════════════════════════════════╗
║  MESSAGE #1                                ║
╚════════════════════════════════════════════╝
╔════════════════════════════════════════════╗
║         DECODED ASTERIX MESSAGE            ║
╚════════════════════════════════════════════╝

Category:     48 (0x30)
Description:  Monoradar Target Reports
Length:       6 bytes

FSPEC (1 bytes): 80
  Binary: 10000000

Data Items (2 bytes):
  Hex: 01 02

Complete message hex dump:
00000000  30 00 06 80 01 02                                 |48.....|

[STATISTICS]
  Total messages received: 1
  Total bytes received:    6
```

**Terminal 2 - Client Output:**
```bash
./build/bin/asterix-tcp-client

# Select option 7 to change to CAT 048 message
Enter choice: 7
Enter choice: 4  # Select Sample CAT 048

# Select option 1 to send a single message
Enter choice: 1

Connecting to 127.0.0.1:3000...
Connected! Sending message...
[SUCCESS] Message sent!
Connection: 0ms | Send: 0ms | Bytes: 6 | Throughput: 0.00 KB/s

Hex dump of sent data:
00000000  30 00 06 80 01 02                                 |48.....|
```

## Customizing Test Messages

To create your own ASTERIX test messages, edit `src/AsterixMessage.cpp` and modify one of the sample message functions:

```cpp
AsterixMessage AsterixMessage::createSampleCat048()
{
    // ============================================
    // USER CUSTOMIZATION SECTION
    // Modify the hex values below to test different
    // ASTERIX message configurations
    // ============================================

    AsterixMessage msg(48);

    // FSPEC: I048/010 (Data Source Identifier) present
    msg.setFspec(QByteArray::fromHex("80"));

    // I048/010: SAC=0x01, SIC=0x02
    msg.setDataItems(QByteArray::fromHex("0102"));

    return msg;
}
```

After modifying, rebuild the application:

```bash
cd build
make
```

## ASTERIX Message Format

The application generates messages following the ASTERIX CAT format:

```
┌────────┬─────────────┬──────────────┬──────────────┐
│  CAT   │     LEN     │    FSPEC     │  Data Items  │
│ 1 byte │   2 bytes   │  Variable    │   Variable   │
│        │ Big-endian  │              │              │
└────────┴─────────────┴──────────────┴──────────────┘
```

- **CAT**: Category number (1 byte)
- **LEN**: Total message length in bytes, big-endian (2 bytes)
- **FSPEC**: Field Specification, indicates which data items are present (variable)
- **Data Items**: The actual data fields (variable)

## Configuration

Default settings (hardcoded in `TcpTestClient.h`):
- **Host**: 127.0.0.1
- **Port**: 3000
- **Connection Timeout**: 5000ms
- **Write Timeout**: 3000ms

To change these, modify the constants in `src/TcpTestClient.h` and rebuild.

## Server Details

The included Qt TCP server (`asterix-tcp-server`) provides complete ASTERIX message decoding:

- **Automatic FSPEC parsing**: Handles variable-length FSPEC with FX extension bits
- **Category identification**: Recognizes standard ASTERIX categories (001, 002, 034, 048, 062, 063)
- **Binary visualization**: Shows FSPEC bits in binary format for easy field identification
- **Complete hex dumps**: Displays both the parsed components and complete message
- **Error handling**: Validates message structure and reports decoding errors
- **Multiple clients**: Supports concurrent connections from multiple clients

## Project Structure

```
.
├── CMakeLists.txt              # CMake build configuration
├── README.md                   # This file
├── qt-tcp-client-development-plan.md  # Detailed development plan
├── src/
│   ├── main.cpp                # Client entry point and interactive menu
│   ├── server-main.cpp         # Server entry point
│   ├── TcpTestClient.h         # TCP client class header
│   ├── TcpTestClient.cpp       # TCP client implementation
│   ├── TcpServer.h             # TCP server class header
│   ├── TcpServer.cpp           # TCP server implementation
│   ├── AsterixMessage.h        # ASTERIX message builder header
│   ├── AsterixMessage.cpp      # ASTERIX message implementation
│   ├── AsterixDecoder.h        # ASTERIX message decoder header
│   ├── AsterixDecoder.cpp      # ASTERIX message decoder implementation
│   ├── HexDumper.h             # Hex dump utility header
│   ├── HexDumper.cpp           # Hex dump implementation
│   ├── TimingStats.h           # Timing statistics header
│   └── TimingStats.cpp         # Timing statistics implementation
└── build/                      # Build directory (generated)
    └── bin/
        ├── asterix-tcp-client  # Client executable
        └── asterix-tcp-server  # Server executable
```

## Troubleshooting

### Connection Refused Error

**Problem**: `[ERROR] Connection refused - is the server running?`

**Solution**: Ensure the ASTERIX TCP server is running before starting the client:
```bash
# Terminal 1 - Start the server first
./build/bin/asterix-tcp-server

# Terminal 2 - Then start the client
./build/bin/asterix-tcp-client
```

### Qt Not Found (CMake Error)

**Problem**: `Could not find a package configuration file provided by "Qt5"`

**Solution**: Install Qt5 development packages:
```bash
# Ubuntu/Debian
sudo apt-get install qtbase5-dev

# macOS (with Homebrew)
brew install qt@5

# Set Qt5_DIR if needed
export Qt5_DIR=/usr/local/opt/qt@5/lib/cmake/Qt5
```

### Permission Denied

**Problem**: Cannot execute the compiled binary

**Solution**: Make the file executable:
```bash
chmod +x build/bin/asterix-tcp-client
```

## License

This project is provided as-is for testing and educational purposes.

## Version

**Version**: 1.0.0
**Date**: January 2026
**Qt Version**: 5.15.13
