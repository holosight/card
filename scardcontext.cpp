#include "scardcontext.h"
#include "pcsclog.h"

static QString getSCardChangeState(DWORD state)
{
    QStringList s;
    if (state & SCARD_STATE_UNAWARE)
        s += "UNAWARE";
    if (state & SCARD_STATE_UNAVAILABLE)
        s += "UNAVAILABLE";
    if (state & SCARD_STATE_IGNORE)
        s += "IGNORE";
    if (state & SCARD_STATE_CHANGED)
        s += "CHANGED";
    if (state & SCARD_STATE_UNKNOWN)
        s += "UNKNOWN";
    if (state & SCARD_STATE_EMPTY)
        s += "EMPTY";
    if (state & SCARD_STATE_PRESENT)
        s += "PRESENT";
    if (state & SCARD_STATE_ATRMATCH)
        s += "ATRMATCH";
    if (state & SCARD_STATE_EXCLUSIVE)
        s += "EXCLUSIVE";
    if (state & SCARD_STATE_INUSE)
        s += "INUSE";
    if (state & SCARD_STATE_MUTE)
        s += "MUTE";

    return s.join(" | ");
}

SCardContext::SCardContext()
{
    m_context = 0;
    LONG ret = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &m_context);
    PCSCLog::log("SCardEstablishContext", ret);
    m_isValid = (ret == SCARD_S_SUCCESS);
}

SCardContext::~SCardContext()
{
    if (!isValid())
        return;

    LONG ret = SCardReleaseContext(m_context);
    PCSCLog::log("SCardReleaseContext", ret);
}

QStringList SCardContext::listReaders(LPCTSTR groups)
{
    if (!isValid())
        return QStringList();

    LPTSTR mszReaders;
    DWORD dwReaders = SCARD_AUTOALLOCATE;
    LONG ret = SCardListReaders(m_context, groups, (LPTSTR)&mszReaders, &dwReaders);
    PCSCLog::log("SCardListReaders", ret);
    m_isValid = (ret == SCARD_S_SUCCESS);

    if (!m_isValid)
        return QStringList();

    QStringList readers = QString::fromLocal8Bit(mszReaders, dwReaders).split(QChar(0));
    readers.removeAll("");
    return readers;
}

SSCard SCardContext::connect(DWORD shareMode, DWORD preferredProt, const QString &readerName)
{
    if (!isValid())
        return SSCard(nullptr);

    SCARDHANDLE hCard;
    DWORD activeProtocol;
    LONG ret = SCardConnect(m_context, readerName.toStdString().c_str(), shareMode, preferredProt, &hCard, &activeProtocol);
    PCSCLog::log("SCardConnect", ret, QString("reader=%0").arg(readerName));
    m_isValid = (ret == SCARD_S_SUCCESS);

    if (!isValid())
        return SSCard(nullptr);

    return SSCard(new SCard(hCard, activeProtocol));
}

SCARDCONTEXT SCardContext::contextHandle() const
{
    return m_context;
}

bool SCardContext::waitForSCInReader(const QString &reader, uint32_t timeout)
{
    SCARD_READERSTATE state;
    std::string readerName = reader.toStdString();
    state.szReader = readerName.c_str();
    state.pvUserData = nullptr;
    state.dwCurrentState = SCARD_STATE_UNAWARE;
    state.dwEventState = SCARD_STATE_UNAWARE;
    state.cbAtr = 0;

    LONG ret = SCardGetStatusChange(m_context, 0, &state, 1);
    PCSCLog::log("SCardGetStatusChange", ret);

    PCSCLog::log(QString("get init reader status = [%0]").arg(getSCardChangeState(state.dwEventState)));

    if (state.dwEventState & SCARD_STATE_PRESENT)
        return true;

    state.dwCurrentState = state.dwEventState;
    state.szReader = readerName.c_str();
    ret = SCardGetStatusChange(m_context, timeout, &state, 1);

    if (ret == SCARD_S_SUCCESS) {
        PCSCLog::log(QString("reader status = [%0]").arg(getSCardChangeState(state.dwEventState)));
        if (state.dwEventState & SCARD_STATE_PRESENT)
            return true;
        else
            return false;
    } else {
        PCSCLog::log("SCardGetStatusChange", ret);
        return false;
    }
}

bool SCardContext::isValid() const
{
    return m_isValid;
}
