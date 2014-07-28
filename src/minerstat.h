#include <QObject>
#include <QMutex>
#include <QQueue>
#include <QDateTime>
#include <qdatetime.h>
#include "bfgconf.h"

#define HISTORY 60  // seconds

class MinerStat : public QObject
{
    Q_OBJECT
public:
    explicit MinerStat(QObject *parent = 0);
    ~MinerStat();
    QTime upTime() const;
    void clearValue();

    QMutex valueMutex;
    /* We should keep 10 more value in buffer for display */
    double value[value_N][HISTORY+10];
    double when[HISTORY+10];
    unsigned int valueCount;
    double valueMax;
    QDateTime *baseTime;

public slots:
    void addValue(const plotInfo &);

private:
};
