#include <QCoreApplication>
#include <QDebug>
#include "scardcontext.h"

void runCard()
{
    SCardContext context;

    auto readers = context.listReaders();

    for (auto reader : readers) {
        qDebug() << reader;
    }

    if (readers.length() == 0) {
        qDebug() << "no reader";
        return;
    }

    if (!context.waitForSCInReader(readers[1], 10)) {
        auto card = context.connect(SCARD_SHARE_DIRECT, SCARD_PROTOCOL_UNDEFINED, readers[1]);

        if (!card) {
            qDebug() << "no card";
            return;
        }

        card->beep(1);
        Sleep(100);
        card->beep(2);
        Sleep(100);
        card->beep(1);

        bool ledState = false;
        for (int i = 0; i < 10; i++) {
            card->controlLed(ledState, !ledState);
            Sleep(100);
            ledState = !ledState;
        }
        card->controlLed(false, false);

        return;
    }

    if (readers.length() >= 2) {
        auto card = context.connect(SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, readers[1]);

        if (!card) {
            qDebug() << "no card";
            return;
        }

        if (!card->beginTransaction())
            return;

        auto uuid = card->getData();
        qDebug() << "data : " << uuid.toHex().toUpper();

        card->endTransaction(SCARD_LEAVE_CARD);

        uint8_t keyNumber = 0;
        uint8_t block = 1;
        card->loadKey(SCard::NonVolatileMemory, keyNumber, SCard::defaultKey());
        if (!card->generalAuthenticate(SCard::KeyA, block, keyNumber)) {
            qDebug() << "auth error";
        }

//        QString blockWrite = "Hello from scard";
//        blockWrite = blockWrite.leftJustified(MIFARE_BLOCK_SIZE, '.', true);
//        card->updareBinary(block, blockWrite.toLatin1());

        QByteArray blockData = card->readBinary(block);
        QString blockAscii = QString::fromLatin1(blockData, blockData.length());
        qDebug() << "read block [" << block << "]";
        qDebug() << "    hex : " << blockData.toHex().toUpper();
        qDebug() << "    ascii : " << blockAscii;

    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    runCard();

    return a.exec();
}
