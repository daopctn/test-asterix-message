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
    static QString toHexString(const QByteArray &data, const QString &separator = " ");
};

#endif // HEX_DUMPER_H
