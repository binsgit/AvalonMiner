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
    void rpcConnected();
    void newInfo(const minerInfo &);
//    void error(int socketError, const QString &message);
    void newValue(const plotInfo &);

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
    bool callApi(const char *);
    void addToPlot(bool, plotInfo);
    void parseSummary();
    void parsePools();
    QString getBfgValue(const char *);
    QString getBfgValue(const QString &);
    QString getBfgValue(const char *, const char *);
    QString getBfgValue(const QString &, const char *);
    QString getBfgValue(const char *, const QString &);
    QString getBfgValue(const QString &, const QString &);
};

#endif // RPCTHREAD_H
