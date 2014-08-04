#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include "bfgminer.h"

BFGMiner::BFGMiner(QObject *parent) :
    QObject(parent)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef Q_OS_WIN
    env.insert("PATH", QString(qApp->applicationDirPath() + "/bfgminer;C:/Windows/System32;C:/Windows;"));
#else
    env.insert("LD_LIBRARY_PATH", QString(qApp->applicationDirPath() + "/bfgminer"));
#endif
    process.setProcessEnvironment(env);
//    process.setWorkingDirectory(qApp->applicationDirPath() + "/bfgminer");
    process.setWorkingDirectory(qApp->applicationDirPath());

    connect(&process, SIGNAL(started()), this, SIGNAL(started()));
    connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SIGNAL(finished(int, QProcess::ExitStatus)));
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdOutput()));
    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(readStdError()));
    connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(readError()));
}

BFGMiner::~BFGMiner()
{

}

void BFGMiner::run(bool action)
{
    /* Stop */
    if (!action) {
        process.kill();
        return;
    }

    /* Start */
#ifdef Q_OS_WIN
    QFile bfgFile(QString(qApp->applicationDirPath() + "/bfgminer/bfgminer.exe"));
#else
    QFile bfgFile(QString(qApp->applicationDirPath() + "/bfgminer/bfgminer"));
#endif
    if (!bfgFile.exists()) {
        QMessageBox messageBox;
        qCritical("ERROR: File not found: %s", QString(bfgFile.fileName()).toLocal8Bit().data());
        messageBox.critical(0, "Error", "File not found: " + bfgFile.fileName());
//        qApp->quit();
        return;
    }
    process.start(bfgFile.fileName(), para);
}

void BFGMiner::confChanged(bfgConf *bfgConfig)
{
    unsigned int i;
    para = QStringList();
    for (i = 0; i < sizeof(bfgConf)/sizeof(poolInfo); i ++) {
        para += QStringList("-o") + QStringList(bfgConfig->info[i].url + QString(":") + QString::number(bfgConfig->info[i].port));
        para += QStringList("-O") + QStringList(bfgConfig->info[i].name + QString(":") + bfgConfig->info[i].pass);
    }
    para += bfgConfig->para;
    para += QStringList("--log-file") + QStringList(bfgConfig->log);
    qDebug() << para;
}

void BFGMiner::readStdOutput()
{
//    qDebug() << process.readAllStandardOutput();
    return;
}

void BFGMiner::readStdError()
{
//    qDebug() << process.readAllStandardError();
    return;
}

void BFGMiner::readError()
{
//    qDebug() << "An Error Occured! Error Code is " + QString::number(process.error());
    return;
}
