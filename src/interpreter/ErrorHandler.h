#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <QString>


class ErrorHandler
{
public:
    static ErrorHandler* instance();

    static void destroy();

    void setError(const QString& a_rstrErrorMsg);

    void clearErrorMsg();

    bool hasError() const;

    const QString& displayErrorMsg() const;

private:
    ErrorHandler();
    ~ErrorHandler();

private:
    static ErrorHandler  *s_pInstance;

    QString m_strErrorMsg;
};

#endif // ERRORHANDLER_H
