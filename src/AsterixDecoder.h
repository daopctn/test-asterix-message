#ifndef ASTERIX_DECODER_H
#define ASTERIX_DECODER_H

#include <QByteArray>
#include <QString>
#include <QMap>
#include <cstdint>

struct DecodedAsterixMessage
{
    uint8_t category;
    uint16_t length;
    QByteArray fspec;
    QByteArray dataItems;
    bool isValid;
    QString errorMessage;

    QString toString() const;
    QString toDetailedString() const;
};

class AsterixDecoder
{
public:
    static DecodedAsterixMessage decode(const QByteArray &data);

    static QString getCategoryName(uint8_t category);
    static QString formatFspec(const QByteArray &fspec);
    static QString formatDataItems(const QByteArray &data);

private:
    static QMap<uint8_t, QString> s_categoryNames;
    static void initCategoryNames();
};

#endif // ASTERIX_DECODER_H
