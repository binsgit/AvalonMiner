#ifndef MINERCONFIG_H
#define MINERCONFIG_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QSettings>
#include "bfgconf.h"
#include "serialwatch.h"

class MinerConfig : public QObject
{
    Q_OBJECT
public:
    explicit MinerConfig(QObject *parent = 0);
    ~MinerConfig();
    void load();
    void watcherStart();

signals:
    void confLoaded(bfgConf *);
    void confSaved(bfgConf *);
    void serialPara(const QStringList &);

public slots:
    void load(bfgConf *);

private:
    bfgConf bfgConfig;
    QSettings *settings;
    SerialWatch watcher;

private slots:
    void serialChanged(const QStringList &);
};

#endif // MINERCONFIG_H
