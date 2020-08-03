#include "ErrorHandler.h"

#include <QDebug>

ErrorHandler* ErrorHandler::s_pInstance = NULL;


ErrorHandler *ErrorHandler::instance()
{
    // Do we have an oracle yet?
    if(s_pInstance == NULL) {
      // No, create it
      s_pInstance = new ErrorHandler();
    }

    // Return the instance ptr
    return s_pInstance;
}

void ErrorHandler::destroy()
{
    // Clean up the pointer
    if(s_pInstance != NULL) {
        delete s_pInstance, s_pInstance = NULL;
    }
}

void ErrorHandler::setError(const QString &a_rstrErrorMsg)
{
    m_strErrorMsg = a_rstrErrorMsg;

    qDebug() << a_rstrErrorMsg;
}

void ErrorHandler::clearErrorMsg()
{
    m_strErrorMsg="";
}

bool ErrorHandler::hasError() const
{
    return m_strErrorMsg != "";
}

const QString &ErrorHandler::displayErrorMsg() const
{
    return m_strErrorMsg;
}

ErrorHandler::ErrorHandler()
{
    m_strErrorMsg="";
}

ErrorHandler::~ErrorHandler()
{

}
