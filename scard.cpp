#include "scard.h"
#include "pcsclog.h"

SCard::SCard(SCARDHANDLE h, DWORD activeProtocol)
{
    m_connected = true;
    m_handle = h;
    m_activeProtocol = activeProtocol;
    m_isValid = true;
}

SCard::~SCard()
{
    disconnect(SCARD_LEAVE_CARD);
}

void SCard::disconnect(DWORD reason)
{
    if (!isValid())
        return;

    m_connected = false;
    LONG ret = SCardDisconnect(m_handle, reason);
    PCSCLog::log("SCardDisconnect", ret, QString("reason = %0").arg(reason));
    m_isValid = (ret == SCARD_S_SUCCESS);
}

QByteArray SCard::getData()
{
    if (!isValid())
        return QByteArray();

    QByteArray apdu(5, 0);
    apdu[0] = SCARD_READER_CUSTOM_CLA; // cla
    apdu[1] = SCARD_MIFARE_GET_DATA; // ins
    apdu[2] = 0x00; // p1
    apdu[3] = 0x00; // p2
    apdu[4] = 0x00;

    PCSCLog::log("SCard::getData");
    return transmit(apdu);
}

QByteArray SCard::readBinary(uint8_t block)
{
    if (!isValid())
        return QByteArray();

    QByteArray apdu(5, 0);
    apdu[0] = SCARD_READER_CUSTOM_CLA;
    apdu[1] = SCARD_MIFARE_READ_BINARY;
    apdu[2] = 0x00;
    apdu[3] = block;
    apdu[4] = MIFARE_BLOCK_SIZE;

    PCSCLog::log("SCard::readBinary");
    return transmit(apdu);
}

void SCard::updareBinary(uint8_t block, const QByteArray &data)
{
    if (!isValid())
        return;

    if (data.length() != MIFARE_BLOCK_SIZE) {
        PCSCLog::log(QString("SCard::updareBinary data length != %1 (%0)").arg(data.length()).arg(MIFARE_BLOCK_SIZE));
        return;
    }

    QByteArray apdu(21, 0);
    apdu[0] = SCARD_READER_CUSTOM_CLA;
    apdu[1] = SCARD_MIFARE_UPDATE_BINARY;
    apdu[2] = 0x00; // MSB
    apdu[3] = block;
    apdu[4] = 0x10;
    apdu.replace(5, data.length(), data);

    PCSCLog::log("SCard::updareBinary");
    transmit(apdu);
}

bool SCard::generalAuthenticate(SCard::MifareKeyType keyType, uint8_t block, uint8_t keyNumber)
{
    if (!isValid())
        return false;

    QByteArray apdu(10, 0);
    apdu[0] = SCARD_READER_CUSTOM_CLA;
    apdu[1] = SCARD_MIFARE_GENERAL_AUTH;
    apdu[2] = 0x00;
    apdu[3] = 0x00;
    apdu[4] = 5;
    apdu[5] = 0x01; // version
    apdu[6] = 0x00; // MSB
    apdu[7] = block; // LSB
    apdu[8] = (uint8_t) keyType;
    apdu[9] = keyNumber;

    PCSCLog::log("SCard::generalAuthenticate");
    return checkSuccessOperation(transmit(apdu));
}

void SCard::loadKey(SCard::ReaderMemory memoryType, uint8_t keyNumber, const QByteArray &data)
{
    if (!isValid())
        return;

    if (data.length() != 6) {
        PCSCLog::log(QString("LoadKey : data.length != 6 (%0)").arg(data.length()));
        m_isValid = false;
        return;
    }

    QByteArray apdu(11, 0);
    apdu[0] = SCARD_READER_CUSTOM_CLA; // cla
    apdu[1] = SCARD_MIFARE_LOAD_KEY; // ins
    apdu[2] = (uint8_t) memoryType; // p1
    apdu[3] = keyNumber;
    apdu[4] = data.length(); // equals 6
    apdu.replace(5, data.length(), data);

    PCSCLog::log("SCard::loadKey");
    transmit(apdu);
}

bool SCard::beginTransaction()
{
    LONG ret = SCardBeginTransaction(m_handle);
    PCSCLog::log("SCardBeginTransaction", ret);
    return (ret == SCARD_S_SUCCESS);
}

void SCard::endTransaction(DWORD reason)
{
    LONG ret = SCardEndTransaction(m_handle, reason);
    PCSCLog::log("SCardEndTransaction", ret);
}

bool SCard::isValid()
{
    return m_isValid && isValidContext();
}

bool SCard::isValidContext()
{
//    LONG ret = SCardIsValidContext(m_handle);
//    if (ret != SCARD_S_SUCCESS)
//        PCSCLog::log("SCard::isValidContext NOT VALID");

//    return (ret == SCARD_S_SUCCESS);
    return true;
}

void SCard::playBeeps()
{
    struct NoteS {
        int note;
        int duration;
    } melody[] = {
        1, 8,
        0, 2,
        1, 8,
        0, 2,
        1, 8,
        2, 8,
        1, 2,
        0, 2,
        1, 4,
        2, 8,
        1, 2,
        0, 2,
        1, 8
    };

    int notesLen = sizeof(melody) / sizeof(NoteS);

    for (int i = 0; i < notesLen; i++) {
        playNote(melody[i].note, melody[i].duration);
    }
}

void SCard::playNote(int note, int duration)
{
    switch (note) {
    case 0:
        Sleep(duration * 10);
        return;
    case 1:
        for (int i = 0; i < duration; i++) {
            beep(3);
            Sleep(10);
        }
        return;
    case 2:
        for (int i = 0; i < duration; i++) {
            beep(1);
            Sleep(50);
        }
        return;
    default:
        break;
    }
}

void SCard::beep(uint8_t duration)
{
    if (!isValid())
        return;

    QByteArray apdu(3, 0);
    apdu[0] = 0x28;
    apdu[1] = 0x01;
    apdu[2] = duration;

    PCSCLog::log("SCard::beep");
    control(apdu);
}

void SCard::controlLed(LedType ledType, LedState state)
{
    if (!isValid())
        return;

    uint8_t ledst = ((uint8_t) state) << (ledType-1);
    QByteArray apdu(3, 0);
    apdu[0] = 0x29;
    apdu[1] = 0x01;
    apdu[2] = ((uint8_t) ledType) & ledst;

    PCSCLog::log(QString("SCard::controlLed : %0 : %1").arg(ledType == LedRed ? "RED" : "green").arg(state ? "ON" : "OFF"));
    control(apdu);
}

void SCard::controlLed(bool red, bool green)
{
    if (!isValid())
        return;

    QByteArray apdu(3, 0);
    apdu[0] = 0x29;
    apdu[1] = 0x01;
    apdu[2] = (((uint8_t) green) << 1) + ((uint8_t) red);

    PCSCLog::log(QString("SCard::controlLed : RED : %0 GREEN : %1").arg(red ? "ON" : "OFF").arg(green ? "ON" : "OFF"));
    control(apdu);
}

QByteArray SCard::control(const QByteArray &apdu)
{
    QByteArray response(SCARD_MAX_APDU_SIZE, 0);
    DWORD cbRecv = response.length();
    LONG ret = SCardControl(m_handle, CONTROL_CODE_ACR128, (LPCVOID)apdu.constData(), (DWORD)apdu.length(),
                            (LPVOID)response.data(), response.length(), &cbRecv);
    response.resize(cbRecv);
    PCSCLog::logControl(ret, apdu, response);
    m_isValid = (ret == SCARD_S_SUCCESS);
    return response;
}

QByteArray SCard::transmit(const QByteArray &apdu)
{
    QByteArray response(SCARD_MAX_APDU_SIZE, 0);
    DWORD cbRecv = response.length();
    LONG ret = SCardTransmit(m_handle, SCARD_PCI_T1, (LPCBYTE)apdu.constData(), apdu.length(), NULL, (LPBYTE)response.data(), &cbRecv);
    response.resize(cbRecv);
    PCSCLog::logTransmit(ret, apdu, response);
    m_isValid = (ret == SCARD_S_SUCCESS);
    return response;
}

QByteArray SCard::defaultKey()
{
    QByteArray keys(6, 0xFF);
    return keys;
}

bool SCard::checkSuccessOperation(const QByteArray &response) const
{
    if (response.length() < 2)
        return false;

    QByteArray swbyt = response.right(2);
    uint8_t sw1 = swbyt[0];
    uint8_t sw2 = swbyt[1];

    if (sw1 == 0x90 && sw2 == 0x00)
        return true;
    if (sw1 == 0x61)
        return true;

    return false;
}
