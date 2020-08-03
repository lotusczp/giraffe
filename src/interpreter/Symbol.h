#ifndef SYMBOL_H
#define SYMBOL_H

#include <QString>

class Symbol
{
public:

    enum Type {
        invalid = -1,
        StringType,
        NumberType
    };

    Symbol(const QString& str);
    Symbol(long double number);
    Symbol();

    const Type& type() const;

    long double getNumber() const;
    void setNumber(long double value);

    const QString& getString() const;
    void setString(const QString& str);

    Symbol operator + (const Symbol &other) const;
    Symbol operator - (const Symbol &other) const;
    Symbol operator * (const Symbol &other) const;
    Symbol operator / (const Symbol &other) const;


    bool operator > (const Symbol &other) const;
    bool operator < (const Symbol &other) const;
    bool operator >= (const Symbol &other) const;
    bool operator <= (const Symbol &other) const;
    bool operator == (const Symbol &other) const;
    bool operator != (const Symbol &other) const;

private:
    QString m_strData;

    long double m_doubleData;

    Type m_type;
};

#endif // SYMBOL_H
