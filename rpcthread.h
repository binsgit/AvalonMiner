#ifndef RPCTHREAD_H
#define RPCTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "bfgconf.h"

class RpcThread : public QThread
{
    Q_OBJECT
public:
    explicit RpcThread(QObject *parent = 0);
    ~RpcThread();

    void run();

signals:
    void newInfo(const minerInfo &miner);
//    void error(int socketError, const QString &message);

public slots:
    void request();
    void stop();

private:
    QString serverName;
    quint16 serverPort;
    bool quit;
    QMutex mutex;
    QWaitCondition cond;
    QString buffer;
    minerInfo miner;
    bool callApi(const char *command);
    void parseSummary();
    void parsePools();
};

#endif // RPCTHREAD_H
