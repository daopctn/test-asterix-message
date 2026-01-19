#include "TimingStats.h"
#include <QDateTime>
#include <algorithm>
#include <limits>

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

TimingStatsCollector::TimingStatsCollector()
    : m_totalBytes(0)
{
}

void TimingStatsCollector::record(const TimingStats &stats)
{
    m_stats.append(stats);
    m_totalBytes += stats.bytesSent;
}

void TimingStatsCollector::clear()
{
    m_stats.clear();
    m_totalBytes = 0;
}

int TimingStatsCollector::count() const
{
    return m_stats.size();
}

qint64 TimingStatsCollector::totalBytesSent() const
{
    return m_totalBytes;
}

double TimingStatsCollector::avgConnectionTimeMs() const
{
    if (m_stats.isEmpty()) return 0.0;

    qint64 sum = 0;
    for (const auto &stat : m_stats) {
        sum += stat.connectionTimeMs;
    }
    return static_cast<double>(sum) / m_stats.size();
}

qint64 TimingStatsCollector::minConnectionTimeMs() const
{
    if (m_stats.isEmpty()) return 0;

    qint64 minTime = std::numeric_limits<qint64>::max();
    for (const auto &stat : m_stats) {
        if (stat.connectionTimeMs < minTime) {
            minTime = stat.connectionTimeMs;
        }
    }
    return minTime;
}

qint64 TimingStatsCollector::maxConnectionTimeMs() const
{
    if (m_stats.isEmpty()) return 0;

    qint64 maxTime = 0;
    for (const auto &stat : m_stats) {
        if (stat.connectionTimeMs > maxTime) {
            maxTime = stat.connectionTimeMs;
        }
    }
    return maxTime;
}

double TimingStatsCollector::avgSendTimeMs() const
{
    if (m_stats.isEmpty()) return 0.0;

    qint64 sum = 0;
    for (const auto &stat : m_stats) {
        sum += stat.sendTimeMs;
    }
    return static_cast<double>(sum) / m_stats.size();
}

qint64 TimingStatsCollector::minSendTimeMs() const
{
    if (m_stats.isEmpty()) return 0;

    qint64 minTime = std::numeric_limits<qint64>::max();
    for (const auto &stat : m_stats) {
        if (stat.sendTimeMs < minTime) {
            minTime = stat.sendTimeMs;
        }
    }
    return minTime;
}

qint64 TimingStatsCollector::maxSendTimeMs() const
{
    if (m_stats.isEmpty()) return 0;

    qint64 maxTime = 0;
    for (const auto &stat : m_stats) {
        if (stat.sendTimeMs > maxTime) {
            maxTime = stat.sendTimeMs;
        }
    }
    return maxTime;
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
