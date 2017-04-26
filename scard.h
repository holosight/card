#ifndef SCARD_H
#define SCARD_H

#undef UNICODE
#include <winscard.h>

#include <QSharedPointer>

#define MIFARE_BLOCK_SIZE 16
#define SCARD_MAX_APDU_SIZE 255

#define SCARD_MIFARE_GET_DATA       0xCA
#define SCARD_MIFARE_LOAD_KEY       0x82
#define SCARD_MIFARE_GENERAL_AUTH   0x86
#define SCARD_MIFARE_UPDATE_BINARY  0xD6
#define SCARD_MIFARE_READ_BINARY    0xB0
#define SCARD_READER_CUSTOM_CLA     0xFF
#define CONTROL_CODE_ACR128 SCARD_CTL_CODE(2079)

class SCard
{
    SCard & operator=(const SCard &) = delete;
    SCard(const SCard &) = delete;
public:
    SCard(SCARDHANDLE h, DWORD activeProtocol);
    ~SCard();

    enum MifareKeyType {
        KeyA = 0x60,
        KeyB = 0x61
    };

    enum ReaderMemory {
        VolatileMemory = 0x00,
        NonVolatileMemory = 0x20
    };

    void disconnect(DWORD reason);
    QByteArray getData();
    QByteArray readBinary(uint8_t block);
    void updareBinary(uint8_t block, const QByteArray & data);
    bool generalAuthenticate(MifareKeyType keyType, uint8_t block, uint8_t keyNumber);
    void loadKey(ReaderMemory memoryType, uint8_t keyNumber, const QByteArray & data);
    bool beginTransaction();
    void endTransaction(DWORD reason);

    bool isValid();
    bool isValidContext();
    void playBeeps();
    void playNote(int note, int duration);
    void beep(uint8_t duration);

    enum LedType {
        LedRed = 1,
        LedGreen = 2
    };

    enum LedState {
        LedOff = 0,
        LedOn = 1
    };

    void controlLed(LedType ledType, LedState state);
    void controlLed(bool red, bool green);

    QByteArray control(const QByteArray & apdu);
    QByteArray transmit(const QByteArray & apdu);

    static QByteArray defaultKey();

private:
    SCARDHANDLE m_handle;
    bool m_connected;
    DWORD m_activeProtocol;
    bool m_isValid;

    bool checkSuccessOperation(const QByteArray & response) const;
};

using SSCard = QSharedPointer<SCard>;

#endif // SCARD_H
