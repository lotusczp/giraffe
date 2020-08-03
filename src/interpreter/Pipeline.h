#ifndef PIPELINE_H
#define PIPELINE_H

#include <QString>

class Pipeline
{
public:
    static Pipeline* instance();

    static void destroy();

    void console_out_num(double value);
    void console_out_string(QString value);


    void insertEmbeddedVar();

    void insertEmbeddedFunc();

private:
    Pipeline();
    ~Pipeline();

private:
    static Pipeline  *s_pInstance;
};

#endif // PIPELINE_H
