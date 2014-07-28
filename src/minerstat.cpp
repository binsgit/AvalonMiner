#include <qstringlist.h>
#include <qfile.h>
#include <qtextstream.h>
#include <QDebug>
#include "minerstat.h"

MinerStat::MinerStat(QObject *parent) :
    QObject(parent), valueCount(0), valueMax(0)
{
    for (unsigned int j = 0; j < sizeof(when)/sizeof(when[0]); j ++) {
        for (int i = 0; i < value_N; i ++) {
            value[i][j] = 0;
        }
        when[j] = 0;
    }
    baseTime = new QDateTime();
}

MinerStat::~MinerStat()
{
    if (!baseTime) {
        delete baseTime;
    }
}

QTime MinerStat::upTime() const
{
    QDateTime tmp(QDateTime::currentDateTime());
    QTime t = tmp.time();
    t = t.addSecs(- (t.second() % 10) - HISTORY);

    baseTime->setDate(tmp.date());
    baseTime->setTime(t);

    return t;
}

void MinerStat::addValue(const plotInfo &plotValue)
{
    valueMutex.lock();

    for (int j = sizeof(when)/sizeof(when[0])-1; j > 0; j --) {
        for (int i = 0; i < value_N; i ++) {
            value[i][j] = value[i][j-1];
        }
        when[j] = when[j-1];
    }

    when[0] = baseTime->secsTo(QDateTime::fromMSecsSinceEpoch(plotValue.when));
    for (int i = 0; i < value_N; i ++) {
        value[i][0] = plotValue.value[i];
    }

    if (valueCount < sizeof(when)/sizeof(when[0]))
        valueCount ++;

    valueMax = 0;
    for (int i = 0; i < value_N; i ++) {
        for (unsigned int j = 0; j < valueCount; j ++)
            if (valueMax < value[i][j])
                valueMax = value[i][j];
    }

    valueMutex.unlock();
}

void MinerStat::clearValue()
{
    valueMutex.lock();
    for (unsigned int j = 0; j < sizeof(when)/sizeof(when[0]); j ++) {
        for (int i = 0; i < value_N; i ++) {
            value[i][j] = 0;
        }
        when[j] = 0;
    }
    valueCount = 0;
    valueMax = 0;
    valueMutex.unlock();
}
