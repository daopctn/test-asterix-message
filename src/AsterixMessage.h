#ifndef ASTERIX_MESSAGE_H
#define ASTERIX_MESSAGE_H

#include <QByteArray>
#include <QString>
#include <cstdint>

class AsterixMessage
{
public:
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

    uint16_t calculateLength() const;
};

#endif // ASTERIX_MESSAGE_H
