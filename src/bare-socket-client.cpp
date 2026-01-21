#include <QCoreApplication>
#include <QTcpSocket>
#include <QTextStream>
#include <QByteArray>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QTextStream cout(stdout);

    cout << "=== Simple Socket Client ===" << Qt::endl;

    // Create socket
    QTcpSocket socket;

    // Connect to server
    cout << "Connecting to 127.0.0.1:3000..." << Qt::endl;
    socket.connectToHost("127.0.0.1", 3000);

    if (!socket.waitForConnected(5000)) {
        cout << "[ERROR] Connection failed: " << socket.errorString() << Qt::endl;
        return 1;
    }

    cout << "[SUCCESS] Connected!" << Qt::endl << Qt::endl;

    // Send custom data - ASTERIX CAT 048 message
    // Format: CAT(1 byte) + LEN(2 bytes big-endian) + FSPEC(1+ bytes) + DATA
    QByteArray data;
    data.append(char(0x30));        // CAT = 48 (0x30)
    data.append(char(0x00));        // LEN high byte
    data.append(char(0x06));        // LEN low byte (total 6 bytes)
    data.append(char(0x80));        // FSPEC
    data.append(char(0x01));        // Data byte 1
    data.append(char(0x02));        // Data byte 2

    cout << "Sending " << data.size() << " bytes..." << Qt::endl;
    cout << "Hex: ";
    for (int i = 0; i < data.size(); ++i) {
        cout << QString("%1 ").arg((unsigned char)data[i], 2, 16, QChar('0'));
    }
    cout << Qt::endl;

    // Send the data
    qint64 written = socket.write(data);
    if (written == -1) {
        cout << "[ERROR] Write failed!" << Qt::endl;
        return 1;
    }

    socket.waitForBytesWritten(3000);
    cout << "[SUCCESS] Sent " << written << " bytes" << Qt::endl << Qt::endl;

    // You can send more custom data here
    // Example: Send another message
    QByteArray data2;
    data2.append(char(0x01));        // CAT = 1
    data2.append(char(0x00));        // LEN high
    data2.append(char(0x0C));        // LEN low (12 bytes)
    data2.append(char(0x80));        // FSPEC
    data2.append(char(0x11));        // Data bytes
    data2.append(char(0x22));
    data2.append(char(0x33));
    data2.append(char(0x44));
    data2.append(char(0x55));
    data2.append(char(0x66));
    data2.append(char(0x77));
    data2.append(char(0x88));

    cout << "Sending another message: " << data2.size() << " bytes..." << Qt::endl;
    socket.write(data2);
    socket.waitForBytesWritten(3000);
    cout << "[SUCCESS] Sent!" << Qt::endl << Qt::endl;

    // Close connection
    socket.disconnectFromHost();
    if (socket.state() != QAbstractSocket::UnconnectedState) {
        socket.waitForDisconnected(1000);
    }

    cout << "Disconnected. Done!" << Qt::endl;

    return 0;
}
