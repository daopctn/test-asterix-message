# Qt TCP Test Client - Development Plan

**Project:** ASTERIX TCP Test Client  
**Version:** 1.0  
**Date:** January 2026  
**Author:** Development Team

---

## Executive Summary

This document outlines a comprehensive development plan for a Qt-based TCP test client application designed to test ASTERIX message transmission to a TCP server running on localhost:3000. The application will be a console-based utility with timing measurements, hex dump display, and interactive testing capabilities.

---

## Technology Stack

| Component | Technology | Version |
|-----------|------------|---------|
| Framework | Qt | 5.14.x |
| Language | C++ | C++11 |
| Build System | CMake | 3.10+ |
| Networking | Qt Network Module | QTcpSocket |
| Platform | Cross-platform | Linux/Windows/macOS |
| Application Type | Console | No GUI |

### Key Qt Classes to Use

| Class | Purpose |
|-------|---------|
| `QCoreApplication` | Event loop for console application |
| `QTcpSocket` | TCP client socket operations |
| `QElapsedTimer` | High-resolution timing measurements |
| `QByteArray` | Binary data handling for ASTERIX messages |
| `QTextStream` | Console I/O operations |
| `QCommandLineParser` | (Optional) Command-line argument parsing |

---

## Project Architecture Overview

```
qt-asterix-tcp-client/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp
│   ├── TcpTestClient.h
│   ├── TcpTestClient.cpp
│   ├── AsterixMessage.h
│   ├── AsterixMessage.cpp
│   ├── HexDumper.h
│   ├── HexDumper.cpp
│   ├── TimingStats.h
│   └── TimingStats.cpp
└── docs/
    └── usage.md
```

---

## Phase 1: Project Structure Setup

### Goal

Establish a properly configured CMake project with Qt 5.14 integration, creating a compilable console application skeleton that verifies the development environment is correctly set up.

### Detailed Steps

#### Step 1.1: Create Directory Structure

```bash
mkdir -p qt-asterix-tcp-client/{src,docs}
cd qt-asterix-tcp-client
```

#### Step 1.2: Create CMakeLists.txt

```cmake
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
```

#### Step 1.3: Create Skeleton main.cpp

```cpp
// src/main.cpp
#include <QCoreApplication>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("asterix-tcp-client");
    QCoreApplication::setApplicationVersion("1.0.0");
    
    QTextStream cout(stdout);
    cout << "=== ASTERIX TCP Test Client v1.0 ===" << Qt::endl;
    cout << "Qt Version: " << QT_VERSION_STR << Qt::endl;
    
    // TODO: Initialize TcpTestClient and start interactive loop
    
    return 0; // Will change to app.exec() when event loop needed
}
```

#### Step 1.4: Create Header Stubs

Create empty header files with include guards for all planned classes to ensure the project compiles.

### Deliverable

A compilable CMake project that prints version information and exits successfully. Build verification commands:

```bash
mkdir build && cd build
cmake ..
make
./bin/asterix-tcp-client
```

### Testing Strategy

| Test | Expected Result |
|------|-----------------|
| CMake configuration | No errors, Qt5 found |
| Compilation | Clean build, no warnings |
| Execution | Prints version info, exits with code 0 |

---

## Phase 2: ASTERIX Message Structure

### Goal

Implement a flexible ASTERIX message builder class that creates properly formatted binary messages according to ASTERIX CAT structure, with an easy-to-modify sample message for testing purposes.

### ASTERIX Message Format Overview

```
+--------+--------+--------+--------+--------+-----+
|  CAT   |   LEN (2 bytes)  |  FSPEC  | Data  |
| 1 byte |   Big Endian     | N bytes | Items |
+--------+--------+--------+--------+--------+-----+
```

### Detailed Steps

#### Step 2.1: Define AsterixMessage Class Interface

```cpp
// src/AsterixMessage.h
#ifndef ASTERIX_MESSAGE_H
#define ASTERIX_MESSAGE_H

#include <QByteArray>
#include <cstdint>

class AsterixMessage
{
public:
    // Constructor with category
    explicit AsterixMessage(uint8_t category = 0);
    
    // Build sample test messages
    static AsterixMessage createSampleCat001();
    static AsterixMessage createSampleCat034();
    static AsterixMessage createSampleCat048();
    static AsterixMessage createMinimalTestMessage();
    
    // Message manipulation
    void setCategory(uint8_t category);
    void setFspec(const QByteArray &fspec);
    void setDataItems(const QByteArray &data);
    void appendDataItem(const QByteArray &item);
    
    // Getters
    uint8_t category() const;
    uint16_t length() const;
    QByteArray fspec() const;
    QByteArray dataItems() const;
    
    // Serialize to binary format
    QByteArray toByteArray() const;
    
    // Validation
    bool isValid() const;
    QString validationError() const;
    
private:
    uint8_t m_category;
    QByteArray m_fspec;
    QByteArray m_dataItems;
    
    // Helper: Calculate total message length
    uint16_t calculateLength() const;
};

#endif // ASTERIX_MESSAGE_H
```

#### Step 2.2: Implement AsterixMessage Class

```cpp
// src/AsterixMessage.cpp
#include "AsterixMessage.h"
#include <QtEndian>

AsterixMessage::AsterixMessage(uint8_t category)
    : m_category(category)
{
}

AsterixMessage AsterixMessage::createMinimalTestMessage()
{
    // Minimal valid ASTERIX message for testing
    // CAT=1, minimal FSPEC, dummy data
    AsterixMessage msg(1);
    msg.setFspec(QByteArray::fromHex("80"));  // Single FSPEC byte
    msg.setDataItems(QByteArray::fromHex("0102030405060708"));  // 8 dummy bytes
    return msg;
}

AsterixMessage AsterixMessage::createSampleCat048()
{
    // Sample CAT048 (Monoradar Target Reports)
    // User can modify this for specific testing needs
    AsterixMessage msg(48);
    
    // FSPEC: I048/010 (Data Source Identifier) present
    msg.setFspec(QByteArray::fromHex("80"));
    
    // I048/010: SAC=0x01, SIC=0x02
    msg.setDataItems(QByteArray::fromHex("0102"));
    
    return msg;
}

uint16_t AsterixMessage::calculateLength() const
{
    // 1 (CAT) + 2 (LEN) + FSPEC length + Data Items length
    return static_cast<uint16_t>(1 + 2 + m_fspec.size() + m_dataItems.size());
}

QByteArray AsterixMessage::toByteArray() const
{
    QByteArray result;
    
    // Category (1 byte)
    result.append(static_cast<char>(m_category));
    
    // Length (2 bytes, big-endian)
    uint16_t len = calculateLength();
    result.append(static_cast<char>((len >> 8) & 0xFF));
    result.append(static_cast<char>(len & 0xFF));
    
    // FSPEC
    result.append(m_fspec);
    
    // Data Items
    result.append(m_dataItems);
    
    return result;
}

bool AsterixMessage::isValid() const
{
    return m_category > 0 && !m_fspec.isEmpty();
}

// ... remaining implementation
```

#### Step 2.3: Create Customization Point

Add clear comments in `createSampleCat048()` indicating where users should modify data for their specific tests:

```cpp
// ============================================
// USER CUSTOMIZATION SECTION
// Modify the hex values below to test different
// ASTERIX message configurations
// ============================================
```

### Deliverable

A complete `AsterixMessage` class that can create valid ASTERIX binary messages with:
- Proper CAT/LEN/FSPEC/DATA structure
- Big-endian length encoding
- Multiple sample message factory methods
- Clear customization points for user modification

### Testing Strategy

| Test | Expected Result |
|------|-----------------|
| createMinimalTestMessage() | Returns valid 12-byte message |
| toByteArray() format | CAT at byte 0, LEN at bytes 1-2 (big-endian) |
| isValid() check | Returns true for properly constructed messages |
| Length calculation | Matches actual serialized size |

---

## Phase 3: TCP Client Core Implementation

### Goal

Implement the core TCP client functionality using Qt's `QTcpSocket` class with proper connection management, data transmission, and signal/slot communication.

### Detailed Steps

#### Step 3.1: Define TcpTestClient Class Interface

```cpp
// src/TcpTestClient.h
#ifndef TCP_TEST_CLIENT_H
#define TCP_TEST_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QElapsedTimer>
#include "AsterixMessage.h"
#include "TimingStats.h"

class TcpTestClient : public QObject
{
    Q_OBJECT

public:
    // Configuration constants
    static constexpr const char* DEFAULT_HOST = "127.0.0.1";
    static constexpr quint16 DEFAULT_PORT = 3000;
    static constexpr int CONNECTION_TIMEOUT_MS = 5000;
    static constexpr int WRITE_TIMEOUT_MS = 3000;
    
    explicit TcpTestClient(QObject *parent = nullptr);
    ~TcpTestClient();
    
    // Connection management
    bool connectToServer();
    void disconnectFromServer();
    bool isConnected() const;
    
    // Message sending
    bool sendMessage(const AsterixMessage &message);
    bool sendRawData(const QByteArray &data);
    
    // Statistics
    TimingStats lastTimingStats() const;
    int totalMessagesSent() const;
    qint64 totalBytesSent() const;
    
    // Configuration
    void setHost(const QString &host);
    void setPort(quint16 port);
    QString host() const;
    quint16 port() const;

signals:
    void connected();
    void disconnected();
    void messageSent(const QByteArray &data, qint64 elapsedMs);
    void errorOccurred(const QString &errorString);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onBytesWritten(qint64 bytes);

private:
    QTcpSocket *m_socket;
    QString m_host;
    quint16 m_port;
    
    // Statistics tracking
    int m_messageCount;
    qint64 m_totalBytes;
    TimingStats m_lastStats;
    QElapsedTimer m_connectionTimer;
    QElapsedTimer m_sendTimer;
    
    // State
    bool m_waitingForWrite;
    qint64 m_pendingBytes;
};

#endif // TCP_TEST_CLIENT_H
```

#### Step 3.2: Implement Connection Logic

```cpp
// src/TcpTestClient.cpp (partial)
#include "TcpTestClient.h"
#include <QEventLoop>
#include <QTimer>

TcpTestClient::TcpTestClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_host(DEFAULT_HOST)
    , m_port(DEFAULT_PORT)
    , m_messageCount(0)
    , m_totalBytes(0)
    , m_waitingForWrite(false)
    , m_pendingBytes(0)
{
    // Connect signals
    connect(m_socket, &QTcpSocket::connected,
            this, &TcpTestClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected,
            this, &TcpTestClient::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &TcpTestClient::onError);
    connect(m_socket, &QTcpSocket::bytesWritten,
            this, &TcpTestClient::onBytesWritten);
}

bool TcpTestClient::connectToServer()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        return true;
    }
    
    m_connectionTimer.start();
    m_socket->connectToHost(m_host, m_port);
    
    // Synchronous wait with timeout
    if (!m_socket->waitForConnected(CONNECTION_TIMEOUT_MS)) {
        emit errorOccurred(QString("Connection timeout: %1")
                          .arg(m_socket->errorString()));
        return false;
    }
    
    m_lastStats.connectionTimeMs = m_connectionTimer.elapsed();
    return true;
}

bool TcpTestClient::sendMessage(const AsterixMessage &message)
{
    if (!message.isValid()) {
        emit errorOccurred("Invalid ASTERIX message");
        return false;
    }
    return sendRawData(message.toByteArray());
}

bool TcpTestClient::sendRawData(const QByteArray &data)
{
    if (!isConnected()) {
        emit errorOccurred("Not connected to server");
        return false;
    }
    
    m_sendTimer.start();
    qint64 bytesWritten = m_socket->write(data);
    
    if (bytesWritten == -1) {
        emit errorOccurred(QString("Write failed: %1")
                          .arg(m_socket->errorString()));
        return false;
    }
    
    // Wait for data to be written
    if (!m_socket->waitForBytesWritten(WRITE_TIMEOUT_MS)) {
        emit errorOccurred("Write timeout");
        return false;
    }
    
    m_lastStats.sendTimeMs = m_sendTimer.elapsed();
    m_lastStats.bytesSent = bytesWritten;
    m_messageCount++;
    m_totalBytes += bytesWritten;
    
    emit messageSent(data, m_lastStats.sendTimeMs);
    return true;
}
```

#### Step 3.3: Implement Error Handling Slots

```cpp
void TcpTestClient::onError(QAbstractSocket::SocketError socketError)
{
    QString errorMsg;
    switch (socketError) {
        case QAbstractSocket::ConnectionRefusedError:
            errorMsg = "Connection refused - is the server running?";
            break;
        case QAbstractSocket::HostNotFoundError:
            errorMsg = "Host not found";
            break;
        case QAbstractSocket::SocketTimeoutError:
            errorMsg = "Connection timed out";
            break;
        case QAbstractSocket::NetworkError:
            errorMsg = "Network error";
            break;
        default:
            errorMsg = m_socket->errorString();
    }
    emit errorOccurred(errorMsg);
}
```

### Deliverable

A fully functional `TcpTestClient` class with:
- Synchronous connection with timeout
- Message sending with write confirmation
- Proper signal/slot architecture for status updates
- Comprehensive error handling and reporting
- Statistics tracking for timing measurements

### Testing Strategy

| Test | Expected Result |
|------|-----------------|
| Connect to running server | Returns true, connected signal emitted |
| Connect to non-existent server | Returns false after timeout, error emitted |
| Send valid message | Returns true, messageSent signal emitted |
| Send while disconnected | Returns false, error emitted |

---

## Phase 4: Timing & Measurement

### Goal

Implement comprehensive timing measurement and statistics tracking for connection establishment and message transmission, with formatted output display.

### Detailed Steps

#### Step 4.1: Define TimingStats Structure

```cpp
// src/TimingStats.h
#ifndef TIMING_STATS_H
#define TIMING_STATS_H

#include <QString>
#include <QVector>
#include <cstdint>

struct TimingStats
{
    qint64 connectionTimeMs = 0;  // Time to establish connection
    qint64 sendTimeMs = 0;        // Time to send and confirm write
    qint64 bytesSent = 0;         // Bytes sent in this operation
    qint64 timestamp = 0;         // Unix timestamp of operation
    
    // Format as human-readable string
    QString toString() const;
    
    // Calculate throughput in bytes/second
    double throughputBps() const;
};

class TimingStatsCollector
{
public:
    TimingStatsCollector();
    
    // Record a new timing measurement
    void record(const TimingStats &stats);
    
    // Clear all recorded stats
    void clear();
    
    // Statistical calculations
    int count() const;
    qint64 totalBytesSent() const;
    
    // Connection time statistics
    double avgConnectionTimeMs() const;
    qint64 minConnectionTimeMs() const;
    qint64 maxConnectionTimeMs() const;
    
    // Send time statistics
    double avgSendTimeMs() const;
    qint64 minSendTimeMs() const;
    qint64 maxSendTimeMs() const;
    
    // Generate formatted report
    QString generateReport() const;
    
private:
    QVector<TimingStats> m_stats;
    qint64 m_totalBytes;
};

#endif // TIMING_STATS_H
```

#### Step 4.2: Implement Statistics Collection

```cpp
// src/TimingStats.cpp
#include "TimingStats.h"
#include <QDateTime>
#include <algorithm>
#include <numeric>

QString TimingStats::toString() const
{
    return QString("Connection: %1ms | Send: %2ms | Bytes: %3 | Throughput: %4 KB/s")
        .arg(connectionTimeMs)
        .arg(sendTimeMs)
        .arg(bytesSent)
        .arg(throughputBps() / 1024.0, 0, 'f', 2);
}

double TimingStats::throughputBps() const
{
    if (sendTimeMs <= 0) return 0.0;
    return (bytesSent * 1000.0) / sendTimeMs;
}

QString TimingStatsCollector::generateReport() const
{
    if (m_stats.isEmpty()) {
        return "No statistics recorded.";
    }
    
    QString report;
    report += QString("\n╔══════════════════════════════════════════╗\n");
    report += QString("║         TIMING STATISTICS REPORT         ║\n");
    report += QString("╠══════════════════════════════════════════╣\n");
    report += QString("║ Total Messages Sent: %1\n").arg(count(), 19);
    report += QString("║ Total Bytes Sent:    %1\n").arg(totalBytesSent(), 19);
    report += QString("╠══════════════════════════════════════════╣\n");
    report += QString("║ CONNECTION TIME (ms)                     ║\n");
    report += QString("║   Average: %1\n").arg(avgConnectionTimeMs(), 10, 'f', 2);
    report += QString("║   Min:     %1\n").arg(minConnectionTimeMs(), 10);
    report += QString("║   Max:     %1\n").arg(maxConnectionTimeMs(), 10);
    report += QString("╠══════════════════════════════════════════╣\n");
    report += QString("║ SEND TIME (ms)                           ║\n");
    report += QString("║   Average: %1\n").arg(avgSendTimeMs(), 10, 'f', 2);
    report += QString("║   Min:     %1\n").arg(minSendTimeMs(), 10);
    report += QString("║   Max:     %1\n").arg(maxSendTimeMs(), 10);
    report += QString("╚══════════════════════════════════════════╝\n");
    
    return report;
}
```

#### Step 4.3: Implement QElapsedTimer Usage

Ensure high-resolution timing using `QElapsedTimer` with `nsecsElapsed()` for sub-millisecond precision when needed:

```cpp
// In TcpTestClient::sendRawData()
m_sendTimer.start();
qint64 bytesWritten = m_socket->write(data);
// ... wait for completion
qint64 elapsedNs = m_sendTimer.nsecsElapsed();
m_lastStats.sendTimeMs = elapsedNs / 1000000; // Convert to ms
m_lastStats.sendTimeUs = elapsedNs / 1000;    // Or keep microseconds for precision
```

### Deliverable

Complete timing infrastructure with:
- Per-operation timing (connection, send)
- Aggregate statistics (min, max, average)
- Throughput calculation
- Formatted statistical report generation
- High-resolution timing support

### Testing Strategy

| Test | Expected Result |
|------|-----------------|
| Single operation timing | Returns positive, reasonable values |
| Statistics aggregation | Correct min/max/avg calculations |
| Report generation | Properly formatted output string |
| Throughput calculation | Accurate bytes-per-second value |

---

## Phase 5: Interactive Testing Loop

### Goal

Implement an interactive console menu system that allows repeated test execution, configuration changes, and statistics viewing without restarting the application.

### Detailed Steps

#### Step 5.1: Define Menu System

```cpp
// In main.cpp or separate InteractiveMenu class

void printMenu()
{
    QTextStream cout(stdout);
    cout << "\n╔═══════════════════════════════════════════╗" << Qt::endl;
    cout << "║     ASTERIX TCP Test Client - Menu        ║" << Qt::endl;
    cout << "╠═══════════════════════════════════════════╣" << Qt::endl;
    cout << "║  [1] Send single test message             ║" << Qt::endl;
    cout << "║  [2] Send N messages (batch test)         ║" << Qt::endl;
    cout << "║  [3] Show last message hex dump           ║" << Qt::endl;
    cout << "║  [4] Show timing statistics               ║" << Qt::endl;
    cout << "║  [5] Reset statistics                     ║" << Qt::endl;
    cout << "║  [6] Test connection                      ║" << Qt::endl;
    cout << "║  [7] Change message (select sample)       ║" << Qt::endl;
    cout << "║  [0] Exit                                 ║" << Qt::endl;
    cout << "╚═══════════════════════════════════════════╝" << Qt::endl;
    cout << "Enter choice: " << Qt::flush;
}
```

#### Step 5.2: Implement Interactive Loop

```cpp
void runInteractiveLoop(TcpTestClient &client, TimingStatsCollector &collector)
{
    QTextStream cin(stdin);
    QTextStream cout(stdout);
    
    AsterixMessage currentMessage = AsterixMessage::createMinimalTestMessage();
    QByteArray lastSentData;
    bool running = true;
    
    while (running) {
        printMenu();
        QString input = cin.readLine().trimmed();
        
        switch (input.toInt()) {
            case 1: // Send single message
                handleSendSingle(client, currentMessage, lastSentData, collector, cout);
                break;
                
            case 2: // Batch send
                handleBatchSend(client, currentMessage, collector, cin, cout);
                break;
                
            case 3: // Show hex dump
                handleShowHexDump(lastSentData, cout);
                break;
                
            case 4: // Show statistics
                cout << collector.generateReport() << Qt::endl;
                break;
                
            case 5: // Reset statistics
                collector.clear();
                cout << "Statistics reset." << Qt::endl;
                break;
                
            case 6: // Test connection
                handleTestConnection(client, cout);
                break;
                
            case 7: // Change message
                handleChangeMessage(currentMessage, cin, cout);
                break;
                
            case 0: // Exit
                running = false;
                cout << "Exiting..." << Qt::endl;
                break;
                
            default:
                cout << "Invalid choice. Try again." << Qt::endl;
        }
    }
}
```

#### Step 5.3: Implement Handler Functions

```cpp
void handleSendSingle(TcpTestClient &client, 
                      const AsterixMessage &msg,
                      QByteArray &lastSentData,
                      TimingStatsCollector &collector,
                      QTextStream &cout)
{
    cout << "\nConnecting to " << client.host() << ":" << client.port() << "..." << Qt::endl;
    
    if (!client.connectToServer()) {
        cout << "[ERROR] Connection failed!" << Qt::endl;
        return;
    }
    
    cout << "Connected! Sending message..." << Qt::endl;
    
    QByteArray data = msg.toByteArray();
    if (client.sendRawData(data)) {
        lastSentData = data;
        TimingStats stats = client.lastTimingStats();
        collector.record(stats);
        
        cout << "[SUCCESS] Message sent!" << Qt::endl;
        cout << stats.toString() << Qt::endl;
        cout << "\nHex dump of sent data:" << Qt::endl;
        cout << HexDumper::dump(data) << Qt::endl;
    } else {
        cout << "[ERROR] Send failed!" << Qt::endl;
    }
    
    client.disconnectFromServer();
}

void handleBatchSend(TcpTestClient &client,
                     const AsterixMessage &msg,
                     TimingStatsCollector &collector,
                     QTextStream &cin,
                     QTextStream &cout)
{
    cout << "Enter number of messages to send: " << Qt::flush;
    int count = cin.readLine().trimmed().toInt();
    
    if (count <= 0 || count > 10000) {
        cout << "Invalid count (1-10000)." << Qt::endl;
        return;
    }
    
    cout << "Keep connection open between sends? (y/n): " << Qt::flush;
    bool keepOpen = cin.readLine().trimmed().toLower() == "y";
    
    int success = 0, failed = 0;
    QElapsedTimer batchTimer;
    batchTimer.start();
    
    // ... batch sending logic with progress display
    
    cout << QString("\nBatch complete: %1 sent, %2 failed, total time: %3ms")
            .arg(success).arg(failed).arg(batchTimer.elapsed()) << Qt::endl;
}
```

### Deliverable

A complete interactive console interface with:
- Clear menu navigation
- Single and batch message sending
- On-demand hex dump display
- Statistics viewing and reset
- Connection testing
- Message type selection

### Testing Strategy

| Test | Expected Result |
|------|-----------------|
| Menu display | Clean, aligned formatting |
| Input validation | Rejects invalid choices gracefully |
| Batch send | Correct success/fail counts |
| Statistics persistence | Survives multiple menu operations |

---

## Phase 6: Error Handling & Logging

### Goal

Implement comprehensive error handling with descriptive messages, proper resource cleanup, and optional logging to file for debugging purposes.

### Detailed Steps

#### Step 6.1: Define Error Categories

```cpp
// Error code enumeration for programmatic handling
enum class TcpClientError {
    None = 0,
    ConnectionRefused,
    ConnectionTimeout,
    HostNotFound,
    NetworkError,
    WriteError,
    WriteTimeout,
    InvalidMessage,
    NotConnected,
    Unknown
};

struct ErrorInfo {
    TcpClientError code;
    QString message;
    QString details;
    QDateTime timestamp;
    
    QString toString() const;
};
```

#### Step 6.2: Implement Logging Infrastructure

```cpp
// Simple logging class (no external dependencies)
class SimpleLogger
{
public:
    enum Level { Debug, Info, Warning, Error };
    
    static void setLogLevel(Level level);
    static void setLogFile(const QString &filename);
    static void enableConsoleOutput(bool enable);
    
    static void debug(const QString &message);
    static void info(const QString &message);
    static void warning(const QString &message);
    static void error(const QString &message);
    
private:
    static void log(Level level, const QString &message);
    
    static Level s_level;
    static QString s_filename;
    static bool s_consoleEnabled;
};

// Usage in TcpTestClient
void TcpTestClient::onError(QAbstractSocket::SocketError socketError)
{
    QString errorMsg = translateSocketError(socketError);
    SimpleLogger::error(QString("Socket error: %1").arg(errorMsg));
    emit errorOccurred(errorMsg);
}
```

#### Step 6.3: Implement Resource Cleanup

```cpp
TcpTestClient::~TcpTestClient()
{
    SimpleLogger::debug("TcpTestClient destructor called");
    
    if (m_socket) {
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->disconnectFromHost();
            if (m_socket->state() != QAbstractSocket::UnconnectedState) {
                m_socket->waitForDisconnected(1000);
            }
        }
        // Note: m_socket deleted by Qt parent-child relationship
    }
    
    SimpleLogger::info("TcpTestClient destroyed cleanly");
}
```

#### Step 6.4: Implement Graceful Exit Handling

```cpp
// In main.cpp
#include <csignal>

static TcpTestClient *g_client = nullptr;

void signalHandler(int signal)
{
    QTextStream cout(stdout);
    cout << "\nReceived signal " << signal << ", shutting down..." << Qt::endl;
    
    if (g_client) {
        g_client->disconnectFromServer();
    }
    
    QCoreApplication::quit();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // Install signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    TcpTestClient client;
    g_client = &client;
    
    // ... rest of initialization
}
```

### Deliverable

Robust error handling system with:
- Categorized error codes
- Descriptive error messages
- Optional file logging
- Proper resource cleanup
- Graceful signal handling (Ctrl+C)

### Testing Strategy

| Test | Expected Result |
|------|-----------------|
| Server not running | Clear "connection refused" message |
| Invalid host | Clear "host not found" message |
| Ctrl+C during operation | Clean shutdown, no resource leaks |
| Log file output | Properly formatted timestamps and levels |

---

## Phase 7: Documentation & Build Instructions

### Goal

Create comprehensive documentation including build instructions, usage guide, and code documentation to ensure the project is maintainable and usable by others.

### Detailed Steps

#### Step 7.1: Create README.md

```markdown
# ASTERIX TCP Test Client

A Qt-based console application for testing ASTERIX message transmission
to TCP servers.

## Features

- Send ASTERIX CAT messages to TCP server
- Hex dump display of sent data
- Timing measurements (connection, transmission)
- Interactive testing with batch support
- Detailed statistics reporting

## Requirements

- Qt 5.14 or later
- CMake 3.10 or later
- C++11 compatible compiler

## Building

### Linux/macOS

```bash
mkdir build && cd build
cmake ..
make
```

### Windows (Visual Studio)

```cmd
mkdir build && cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release
```

## Usage

```bash
./bin/asterix-tcp-client
```

The application will present an interactive menu for testing.

## Customizing Test Messages

Edit `src/AsterixMessage.cpp` and modify the `createSampleCat048()` 
function to create your specific test messages.

## Configuration

Default settings (hardcoded):
- Host: 127.0.0.1
- Port: 3000

## License

[Your License]
```

#### Step 7.2: Add Code Documentation

Use Doxygen-style comments for all public interfaces:

```cpp
/**
 * @brief TCP test client for ASTERIX message transmission
 * 
 * This class provides functionality to connect to a TCP server
 * and send ASTERIX-formatted binary messages for testing purposes.
 * 
 * @note This is a send-only client; no receive functionality is implemented.
 * 
 * Example usage:
 * @code
 * TcpTestClient client;
 * if (client.connectToServer()) {
 *     AsterixMessage msg = AsterixMessage::createSampleCat048();
 *     client.sendMessage(msg);
 *     client.disconnectFromServer();
 * }
 * @endcode
 */
class TcpTestClient : public QObject
```

#### Step 7.3: Create Usage Documentation

Create `docs/usage.md` with detailed usage examples, screenshots of expected output, and troubleshooting guide.

### Deliverable

Complete documentation package:
- README.md with quick start guide
- Doxygen-compatible code comments
- Usage documentation with examples
- Troubleshooting section

---

## Summary Timeline

| Phase | Description | Estimated Time | Dependencies |
|-------|-------------|----------------|--------------|
| 1 | Project Structure Setup | 1-2 hours | None |
| 2 | ASTERIX Message Structure | 2-3 hours | Phase 1 |
| 3 | TCP Client Core Implementation | 3-4 hours | Phases 1, 2 |
| 4 | Timing & Measurement | 2-3 hours | Phase 3 |
| 5 | Interactive Testing Loop | 2-3 hours | Phases 3, 4 |
| 6 | Error Handling & Logging | 2-3 hours | Phases 3, 5 |
| 7 | Documentation & Build Instructions | 1-2 hours | All phases |
| **Total** | | **13-20 hours** | |

---

## Complete Testing Strategy

### Unit Testing Approach

While not implementing a full unit test framework, manual testing should cover:

| Component | Test Cases |
|-----------|------------|
| AsterixMessage | Serialization, length calculation, validation |
| TcpTestClient | Connection, disconnection, send operations |
| TimingStats | Statistical calculations, report formatting |
| HexDumper | Various data sizes, alignment |
| Interactive Loop | Menu navigation, input validation |

### Integration Testing

1. Start the target TCP server on localhost:3000
2. Run the test client
3. Verify received messages on server side
4. Compare hex dumps for data integrity

### Stress Testing

Use batch mode to send 1000+ messages and verify:
- Memory usage stability
- Connection reliability
- Statistics accuracy

---

## Hex Dumper Utility (Bonus Component)

```cpp
// src/HexDumper.h
#ifndef HEX_DUMPER_H
#define HEX_DUMPER_H

#include <QString>
#include <QByteArray>

class HexDumper
{
public:
    /**
     * @brief Generate formatted hex dump of binary data
     * @param data Binary data to dump
     * @param bytesPerLine Bytes to show per line (default 16)
     * @return Formatted string with offset, hex, and ASCII columns
     * 
     * Output format:
     * 00000000  48 65 6C 6C 6F 20 57 6F  72 6C 64 21 00 00 00 00  |Hello World!....|
     */
    static QString dump(const QByteArray &data, int bytesPerLine = 16);
    
    /**
     * @brief Convert byte array to simple hex string
     * @param data Binary data
     * @param separator Separator between bytes (default space)
     * @return Hex string like "48 65 6C 6C 6F"
     */
    static QString toHexString(const QByteArray &data, 
                               const QString &separator = " ");
};

#endif // HEX_DUMPER_H
```

---

## Suggested Improvements for Future Versions

| Feature | Priority | Description |
|---------|----------|-------------|
| Configuration File | Medium | Load host/port from config file instead of hardcoded |
| Command-Line Arguments | Medium | Support `--host`, `--port`, `--message-file` options |
| Message from File | High | Load ASTERIX message hex from external file |
| Response Handling | Low | Optional receive and display server responses |
| Multiple Server Support | Low | Test against multiple endpoints |
| GUI Version | Low | Optional Qt Widgets interface |
| Wireshark Integration | Medium | PCAP file generation for analysis |

---

## Appendix A: ASTERIX Reference

### Common Category Numbers

| CAT | Description |
|-----|-------------|
| 001 | Monoradar Target Reports (Plot) |
| 002 | Monoradar Service Messages |
| 034 | Monoradar Service Messages (Enhanced) |
| 048 | Monoradar Target Reports |
| 062 | SDPS Track Messages |
| 063 | Sensor Status Messages |

### Message Structure

```
┌────────┬─────────────┬──────────────┬──────────────┐
│  CAT   │     LEN     │    FSPEC     │  Data Items  │
│ 1 byte │   2 bytes   │  Variable    │   Variable   │
│        │ Big-endian  │              │              │
└────────┴─────────────┴──────────────┴──────────────┘
```

---

*Document Version: 1.0*  
*Last Updated: January 2026*
