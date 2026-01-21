#include <QCoreApplication>
#include <QTextStream>
#include <QTimer>
#include "TcpTestClient.h"
#include "AsterixMessage.h"
#include "HexDumper.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QTextStream cout(stdout);

    cout << "=== Simple ASTERIX Client ===" << Qt::endl;
    cout << "Connecting to 127.0.0.1:3000..." << Qt::endl;

    // Create client
    TcpTestClient client;

    // Connect to server
    if (!client.connectToServer()) {
        cout << "[ERROR] Failed to connect to server!" << Qt::endl;
        cout << "Make sure the server is running: ./build/bin/asterix-tcp-server" << Qt::endl;
        return 1;
    }

    cout << "[SUCCESS] Connected to server!" << Qt::endl << Qt::endl;

    // Send a few different message types
    cout << "Sending CAT 001 message..." << Qt::endl;
    AsterixMessage msg1 = AsterixMessage::createMinimalTestMessage();
    if (client.sendMessage(msg1)) {
        cout << "  Sent: " << msg1.toByteArray().size() << " bytes" << Qt::endl;
        cout << "  Hex: " << HexDumper::toHexString(msg1.toByteArray()) << Qt::endl;
    }

    cout << Qt::endl;
    cout << "Sending CAT 048 message..." << Qt::endl;
    AsterixMessage msg2 = AsterixMessage::createSampleCat048();
    if (client.sendMessage(msg2)) {
        cout << "  Sent: " << msg2.toByteArray().size() << " bytes" << Qt::endl;
        cout << "  Hex: " << HexDumper::toHexString(msg2.toByteArray()) << Qt::endl;
    }

    cout << Qt::endl;
    cout << "Sending CAT 034 message..." << Qt::endl;
    AsterixMessage msg3 = AsterixMessage::createSampleCat034();
    if (client.sendMessage(msg3)) {
        cout << "  Sent: " << msg3.toByteArray().size() << " bytes" << Qt::endl;
        cout << "  Hex: " << HexDumper::toHexString(msg3.toByteArray()) << Qt::endl;
    }

    // Show statistics
    cout << Qt::endl;
    cout << "[STATISTICS]" << Qt::endl;
    cout << "  Total messages sent: " << client.totalMessagesSent() << Qt::endl;
    cout << "  Total bytes sent:    " << client.totalBytesSent() << Qt::endl;

    // Disconnect
    client.disconnectFromServer();
    cout << Qt::endl << "Disconnected. Done!" << Qt::endl;

    return 0;
}
