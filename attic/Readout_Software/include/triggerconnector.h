#ifndef TRIGGERCONNECTOR_H
#define TRIGGERCONNECTOR_H

#include <QObject>
#include "constants.h"

class triggerConnector : public QTCP
{
    Q_OBJECT
public:
    explicit triggerConnector(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // TRIGGERCONNECTOR_H
