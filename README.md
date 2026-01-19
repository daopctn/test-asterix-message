# ASTERIX TCP Test Client

A Qt-based console application for testing ASTERIX message transmission to TCP servers. This application allows you to send ASTERIX-formatted binary messages, view hex dumps, and measure connection/transmission timings.

## Features

- **Send ASTERIX CAT messages** to TCP server (localhost:3000)
- **Hex dump display** of sent data with offset, hex, and ASCII columns
- **Timing measurements** for connection establishment and transmission
- **Interactive testing** with single-send and batch-send modes
- **Detailed statistics reporting** with min/max/average timings
- **Multiple sample messages** (CAT 001, CAT 034, CAT 048)
- **Customizable message types** for different testing scenarios

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

# The executable will be in build/bin/asterix-tcp-client
```

### Windows (Visual Studio)

```cmd
mkdir build && cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release
```

## Usage

### Starting the Application

```bash
./build/bin/asterix-tcp-client
```

The application will present an interactive menu:

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

```bash
# Terminal 1: Start your ASTERIX TCP server
node server.js  # or your server implementation

# Terminal 2: Run the test client
./build/bin/asterix-tcp-client

# Select option 1 to send a single message
Enter choice: 1

Connecting to 127.0.0.1:3000...
Connected! Sending message...
[SUCCESS] Message sent!
Connection: 2ms | Send: 1ms | Bytes: 12 | Throughput: 12.00 KB/s

Hex dump of sent data:
00000000  01 00 0C 80 01 02 03 04  05 06 07 08              |............|
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

## Testing with a Simple Server

You can test the client with a simple TCP server. Here's a Node.js example:

```javascript
// server.js
const net = require('net');

const server = net.createServer((socket) => {
  console.log('Client connected');

  socket.on('data', (data) => {
    console.log('Received:', data.length, 'bytes');
    console.log('Hex:', data.toString('hex'));

    // Parse ASTERIX header
    const cat = data[0];
    const len = (data[1] << 8) | data[2];
    console.log(`CAT: ${cat}, Length: ${len}`);
  });

  socket.on('end', () => {
    console.log('Client disconnected');
  });
});

server.listen(3000, '127.0.0.1', () => {
  console.log('Server listening on 127.0.0.1:3000');
});
```

Run the server:
```bash
node server.js
```

Then run the client in another terminal and test the connection.

## Project Structure

```
.
├── CMakeLists.txt              # CMake build configuration
├── README.md                   # This file
├── qt-tcp-client-development-plan.md  # Detailed development plan
├── src/
│   ├── main.cpp                # Main entry point and interactive menu
│   ├── TcpTestClient.h         # TCP client class header
│   ├── TcpTestClient.cpp       # TCP client implementation
│   ├── AsterixMessage.h        # ASTERIX message builder header
│   ├── AsterixMessage.cpp      # ASTERIX message implementation
│   ├── HexDumper.h             # Hex dump utility header
│   ├── HexDumper.cpp           # Hex dump implementation
│   ├── TimingStats.h           # Timing statistics header
│   └── TimingStats.cpp         # Timing statistics implementation
└── build/                      # Build directory (generated)
    └── bin/
        └── asterix-tcp-client  # Compiled executable
```

## Troubleshooting

### Connection Refused Error

**Problem**: `[ERROR] Connection refused - is the server running?`

**Solution**: Ensure your TCP server is running on localhost:3000 before starting the client.

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
