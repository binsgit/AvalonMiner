#ifndef BFGCONF_H
#define BFGCONF_H

#include <QString>
#include <QStringList>
#include <QDateTime>

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
    QString when;
    QString hashrate_20s;
    QString hashrate_av;
    QString hashrate_cur;
    QString poolUrl[3];
    QString active[3];
    QString lastCommit[3];
};

enum Hashrate
{
    value_20s,
    value_av,
    value_cur,
    value_N,
};

struct plotInfo
{
    qint64 when;            // ~~ QDateTime::currentMSecsSinceEpoch()
    double value[value_N];  // 20s, av, and cur
};

#endif // BFGCONF_H
