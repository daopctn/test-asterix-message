#ifndef TIMING_STATS_H
#define TIMING_STATS_H

#include <QString>
#include <QVector>
#include <cstdint>

struct TimingStats
{
    qint64 connectionTimeMs = 0;
    qint64 sendTimeMs = 0;
    qint64 bytesSent = 0;
    qint64 timestamp = 0;

    QString toString() const;
    double throughputBps() const;
};

class TimingStatsCollector
{
public:
    TimingStatsCollector();

    void record(const TimingStats &stats);
    void clear();

    int count() const;
    qint64 totalBytesSent() const;

    double avgConnectionTimeMs() const;
    qint64 minConnectionTimeMs() const;
    qint64 maxConnectionTimeMs() const;

    double avgSendTimeMs() const;
    qint64 minSendTimeMs() const;
    qint64 maxSendTimeMs() const;

    QString generateReport() const;

private:
    QVector<TimingStats> m_stats;
    qint64 m_totalBytes;
};

#endif // TIMING_STATS_H
