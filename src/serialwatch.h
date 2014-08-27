#ifndef SERIALWATCH_H
#define SERIALWATCH_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QTimer>
#include <QMutex>

class SerialWatch : public QObject
{
    Q_OBJECT

public:
    explicit SerialWatch(QObject *parent = 0);
    ~SerialWatch();
    QStringList getPorts();
    QStringList getOldPorts();

signals:
    void changed(const QStringList &);

public slots:
    void start();
    void stop();

private:
    QTimer timer;
    QMutex mutex;
    QStringList oldPorts;
    QStringList newPorts;
    bool firstWatch;

private slots:
    void watchPorts();
};

#endif // SERIALWATCH_H
