#include <QApplication>
#include <QMetaType>
#include "mainwindow.h"
#include "minerconfig.h"
#include "bfgminer.h"
#include "rpcthread.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MinerConfig minerConfig;
    MainWindow w;
    BFGMiner bfg;
    RpcThread rpc;

    qRegisterMetaType<minerInfo>("minerInfo");

    QObject::connect(&minerConfig, SIGNAL(confLoaded(bfgConf*)), &w, SLOT(confChanged(bfgConf*)));
    QObject::connect(&minerConfig, SIGNAL(confSaved(bfgConf*)), &w, SLOT(confSaved()));
    QObject::connect(&minerConfig, SIGNAL(confLoaded(bfgConf*)), &bfg, SLOT(confChanged(bfgConf*)));
    QObject::connect(&minerConfig, SIGNAL(confSaved(bfgConf*)), &bfg, SLOT(confChanged(bfgConf*)));
    QObject::connect(&w, SIGNAL(saveConf(bfgConf*)), &minerConfig, SLOT(load(bfgConf*)));
    QObject::connect(&w, SIGNAL(runProcess(bool)), &bfg, SLOT(run(bool)));
    QObject::connect(&bfg, SIGNAL(started()), &w, SLOT(processStarted()));
    QObject::connect(&bfg, SIGNAL(finished(int,QProcess::ExitStatus)), &w, SLOT(processFinished()));
    QObject::connect(&bfg, SIGNAL(started()), &rpc, SLOT(request()));
    QObject::connect(&bfg, SIGNAL(finished(int,QProcess::ExitStatus)), &rpc, SLOT(stop()));
    QObject::connect(&rpc, SIGNAL(newInfo(minerInfo)), &w, SLOT(updateInfo(minerInfo)));
    QObject::connect(&rpc, SIGNAL(rpcConnected()), &w, SLOT(rpcConnected()));
    minerConfig.load();
    w.show();

    return a.exec();
}
