#ifndef MINERCONFIG_H
#define MINERCONFIG_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QSettings>
#include "bfgconf.h"

class MinerConfig : public QObject
{
    Q_OBJECT
public:
    explicit MinerConfig(QObject *parent = 0);
    ~MinerConfig();
    void load();

signals:
    void confLoaded(bfgConf *);
    void confSaved(bfgConf *);

public slots:
    void load(bfgConf *);

private:
    bfgConf bfgConfig;
    QSettings *settings;
};

#endif // MINERCONFIG_H
