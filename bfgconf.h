#ifndef BFGCONF_H
#define BFGCONF_H

#include <QString>
#include <QStringList>

struct poolInfo
{
    QString url;
    int port;
    QString name;
    QString pass;
};

struct bfgConf
{
    poolInfo info[3];
    QString log;
    QStringList para;
};

struct minerInfo
{
    QString hashrate_20s;
    QString hashrate_av;
    QString hashrate_cur;
    QString poolUrl[3];
    QString active[3];
    QString lastCommit[3];
};

#endif // BFGCONF_H
