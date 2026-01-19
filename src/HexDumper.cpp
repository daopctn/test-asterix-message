#include "HexDumper.h"
#include <QTextStream>

QString HexDumper::dump(const QByteArray &data, int bytesPerLine)
{
    QString result;
    QTextStream stream(&result);

    for (int offset = 0; offset < data.size(); offset += bytesPerLine) {
        // Offset column
        stream << QString("%1  ").arg(offset, 8, 16, QChar('0'));

        // Hex column (two groups of 8 bytes)
        for (int i = 0; i < bytesPerLine; ++i) {
            if (offset + i < data.size()) {
                stream << QString("%1 ").arg(static_cast<unsigned char>(data[offset + i]), 2, 16, QChar('0'));
            } else {
                stream << "   ";
            }

            // Add extra space between groups
            if (i == 7) {
                stream << " ";
            }
        }

        stream << " |";

        // ASCII column
        for (int i = 0; i < bytesPerLine && offset + i < data.size(); ++i) {
            unsigned char c = static_cast<unsigned char>(data[offset + i]);
            if (c >= 32 && c <= 126) {
                stream << c;
            } else {
                stream << '.';
            }
        }

        stream << "|";
        if (offset + bytesPerLine < data.size()) {
            stream << "\n";
        }
    }

    return result;
}

QString HexDumper::toHexString(const QByteArray &data, const QString &separator)
{
    QString result;
    for (int i = 0; i < data.size(); ++i) {
        if (i > 0) {
            result += separator;
        }
        result += QString("%1").arg(static_cast<unsigned char>(data[i]), 2, 16, QChar('0'));
    }
    return result;
}
