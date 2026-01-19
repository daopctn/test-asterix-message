#include "AsterixMessage.h"
#include <QtEndian>

AsterixMessage::AsterixMessage(uint8_t category)
    : m_category(category)
{
}

AsterixMessage AsterixMessage::createMinimalTestMessage()
{
    AsterixMessage msg(1);
    msg.setFspec(QByteArray::fromHex("80"));
    msg.setDataItems(QByteArray::fromHex("0102030405060708"));
    return msg;
}

AsterixMessage AsterixMessage::createSampleCat001()
{
    AsterixMessage msg(1);
    msg.setFspec(QByteArray::fromHex("80"));
    msg.setDataItems(QByteArray::fromHex("0A0B"));
    return msg;
}

AsterixMessage AsterixMessage::createSampleCat034()
{
    AsterixMessage msg(34);
    msg.setFspec(QByteArray::fromHex("C0"));
    msg.setDataItems(QByteArray::fromHex("01020304"));
    return msg;
}

AsterixMessage AsterixMessage::createSampleCat048()
{
    // ============================================
    // USER CUSTOMIZATION SECTION
    // Modify the hex values below to test different
    // ASTERIX message configurations
    // ============================================

    AsterixMessage msg(48);

    // FSPEC: I048/010 (Data Source Identifier) present
    msg.setFspec(QByteArray::fromHex("80"));

    // I048/010: SAC=0x01, SIC=0x02
    msg.setDataItems(QByteArray::fromHex("0102"));

    return msg;
}

void AsterixMessage::setCategory(uint8_t category)
{
    m_category = category;
}

void AsterixMessage::setFspec(const QByteArray &fspec)
{
    m_fspec = fspec;
}

void AsterixMessage::setDataItems(const QByteArray &data)
{
    m_dataItems = data;
}

void AsterixMessage::appendDataItem(const QByteArray &item)
{
    m_dataItems.append(item);
}

uint8_t AsterixMessage::category() const
{
    return m_category;
}

uint16_t AsterixMessage::length() const
{
    return calculateLength();
}

QByteArray AsterixMessage::fspec() const
{
    return m_fspec;
}

QByteArray AsterixMessage::dataItems() const
{
    return m_dataItems;
}

uint16_t AsterixMessage::calculateLength() const
{
    // 1 (CAT) + 2 (LEN) + FSPEC length + Data Items length
    return static_cast<uint16_t>(1 + 2 + m_fspec.size() + m_dataItems.size());
}

QByteArray AsterixMessage::toByteArray() const
{
    QByteArray result;

    // Category (1 byte)
    result.append(static_cast<char>(m_category));

    // Length (2 bytes, big-endian)
    uint16_t len = calculateLength();
    result.append(static_cast<char>((len >> 8) & 0xFF));
    result.append(static_cast<char>(len & 0xFF));

    // FSPEC
    result.append(m_fspec);

    // Data Items
    result.append(m_dataItems);

    return result;
}

bool AsterixMessage::isValid() const
{
    return m_category > 0 && !m_fspec.isEmpty();
}

QString AsterixMessage::validationError() const
{
    if (m_category == 0) {
        return "Category must be greater than 0";
    }
    if (m_fspec.isEmpty()) {
        return "FSPEC cannot be empty";
    }
    return QString();
}
