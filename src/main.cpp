#include <QApplication>
#include <QMetaType>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMessageBox>
#include <QDir>
#include "mainwindow.h"
#include "minerconfig.h"
#include "bfgminer.h"
#include "rpcthread.h"

#define SAVE_LOG

/* http://developer.nokia.com/community/wiki/How_to_redirect_qDebug_output_to_a_file
 * http://www.qtcentre.org/threads/55638-qInstallMsgHander-identifier-not-found
 * http://qt-project.org/doc/qt-5/sourcebreaks.html
 */
#ifdef SAVE_LOG
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void crashMessageOutput(QtMsgType type, const QMessageLogContext &, const QString & str)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("[" + QDateTime::currentDateTime().toString(Qt::ISODate) + " Debug]    " + str);
        break;

    case QtWarningMsg:
        txt = QString("[" + QDateTime::currentDateTime().toString(Qt::ISODate) + " Warning]  " + str);
        break;
    case QtCriticalMsg:
        txt = QString("[" + QDateTime::currentDateTime().toString(Qt::ISODate) + " Critical] " + str);
        break;
    case QtFatalMsg:
        txt = QString("[" + QDateTime::currentDateTime().toString(Qt::ISODate) + " Fatal]    " + str);
        abort();
    }
#else
void customMessageHandler(QtMsgType type, const char *msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("[" + QDateTime::currentDateTime().toString(Qt::ISODate) + " Debug]    %1").arg(msg);
        break;

    case QtWarningMsg:
        txt = QString("[" + QDateTime::currentDateTime().toString(Qt::ISODate) + " Warning]  %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("[" + QDateTime::currentDateTime().toString(Qt::ISODate) + " Critical] %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("[" + QDateTime::currentDateTime().toString(Qt::ISODate) + " Fatal]    %1").arg(msg);
        abort();
    }
#endif  // QT_VERSION

    QString logDir(qApp->applicationDirPath() + "/log/");
    QFile outFile(QString(logDir + QDateTime::currentDateTime().toString("yyyyMM") + ".log"));
    if (!QDir(logDir).exists()) {
        QDir(logDir).mkpath(logDir);
    }
    outFile.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
    outFile.close();
}
#endif  // SAVE_LOG

#define LOCAL_SERVER_NAME   "AVALON_MINER_LOCAL_SERVER"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /* Lets register our custom handler, before we start */
#ifdef SAVE_LOG
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qInstallMessageHandler(crashMessageOutput);
#else
    qInstallMsgHandler(customMessageHandler);
#endif  // QT_VERSION
#endif  // SAVE_LOG

    qCritical(" ");
    qCritical(" ");

    /* Check Single Instance */
    /* http://developer.nokia.com/community/wiki/Run_only_one_instance_of_a_Qt_application */
    QLocalSocket sock;
    sock.connectToServer(LOCAL_SERVER_NAME, QIODevice::ReadWrite);
    if (sock.waitForConnected(500)) {
        sock.close();
        QMessageBox messageBox;
        qCritical("Application is running now");
        messageBox.critical(0, "Error", "Application is running now.\n\nYou should run ONLY ONE instance.\n");
        a.quit();
        return (-1);
    }
    sock.close();
    QLocalServer localserver;
    localserver.listen(LOCAL_SERVER_NAME);

    MinerConfig minerConfig;
    MainWindow w;
    BFGMiner bfg;
    RpcThread rpc;

    qRegisterMetaType<minerInfo>("minerInfo");
    qRegisterMetaType<plotInfo>("plotInfo");

    QObject::connect(&minerConfig, SIGNAL(confLoaded(bfgConf*)), &w, SLOT(confChanged(bfgConf*)));
    QObject::connect(&minerConfig, SIGNAL(confSaved(bfgConf*)), &w, SLOT(confSaved()));
    QObject::connect(&minerConfig, SIGNAL(serialPara(QStringList)), &w, SLOT(serialReady(QStringList)));
    QObject::connect(&minerConfig, SIGNAL(confLoaded(bfgConf*)), &bfg, SLOT(confChanged(bfgConf*)));
    QObject::connect(&minerConfig, SIGNAL(confSaved(bfgConf*)), &bfg, SLOT(confChanged(bfgConf*)));
    QObject::connect(&w, SIGNAL(saveConf(bfgConf*)), &minerConfig, SLOT(load(bfgConf*)));
    QObject::connect(&w, SIGNAL(runProcess(bool, QStringList)), &bfg, SLOT(run(bool, QStringList)));
    QObject::connect(&bfg, SIGNAL(started()), &w, SLOT(processStarted()));
    QObject::connect(&bfg, SIGNAL(finished(int,QProcess::ExitStatus)), &w, SLOT(processFinished()));
    QObject::connect(&bfg, SIGNAL(started()), &rpc, SLOT(request()));
    QObject::connect(&bfg, SIGNAL(finished(int,QProcess::ExitStatus)), &rpc, SLOT(stop()));
    QObject::connect(&rpc, SIGNAL(newInfo(minerInfo)), &w, SLOT(updateInfo(minerInfo)));
    QObject::connect(&rpc, SIGNAL(rpcConnected()), &w, SLOT(rpcConnected()));
    QObject::connect(&rpc, SIGNAL(newValue(plotInfo)), &w, SIGNAL(addPlotValue(plotInfo)));
    minerConfig.load();
    w.show();
    minerConfig.watcherStart();

    return a.exec();
}
