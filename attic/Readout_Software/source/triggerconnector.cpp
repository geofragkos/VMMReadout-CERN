#include "triggerconnector.h"
#include "constants.h"

triggerConnector::triggerConnector(QObject *parent)
    : QTcpServer(parent)
{
}
