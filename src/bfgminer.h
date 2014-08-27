#ifndef BFGMINER_H
#define BFGMINER_H

#include <QObject>
#include <QProcess>
#include <QStringList>
#include "bfgconf.h"

class BFGMiner : public QObject
{
    Q_OBJECT
public:
    explicit BFGMiner(QObject *parent = 0);
    ~BFGMiner();

signals:
    void started();
    void finished(int, QProcess::ExitStatus);

public slots:
    void run(bool, QStringList);
    void confChanged(bfgConf *);
    void readStdOutput();
    void readStdError();
    void readError();

private:
    QProcess process;
    QStringList para;
};

#endif // BFGMINER_H
