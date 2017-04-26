#ifndef PCSCLOG_H
#define PCSCLOG_H

#include <QByteArray>
#include <QString>

#define PCSCFunc(fun, r) PCSCLog::log(#fun, r)
#define PCSCFuncSW(fun, r, sw1, sw2) PCSCLog::logsw(#fun, r, sw1, sw2)

class PCSCLog
{
public:
    PCSCLog();

    static void log (const char * fun, long result, const QString & comment = QString(""));
    static void log (const QString & message);
    static void logsw (const char * fun, long result, uint8_t sw1, uint8_t sw2, const QString & comment = QString(""));
    static void logTransmit (long result, const QByteArray & apdu, const QByteArray & response);
    static void logControl (long result, const QByteArray & apdu, const QByteArray & response);
};

#endif // PCSCLOG_H
