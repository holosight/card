#include "pcsclog.h"

#include <iostream>

#include <QString>

#undef UNICODE
#include <winscard.h>

struct SCardErrorEntry {
    HRESULT code;
    const char * name;
};

static const SCardErrorEntry SCardErrorsNames[] =
{{ ERROR_BROKEN_PIPE,       "(ERROR_BROKEN_PIPE) The client attempted a smart card operation in a remote session, such as a client session running on a terminal server, and the operating system in use does not support smart card redirection." },
 { SCARD_E_BAD_SEEK,        "(SCARD_E_BAD_SEEK) An error occurred in setting the smart card file object pointer." },
 { SCARD_E_CANCELLED,       "(SCARD_E_CANCELLED) The action was canceled by an SCardCancel request." },
 { SCARD_E_CANT_DISPOSE,    "(SCARD_E_CANT_DISPOSE) The system could not dispose of the media in the requested manner." },
 { SCARD_E_CARD_UNSUPPORTED, "(SCARD_E_CARD_UNSUPPORTED) The smart card does not meet minimal requirements for support." },
 { SCARD_E_CERTIFICATE_UNAVAILABLE, "(SCARD_E_CERTIFICATE_UNAVAILABLE) The requested certificate could not be obtained." },
 { SCARD_E_COMM_DATA_LOST,  "(SCARD_E_COMM_DATA_LOST) A communications error with the smart card has been detected." },
 { SCARD_E_DIR_NOT_FOUND,   "(SCARD_E_DIR_NOT_FOUND) The specified directory does not exist in the smart card." },
 { SCARD_E_DUPLICATE_READER, "(SCARD_E_DUPLICATE_READER) The reader driver did not produce a unique reader name." },
 { SCARD_E_FILE_NOT_FOUND,  "(SCARD_E_FILE_NOT_FOUND) The specified file does not exist in the smart card." },
 { SCARD_E_ICC_CREATEORDER, "(SCARD_E_ICC_CREATEORDER) The requested order of object creation is not supported." },
 { SCARD_E_ICC_INSTALLATION, "(SCARD_E_ICC_INSTALLATION) No primary provider can be found for the smart card." },
 { SCARD_E_INSUFFICIENT_BUFFER, "(SCARD_E_INSUFFICIENT_BUFFER) The data buffer for returned data is too small for the returned data." },
 { SCARD_E_INVALID_ATR,     "(SCARD_E_INVALID_ATR) An ATR string obtained from the registry is not a valid ATR string." },
 { SCARD_E_INVALID_CHV,     "(SCARD_E_INVALID_CHV) The supplied PIN is incorrect." },
 { SCARD_E_INVALID_HANDLE,  "(SCARD_E_INVALID_HANDLE) The supplied handle was not valid." },
 { SCARD_E_INVALID_PARAMETER, "(SCARD_E_INVALID_PARAMETER) One or more of the supplied parameters could not be properly interpreted." },
 { SCARD_E_INVALID_TARGET,  "(SCARD_E_INVALID_TARGET) Registry startup information is missing or not valid." },
 { SCARD_E_INVALID_VALUE,   "(SCARD_E_INVALID_VALUE) One or more of the supplied parameter values could not be properly interpreted." },
 { SCARD_E_NO_ACCESS,       "(SCARD_E_NO_ACCESS) Access is denied to the file." },
 { SCARD_E_NO_DIR,          "(SCARD_E_NO_DIR) The supplied path does not represent a smart card directory." },
 { SCARD_E_NO_FILE,         "(SCARD_E_NO_FILE) The supplied path does not represent a smart card file." },
 { SCARD_E_NO_KEY_CONTAINER, "(SCARD_E_NO_KEY_CONTAINER) The requested key container does not exist on the smart card." },
 { SCARD_E_NO_MEMORY,       "(SCARD_E_NO_MEMORY) Not enough memory available to complete this command." },
 { SCARD_E_NO_READERS_AVAILABLE, "(SCARD_E_NO_READERS_AVAILABLE) No smart card reader is available." },
 { SCARD_E_NO_SERVICE,      "(SCARD_E_NO_SERVICE) The smart card resource manager is not running." },
 { SCARD_E_NO_SMARTCARD,    "(SCARD_E_NO_SMARTCARD) The operation requires a smart card, but no smart card is currently in the device." },
 { SCARD_E_NO_SUCH_CERTIFICATE, "(SCARD_E_NO_SUCH_CERTIFICATE) The requested certificate does not exist." },
 { SCARD_E_NOT_READY,       "(SCARD_E_NOT_READY) The reader or card is not ready to accept commands." },
 { SCARD_E_NOT_TRANSACTED,  "(SCARD_E_NOT_TRANSACTED) An attempt was made to end a nonexistent transaction." },
 { SCARD_E_PCI_TOO_SMALL,   "(SCARD_E_PCI_TOO_SMALL) The PCI receive buffer was too small." },
 { SCARD_E_PROTO_MISMATCH,  "(SCARD_E_PROTO_MISMATCH) The requested protocols are incompatible with the protocol currently in use with the card." },
 { SCARD_E_READER_UNAVAILABLE, "(SCARD_E_READER_UNAVAILABLE) The specified reader is not currently available for use." },
 { SCARD_E_READER_UNSUPPORTED, "(SCARD_E_READER_UNSUPPORTED) The reader driver does not meet minimal requirements for support." },
 { SCARD_E_SERVER_TOO_BUSY, "(SCARD_E_SERVER_TOO_BUSY) The smart card resource manager is too busy to complete this operation." },
 { SCARD_E_SERVICE_STOPPED, "(SCARD_E_SERVICE_STOPPED) The smart card resource manager has shut down." },
 { SCARD_E_SHARING_VIOLATION, "(SCARD_E_SHARING_VIOLATION) The smart card cannot be accessed because of other outstanding connections." },
 { SCARD_E_SYSTEM_CANCELLED, "(SCARD_E_SYSTEM_CANCELLED) The action was canceled by the system, presumably to log off or shut down." },
 { SCARD_E_TIMEOUT,         "(SCARD_E_TIMEOUT) The user-specified time-out value has expired." },
 { SCARD_E_UNEXPECTED,      "(SCARD_E_UNEXPECTED) An unexpected card error has occurred." },
 { SCARD_E_UNKNOWN_CARD,    "(SCARD_E_UNKNOWN_CARD) The specified smart card name is not recognized." },
 { SCARD_E_UNKNOWN_READER,  "(SCARD_E_UNKNOWN_READER) The specified reader name is not recognized." },
 { SCARD_E_UNKNOWN_RES_MNG, "(SCARD_E_UNKNOWN_RES_MNG) An unrecognized error code was returned." },
 { SCARD_E_UNSUPPORTED_FEATURE, "(SCARD_E_UNSUPPORTED_FEATURE) This smart card does not support the requested feature." },
 { SCARD_E_WRITE_TOO_MANY,  "(SCARD_E_WRITE_TOO_MANY) An attempt was made to write more data than would fit in the target object." },
 { SCARD_F_COMM_ERROR,      "(SCARD_F_COMM_ERROR) An internal communications error has been detected." },
 { SCARD_F_INTERNAL_ERROR,  "(SCARD_F_INTERNAL_ERROR) An internal consistency check failed." },
 { SCARD_F_UNKNOWN_ERROR,   "(SCARD_F_UNKNOWN_ERROR) An internal error has been detected, but the source is unknown." },
 { SCARD_F_WAITED_TOO_LONG, "(SCARD_F_WAITED_TOO_LONG) An internal consistency timer has expired." },
 { SCARD_P_SHUTDOWN,        "(SCARD_P_SHUTDOWN) The operation has been aborted to allow the server application to exit." },
 { SCARD_S_SUCCESS,         "(SCARD_S_SUCCESS) No error was encountered." },
 { SCARD_W_CANCELLED_BY_USER, "(SCARD_W_CANCELLED_BY_USER) The action was canceled by the user." },
 { SCARD_W_CACHE_ITEM_NOT_FOUND, "(SCARD_W_CACHE_ITEM_NOT_FOUND) The requested item could not be found in the cache." },
 { SCARD_W_CACHE_ITEM_STALE, "(SCARD_W_CACHE_ITEM_STALE) The requested cache item is too old and was deleted from the cache." },
 { SCARD_W_CARD_NOT_AUTHENTICATED, "(SCARD_W_CARD_NOT_AUTHENTICATED) No PIN was presented to the smart card." },
 { SCARD_W_CHV_BLOCKED,     "(SCARD_W_CHV_BLOCKED) The card cannot be accessed because the maximum number of PIN entry attempts has been reached." },
 { SCARD_W_EOF,             "(SCARD_W_EOF) The end of the smart card file has been reached." },
 { SCARD_W_REMOVED_CARD,    "(SCARD_W_REMOVED_CARD) The smart card has been removed, so further communication is not possible." },
 { SCARD_W_RESET_CARD,      "(SCARD_W_RESET_CARD) The smart card was reset." },
 { SCARD_W_SECURITY_VIOLATION, "(SCARD_W_SECURITY_VIOLATION) Access was denied because of a security violation." },
 { SCARD_W_UNPOWERED_CARD,  "(SCARD_W_UNPOWERED_CARD) Power has been removed from the smart card, so that further communication is not possible." },
 { SCARD_W_UNRESPONSIVE_CARD, "(SCARD_W_UNRESPONSIVE_CARD) The smart card is not responding to a reset." },
 { SCARD_W_UNSUPPORTED_CARD, "(SCARD_W_UNSUPPORTED_CARD) The reader cannot communicate with the card, due to ATR string configuration conflicts." },
 { SCARD_W_WRONG_CHV,       "(SCARD_W_WRONG_CHV) The card cannot be accessed because the wrong PIN was presented." }};

static const char * getSCardErrorName (LONG error)
{
    int errors = sizeof(SCardErrorsNames) / sizeof(SCardErrorEntry);

    for (int i = 0; i < errors; i++) {
        if (error == SCardErrorsNames[i].code)
            return SCardErrorsNames[i].name;
    }

    return nullptr;
}

static QString getErrorAsString(LONG error)
{
    if (error == ERROR_SUCCESS)
        return QString("SUCCESS");

    const char * scardError = getSCardErrorName(error);
    if (scardError != nullptr)
        return QString(scardError);

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, error, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPSTR)&messageBuffer, 0, NULL);

    QString message = QString::fromLocal8Bit(messageBuffer, size);
    LocalFree(messageBuffer);

    return message;
}

static const char * getStatusWordAsStr(uint8_t sw1, uint8_t sw2)
{
    uint16_t sw = (((uint16_t) sw1) << 8) + sw2;

    if (sw == 0x9000)
        return "SUCCESS";
    if (sw1 == 0x61)
        return "SUCCESS. More Bytes Available";
    if (sw == 0x6200)
        return "State of non-volatile memory unchanged. No information given.";
    if (sw == 0x6281)
        return "State of non-volatile memory unchanged. Part of returned data my be corrupted.";
    if (sw == 0x6282)
        return "State of non-volatile memory unchanged. End of file/record reached before reading Le bytes.";
    if (sw == 0x6283)
        return "State of non-volatile memory unchanged. Selected file invalidated.";
    if (sw1 == 0x62)
        return "State of non-volatile memory unchanged.";
    if (sw == 0x6300)
        return "State of non-volatile memory changed. No information given. (MIFARE ERROR)";
    if (sw == 0x6381)
        return "State of non-volatile memory changed. File filled up by the last write.";
    if (sw1 == 0x63)
        return "State of non-volatile memory changed.";
    if (sw == 0x6400)
        return "State of non-volatile memory unchanged.";
    if (sw == 0x6581)
        return "State of non-volatile memory changed. Memory failure.";
    if (sw1 == 0x65)
        return "State of non-volatile memory changed.";
    if (sw1 == 0x66)
        return "Reserved for security-related issues.";
    if (sw == 0x6700)
        return "Wrong length.";
    if (sw == 0x6881)
        return "Functions in CLA not supported. Logical channel not supported.";
    if (sw == 0x6882)
        return "Functions in CLA not supported. Secure messaging not supported.";
    if (sw1 == 0x68)
        return "Functions in CLA not supported.";
    if (sw == 0x6981)
        return "Command not allowed. Command incompatible with file structure.";
    if (sw == 0x6982)
        return "Command not allowed. Security status not satisfied.";
    if (sw == 0x6983)
        return "Command not allowed. Authentication method blocked.";
    if (sw == 0x6984)
        return "Command not allowed. Referenced data invalidated.";
    if (sw == 0x6985)
        return "Command not allowed. Conditions of use not satisfied.";
    if (sw == 0x6986)
        return "Command not allowed. Command not allowed (no current EF).";
    if (sw == 0x6987)
        return "Command not allowed. Expected SM data objects missing.";
    if (sw == 0x6988)
        return "Command not allowed. SM data objects incorrect.";
    if (sw1 == 0x69)
        return "Command not allowed.";
    if (sw == 0x6A80)
        return "Wrong parameter(s) P1-P2. Incorrect parameters in the data field.";
    if (sw == 0x6A81)
        return "Wrong parameter(s) P1-P2. Function not supported.";
    if (sw == 0x6A82)
        return "Wrong parameter(s) P1-P2. File not found.";
    if (sw == 0x6A83)
        return "Wrong parameter(s) P1-P2. Record not found.";
    if (sw == 0x6A84)
        return "Wrong parameter(s) P1-P2. Not enough memory space in the file.";
    if (sw == 0x6A85)
        return "Wrong parameter(s) P1-P2. Lc inconsistent with TLV structure.";
    if (sw == 0x6A86)
        return "Wrong parameter(s) P1-P2. Incorrect parameters P1-P2.";
    if (sw == 0x6A87)
        return "Wrong parameter(s) P1-P2. Lc inconsistent with P1-P2.";
    if (sw == 0x6A88)
        return "Wrong parameter(s) P1-P2. Referenced data not found.";
    if (sw1 == 0x6A)
        return "Wrong parameter(s) P1-P2.";
    if (sw == 0x6B00)
        return "Wrong parameter(s) P1-P2.";
    if (sw1 == 0x6C)
        return "Wrong length Le. More Bytes Available";
    if (sw == 0x6D00)
        return "Instruction code not supported or invalid.";
    if (sw == 0x6E00)
        return "Class not supported.";
    if (sw == 0x6F00)
        return "No precise diagnosis.";

    return "Error not defined by ISO 7816";

}

PCSCLog::PCSCLog()
{

}

void PCSCLog::log(const char * fun, long result, const QString & comment)
{
    if (comment.isEmpty())
        printf("%s : %ld (0x%lX) (%s)\n", fun, result, result, getErrorAsString(result).toStdString().c_str());
    else
        printf("%s : %ld (0x%lX) (%s) : [%s]\n", fun, result, result, getErrorAsString(result).toStdString().c_str(),
               comment.toStdString().c_str());
}

void PCSCLog::log(const QString &message)
{
    printf("%s\n", message.toStdString().c_str());
}

void PCSCLog::logsw(const char *fun, long result, uint8_t sw1, uint8_t sw2, const QString & comment)
{
    printf("%s : %ld (0x%lX) (%s) SW=[%02X, %02X] (%s) : [%s]\n", fun, result, result, getErrorAsString(result).toStdString().c_str(),
           sw1, sw2, getStatusWordAsStr(sw1, sw2), comment.toStdString().c_str());
}

void PCSCLog::logTransmit(long result, const QByteArray &apdu, const QByteArray &response)
{
    if (response.length() < 2) {
        printf("SCardTransmit : %ld (0x%lX) (%s) ERROR RESPONSE < 2\n", result, result, getErrorAsString(result).toStdString().c_str());
    }

    uint8_t sw1 = response.right(2)[0];
    uint8_t sw2 = response.right(2)[1];
    QByteArray resp(response);
    resp.chop(2);

    printf("SCardTransmit : %ld (0x%lX) (%s)\n", result, result, getErrorAsString(result).toStdString().c_str());
    printf("    apdu = [ ");
    for (uint8_t c : apdu)
        printf("%02X ", c);
    printf("] ");
    printf("(length = %d)\n", apdu.length());
    printf("    resp = [ ");
    for (uint8_t c : resp)
        printf("%02X ", c);
    printf("] ");
    printf("(length = %d)\n", resp.length());
    printf("    SW=[%02X%02X] (%s)\n", sw1, sw2, getStatusWordAsStr(sw1, sw2));
}

void PCSCLog::logControl(long result, const QByteArray &apdu, const QByteArray &response)
{
    QByteArray resp(response);

    printf("SCardControl : %ld (0x%lX) (%s)\n", result, result, getErrorAsString(result).toStdString().c_str());
    printf("    apdu = [ ");
    for (uint8_t c : apdu)
        printf("%02X ", c);
    printf("] ");
    printf("(length = %d)\n", apdu.length());
    printf("    resp = [ ");
    for (uint8_t c : resp)
        printf("%02X ", c);
    printf("] ");
    printf("(length = %d)\n", resp.length());
}
