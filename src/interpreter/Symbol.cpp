#include "Symbol.h"

Symbol::Symbol(const QString &str)
{
    m_type = StringType;
    m_strData = str;
}

Symbol::Symbol(long double number)
{
    m_type = NumberType;
    m_doubleData = number;
}

Symbol::Symbol()
{
    m_type = invalid;
}

const Symbol::Type &Symbol::type() const
{
    return m_type;
}

long double Symbol::getNumber() const
{
    return m_doubleData;
}

void Symbol::setNumber(long double value)
{
    m_doubleData = value;
}

const QString &Symbol::getString() const
{
    return m_strData;
}

void Symbol::setString(const QString &str)
{
    m_strData = str;
}

Symbol Symbol::operator +(const Symbol &other) const
{
    if (m_type != other.m_type) {
        QString strErrorMsg = "Type Error: can not apply \"+\" on two different types";
        throw (strErrorMsg.toStdString());
    }
    else {
        switch (m_type)
        {
        case StringType: {
            Symbol strData(m_strData + other.m_strData);
            return strData;
        }
        case NumberType: {
            Symbol numData(m_doubleData + other.m_doubleData);
            return numData;
        }
        default:
            QString strErrorMsg = "Type Error: can not apply \"+\" on invalid types";
            throw (strErrorMsg.toStdString());
        }
    }
}

Symbol Symbol::operator -(const Symbol &other) const
{
    if (m_type != other.m_type) {
        QString strErrorMsg = "Type Error: can not apply \"-\" on two different types";
        throw (strErrorMsg.toStdString());
    }
    else {
        switch (m_type)
        {
        case StringType: {
            QString strErrorMsg = "Type Error: can not apply \"-\" on string type";
            throw (strErrorMsg.toStdString());
        }
        case NumberType: {
            Symbol numData(m_doubleData - other.m_doubleData);
            return numData;
        }
        default:
            QString strErrorMsg = "Type Error: can not apply \"-\" on invalid types";
            throw (strErrorMsg.toStdString());
        }
    }
}

Symbol Symbol::operator *(const Symbol &other) const
{
    if (m_type != other.m_type) {
        QString strErrorMsg = "Type Error: can not apply \"*\" on two different types";
        throw (strErrorMsg.toStdString());
    }
    else {
        switch (m_type)
        {
        case StringType: {
            QString strErrorMsg = "Type Error: can not apply \"*\" on string type";
            throw (strErrorMsg.toStdString());
        }
        case NumberType: {
            Symbol numData(m_doubleData * other.m_doubleData);
            return numData;
        }
        default:
            QString strErrorMsg = "Type Error: can not apply \"*\" on invalid types";
            throw (strErrorMsg.toStdString());
        }
    }
}

Symbol Symbol::operator /(const Symbol &other) const
{
    if (m_type != other.m_type) {
        QString strErrorMsg = "Type Error: can not apply \"/\" on two different types";
        throw (strErrorMsg.toStdString());
    }
    else {
        switch (m_type)
        {
        case StringType: {
            QString strErrorMsg = "Type Error: can not apply \"/\" on string type";
            throw (strErrorMsg.toStdString());
        }
        case NumberType: {
            Symbol numData(m_doubleData / other.m_doubleData);
            return numData;
        }
        default:
            QString strErrorMsg = "Type Error: can not apply \"/\" on invalid types";
            throw (strErrorMsg.toStdString());
        }
    }
}

bool Symbol::operator >(const Symbol &other) const
{
    if (m_type != other.m_type) {
        QString strErrorMsg = "Type Error: can not apply \">\" on two different types";
        throw (strErrorMsg.toStdString());
    }
    else {
        switch (m_type)
        {
        case StringType:
            return (m_strData > other.m_strData)? true : false;
        case NumberType:
            return (m_doubleData > other.m_doubleData)? true : false;
        default:
            return false;
        }
    }
}

bool Symbol::operator <(const Symbol &other) const
{
    if (m_type != other.m_type) {
        QString strErrorMsg = "Type Error: can not apply \"<\" on two different types";
        throw (strErrorMsg.toStdString());
    }
    else {
        switch (m_type)
        {
        case StringType:
            return (m_strData < other.m_strData)? true : false;
        case NumberType:
            return (m_doubleData < other.m_doubleData)? true : false;
        default:
            return false;
        }
    }
}

bool Symbol::operator >=(const Symbol &other) const
{
    if (m_type != other.m_type) {
        QString strErrorMsg = "Type Error: can not apply \">=\" on two different types";
        throw (strErrorMsg.toStdString());
    }
    else {
        switch (m_type)
        {
        case StringType:
            return (m_strData >= other.m_strData)? true : false;
        case NumberType:
            return (m_doubleData >= other.m_doubleData)? true : false;
        default:
            return false;
        }
    }
}

bool Symbol::operator <=(const Symbol &other) const
{
    if (m_type != other.m_type) {
        QString strErrorMsg = "Type Error: can not apply \"<=\" on two different types";
        throw (strErrorMsg.toStdString());
    }
    else {
        switch (m_type)
        {
        case StringType:
            return (m_strData <= other.m_strData)? true : false;
        case NumberType:
            return (m_doubleData <= other.m_doubleData)? true : false;
        default:
            return false;
        }
    }
}

bool Symbol::operator ==(const Symbol &other) const
{
    if (m_type != other.m_type) {
        QString strErrorMsg = "Type Error: can not apply \"==\" on two different types";
        throw (strErrorMsg.toStdString());
    }
    else {
        switch (m_type)
        {
        case StringType:
            return (m_strData == other.m_strData)? true : false;
        case NumberType:
            return (m_doubleData == other.m_doubleData)? true : false;
        default:
            return false;
        }
    }
}

bool Symbol::operator !=(const Symbol &other) const
{
    if (m_type != other.m_type) {
        QString strErrorMsg = "Type Error: can not apply \"!=\" on two different types";
        throw (strErrorMsg.toStdString());
    }
    else {
        switch (m_type)
        {
        case StringType:
            return (m_strData != other.m_strData)? true : false;
        case NumberType:
            return (m_doubleData != other.m_doubleData)? true: false;
        default:
            return false;
        }
    }
}
