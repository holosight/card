#ifndef SCARDCONTEXT_H
#define SCARDCONTEXT_H

#undef UNICODE
#include <winscard.h>

#include <QString>
#include <QStringList>
#include <QSharedPointer>

#include "scard.h"

#define SCARD_READER_NOTIFY "\\\\?PnP?\\Notification"

class SCardContext
{
public:
    SCardContext();
    ~SCardContext();
    QStringList listReaders(LPCTSTR groups = SCARD_ALL_READERS);
    SSCard connect(DWORD shareMode, DWORD preferredProt, const QString & readerName);
    SCARDCONTEXT contextHandle() const;
    bool waitForSCInReader(const QString & reader, uint32_t timeout);

    bool isValid() const;

private:
    SCARDCONTEXT m_context;
    bool m_isValid;
};

#endif // SCARDCONTEXT_H
