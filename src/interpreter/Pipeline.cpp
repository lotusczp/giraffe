#include "Pipeline.h"

#include <QDebug>

Pipeline* Pipeline::s_pInstance = NULL;

Pipeline *Pipeline::instance()
{
    // Do we have an oracle yet?
    if(s_pInstance == NULL) {
      // No, create it
      s_pInstance = new Pipeline();
    }

    // Return the instance ptr
    return s_pInstance;
}

void Pipeline::destroy()
{
    // Clean up the pointer
    if(s_pInstance != NULL) {
        delete s_pInstance, s_pInstance = NULL;
    }
}

void Pipeline::console_out_num(double value)
{
    qDebug() << value;
}

void Pipeline::console_out_string(QString value)
{
    qDebug() << value;
}

Pipeline::Pipeline()
{
}

Pipeline::~Pipeline()
{

}
