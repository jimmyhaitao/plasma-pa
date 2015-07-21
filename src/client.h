#ifndef CLIENT_H
#define CLIENT_H

#include <pulse/introspect.h>

#include <QMap>

#include "pulseobject.h"

class Q_DECL_EXPORT Client : public PulseObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
public:
    Client(QObject *parent);

    void update(const pa_client_info *info);

    QString name() const { return m_name; }

signals:
    void nameChanged();

private:
    QString m_name;
};

#endif // CLIENT_H
