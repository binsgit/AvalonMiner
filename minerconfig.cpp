#include <QApplication>
#include <QDebug>
#include "minerconfig.h"

MinerConfig::MinerConfig(QObject *parent) :
    QObject(parent)
{
    settings = new QSettings(qApp->applicationDirPath() + "/avalon.ini", QSettings::IniFormat);
}

MinerConfig::~MinerConfig()
{
    delete settings;
}

void MinerConfig::load()
{
    unsigned int i;

    settings->sync();
    for (i = 0; i < sizeof(bfgConf)/sizeof(poolInfo); i ++) {
        switch (i) {
        case 0:
            bfgConfig.info[i].url = settings->value(QString("PoolInfo_%1/PoolURL").arg(QString::number(i+1)), "stratum+tcp://stratum.ozco.in").toString();
            break;
        case 1:
            bfgConfig.info[i].url = settings->value(QString("PoolInfo_%1/PoolURL").arg(QString::number(i+1)), "stratum+tcp://au.ozco.in").toString();
            break;
        case 2:
            bfgConfig.info[i].url = settings->value(QString("PoolInfo_%1/PoolURL").arg(QString::number(i+1)), "stratum+tcp://eustratum.ozco.in").toString();
            break;
        default:
            bfgConfig.info[i].url = settings->value(QString("PoolInfo_%1/PoolURL").arg(QString::number(i+1))).toString();
            break;
        }
        bfgConfig.info[i].port = settings->value(QString("PoolInfo_%1/PoolPort").arg(QString::number(i+1)), "80").toInt();
        bfgConfig.info[i].name = settings->value(QString("PoolInfo_%1/WorkerName").arg(QString::number(i+1)), "canaan.default").toString();
        bfgConfig.info[i].pass = settings->value(QString("PoolInfo_%1/WorkerPassword").arg(QString::number(i+1)), "1234").toString();

        settings->setValue(QString("PoolInfo_%1/PoolURL").arg(QString::number(i+1)), bfgConfig.info[i].url);
        settings->setValue(QString("PoolInfo_%1/PoolPort").arg(QString::number(i+1)), QString::number(bfgConfig.info[i].port));
        settings->setValue(QString("PoolInfo_%1/WorkerName").arg(QString::number(i+1)), bfgConfig.info[i].name);
        settings->setValue(QString("PoolInfo_%1/WorkerPassword").arg(QString::number(i+1)), bfgConfig.info[i].pass);
    }
    bfgConfig.log = settings->value(QString("Avalon/Log"), "avalon.log").toString();
    QStringList para_default = QString("-T -S ICA:/dev/ttyACM0 --set-device ICA:baud=115200 --set-device ICA:reopen=timeout --set-device ICA:work_division=1 --set-device ICA:fpga_count=1 --set-device ICA:probe_timeout=100 --set-device ICA:timing=0.22 --api-listen").split(" ");
    bfgConfig.para = settings->value(QString("Avalon/Parameters"), para_default).toStringList();
    settings->setValue(QString("Avalon/Log"), bfgConfig.log);
    settings->setValue(QString("Avalon/Parameters"), bfgConfig.para);
    settings->sync();
    emit confLoaded(&bfgConfig);
}

void MinerConfig::load(bfgConf *conf)
{
    unsigned int i;
    for (i = 0; i < sizeof(bfgConf)/sizeof(poolInfo); i ++) {
        bfgConfig.info[i].url = conf->info[i].url;
        bfgConfig.info[i].port = conf->info[i].port;
        bfgConfig.info[i].name = conf->info[i].name;
        bfgConfig.info[i].pass = conf->info[i].pass;

        settings->setValue(QString("PoolInfo_%1/PoolURL").arg(QString::number(i+1)), bfgConfig.info[i].url);
        settings->setValue(QString("PoolInfo_%1/PoolPort").arg(QString::number(i+1)), QString::number(bfgConfig.info[i].port));
        settings->setValue(QString("PoolInfo_%1/WorkerName").arg(QString::number(i+1)), bfgConfig.info[i].name);
        settings->setValue(QString("PoolInfo_%1/WorkerPassword").arg(QString::number(i+1)), bfgConfig.info[i].pass);
    }
//    bfgConfig.log = conf->log;
//    bfgConfig.para = conf->para;
    settings->setValue(QString("Avalon/Log"), bfgConfig.log);
    settings->setValue(QString("Avalon/Parameters"), bfgConfig.para);
    settings->sync();
    emit confSaved(&bfgConfig);
}
