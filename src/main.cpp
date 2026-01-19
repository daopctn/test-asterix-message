#include <QCoreApplication>
#include <QTextStream>
#include <csignal>
#include "TcpTestClient.h"
#include "AsterixMessage.h"
#include "HexDumper.h"
#include "TimingStats.h"

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

    QByteArray data = msg.toByteArray();

    if (keepOpen) {
        cout << "Connecting to " << client.host() << ":" << client.port() << "..." << Qt::endl;
        if (!client.connectToServer()) {
            cout << "[ERROR] Connection failed!" << Qt::endl;
            return;
        }
        cout << "Connected! Sending messages..." << Qt::endl;
    }

    for (int i = 0; i < count; ++i) {
        if (!keepOpen) {
            if (!client.connectToServer()) {
                failed++;
                continue;
            }
        }

        if (client.sendRawData(data)) {
            success++;
            TimingStats stats = client.lastTimingStats();
            collector.record(stats);

            if ((i + 1) % 10 == 0 || i == count - 1) {
                cout << QString("Progress: %1/%2 messages sent\r").arg(i + 1).arg(count) << Qt::flush;
            }
        } else {
            failed++;
        }

        if (!keepOpen) {
            client.disconnectFromServer();
        }
    }

    if (keepOpen) {
        client.disconnectFromServer();
    }

    cout << QString("\n\nBatch complete: %1 sent, %2 failed, total time: %3ms")
            .arg(success).arg(failed).arg(batchTimer.elapsed()) << Qt::endl;
}

void handleShowHexDump(const QByteArray &lastSentData, QTextStream &cout)
{
    if (lastSentData.isEmpty()) {
        cout << "No message sent yet." << Qt::endl;
    } else {
        cout << "\nLast sent message hex dump:" << Qt::endl;
        cout << HexDumper::dump(lastSentData) << Qt::endl;
    }
}

void handleTestConnection(TcpTestClient &client, QTextStream &cout)
{
    cout << "\nTesting connection to " << client.host() << ":" << client.port() << "..." << Qt::endl;

    if (client.connectToServer()) {
        cout << "[SUCCESS] Connected successfully!" << Qt::endl;
        TimingStats stats = client.lastTimingStats();
        cout << QString("Connection time: %1ms").arg(stats.connectionTimeMs) << Qt::endl;
        client.disconnectFromServer();
        cout << "Disconnected." << Qt::endl;
    } else {
        cout << "[ERROR] Connection failed!" << Qt::endl;
    }
}

void handleChangeMessage(AsterixMessage &currentMessage, QTextStream &cin, QTextStream &cout)
{
    cout << "\nAvailable sample messages:" << Qt::endl;
    cout << "  [1] Minimal Test Message (CAT 1)" << Qt::endl;
    cout << "  [2] Sample CAT 001" << Qt::endl;
    cout << "  [3] Sample CAT 034" << Qt::endl;
    cout << "  [4] Sample CAT 048" << Qt::endl;
    cout << "Enter choice: " << Qt::flush;

    int choice = cin.readLine().trimmed().toInt();

    switch (choice) {
        case 1:
            currentMessage = AsterixMessage::createMinimalTestMessage();
            cout << "Switched to: Minimal Test Message (CAT 1)" << Qt::endl;
            break;
        case 2:
            currentMessage = AsterixMessage::createSampleCat001();
            cout << "Switched to: Sample CAT 001" << Qt::endl;
            break;
        case 3:
            currentMessage = AsterixMessage::createSampleCat034();
            cout << "Switched to: Sample CAT 034" << Qt::endl;
            break;
        case 4:
            currentMessage = AsterixMessage::createSampleCat048();
            cout << "Switched to: Sample CAT 048" << Qt::endl;
            break;
        default:
            cout << "Invalid choice." << Qt::endl;
            return;
    }

    cout << "\nMessage preview:" << Qt::endl;
    QByteArray data = currentMessage.toByteArray();
    cout << HexDumper::dump(data) << Qt::endl;
}

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
            case 1:
                handleSendSingle(client, currentMessage, lastSentData, collector, cout);
                break;

            case 2:
                handleBatchSend(client, currentMessage, collector, cin, cout);
                break;

            case 3:
                handleShowHexDump(lastSentData, cout);
                break;

            case 4:
                cout << collector.generateReport() << Qt::endl;
                break;

            case 5:
                collector.clear();
                cout << "Statistics reset." << Qt::endl;
                break;

            case 6:
                handleTestConnection(client, cout);
                break;

            case 7:
                handleChangeMessage(currentMessage, cin, cout);
                break;

            case 0:
                running = false;
                cout << "Exiting..." << Qt::endl;
                break;

            default:
                cout << "Invalid choice. Try again." << Qt::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("asterix-tcp-client");
    QCoreApplication::setApplicationVersion("1.0.0");

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    QTextStream cout(stdout);
    cout << "=== ASTERIX TCP Test Client v1.0 ===" << Qt::endl;
    cout << "Qt Version: " << QT_VERSION_STR << Qt::endl;
    cout << "Target: 127.0.0.1:3000" << Qt::endl;

    TcpTestClient client;
    g_client = &client;

    TimingStatsCollector collector;

    runInteractiveLoop(client, collector);

    return 0;
}
