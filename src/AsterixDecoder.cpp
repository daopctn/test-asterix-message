#include "AsterixDecoder.h"
#include "HexDumper.h"
#include <QTextStream>

QMap<uint8_t, QString> AsterixDecoder::s_categoryNames;

void AsterixDecoder::initCategoryNames()
{
    if (!s_categoryNames.isEmpty()) {
        return;
    }

    s_categoryNames[1] = "Monoradar Target Reports (Plot)";
    s_categoryNames[2] = "Monoradar Service Messages";
    s_categoryNames[34] = "Monoradar Service Messages (Enhanced)";
    s_categoryNames[48] = "Monoradar Target Reports";
    s_categoryNames[62] = "SDPS Track Messages";
    s_categoryNames[63] = "Sensor Status Messages";
}

DecodedAsterixMessage AsterixDecoder::decode(const QByteArray &data)
{
    DecodedAsterixMessage result;
    result.isValid = false;

    // Minimum ASTERIX message: CAT (1) + LEN (2) + FSPEC (1) = 4 bytes
    if (data.size() < 4) {
        result.errorMessage = QString("Message too short: %1 bytes (minimum 4)").arg(data.size());
        return result;
    }

    // Parse Category (1 byte)
    result.category = static_cast<uint8_t>(data[0]);

    // Parse Length (2 bytes, big-endian)
    result.length = (static_cast<uint8_t>(data[1]) << 8) | static_cast<uint8_t>(data[2]);

    // Validate length
    if (result.length < 4) {
        result.errorMessage = QString("Invalid length field: %1 (minimum 4)").arg(result.length);
        return result;
    }

    if (result.length > data.size()) {
        result.errorMessage = QString("Length mismatch: message claims %1 bytes but only %2 received")
            .arg(result.length).arg(data.size());
        return result;
    }

    // Parse FSPEC (variable length, ends when bit 0 is not set)
    int fspecLength = 0;
    int offset = 3; // Start after CAT and LEN

    while (offset < data.size()) {
        fspecLength++;
        uint8_t fspecByte = static_cast<uint8_t>(data[offset]);
        offset++;

        // If bit 0 (FX bit) is not set, FSPEC ends
        if ((fspecByte & 0x01) == 0) {
            break;
        }

        // Safety check
        if (fspecLength > 16) {
            result.errorMessage = "FSPEC too long (> 16 bytes)";
            return result;
        }
    }

    result.fspec = data.mid(3, fspecLength);

    // Parse Data Items (everything after FSPEC)
    int dataItemsStart = 3 + fspecLength;
    int dataItemsLength = result.length - dataItemsStart;

    if (dataItemsLength < 0) {
        result.errorMessage = "Invalid data items length";
        return result;
    }

    result.dataItems = data.mid(dataItemsStart, dataItemsLength);
    result.isValid = true;

    return result;
}

QString AsterixDecoder::getCategoryName(uint8_t category)
{
    initCategoryNames();

    if (s_categoryNames.contains(category)) {
        return s_categoryNames[category];
    }
    return QString("Unknown Category %1").arg(category);
}

QString AsterixDecoder::formatFspec(const QByteArray &fspec)
{
    QString result;
    QTextStream stream(&result);

    stream << "FSPEC (" << fspec.size() << " bytes): ";

    for (int i = 0; i < fspec.size(); ++i) {
        if (i > 0) stream << " ";
        stream << QString("%1").arg(static_cast<uint8_t>(fspec[i]), 2, 16, QChar('0'));
    }

    stream << "\n  Binary: ";
    for (int i = 0; i < fspec.size(); ++i) {
        if (i > 0) stream << " ";
        uint8_t byte = static_cast<uint8_t>(fspec[i]);
        for (int bit = 7; bit >= 0; --bit) {
            stream << ((byte & (1 << bit)) ? '1' : '0');
        }
    }

    return result;
}

QString AsterixDecoder::formatDataItems(const QByteArray &data)
{
    if (data.isEmpty()) {
        return "  (no data items)";
    }

    QString result;
    QTextStream stream(&result);

    stream << "Data Items (" << data.size() << " bytes):\n";
    stream << "  Hex: " << HexDumper::toHexString(data);

    return result;
}

QString DecodedAsterixMessage::toString() const
{
    if (!isValid) {
        return QString("INVALID: %1").arg(errorMessage);
    }

    return QString("CAT %1, Length %2, FSPEC bytes: %3, Data bytes: %4")
        .arg(category)
        .arg(length)
        .arg(fspec.size())
        .arg(dataItems.size());
}

QString DecodedAsterixMessage::toDetailedString() const
{
    QString result;
    QTextStream stream(&result);

    if (!isValid) {
        stream << "╔════════════════════════════════════════════╗\n";
        stream << "║         INVALID ASTERIX MESSAGE            ║\n";
        stream << "╚════════════════════════════════════════════╝\n";
        stream << "Error: " << errorMessage << "\n";
        return result;
    }

    stream << "╔════════════════════════════════════════════╗\n";
    stream << "║         DECODED ASTERIX MESSAGE            ║\n";
    stream << "╚════════════════════════════════════════════╝\n";
    stream << "\n";
    stream << "Category:     " << category << " (0x"
           << QString("%1").arg(category, 2, 16, QChar('0')).toUpper() << ")\n";
    stream << "Description:  " << AsterixDecoder::getCategoryName(category) << "\n";
    stream << "Length:       " << length << " bytes\n";
    stream << "\n";
    stream << AsterixDecoder::formatFspec(fspec) << "\n";
    stream << "\n";
    stream << AsterixDecoder::formatDataItems(dataItems) << "\n";

    return result;
}
