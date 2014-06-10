#include <QDebug>
#include <QDateTime>
#include <QtNetwork/QTcpSocket>
#include <QStringList>
#include <QRegExp>
#include "rpcthread.h"

RpcThread::RpcThread(QObject *parent) :
    QThread(parent), serverName(QString("127.0.0.1")), serverPort(4028), quit(false)
{
}

RpcThread::~RpcThread()
{
}

bool RpcThread::callApi(const char *command)
{
    qDebug() << "command : " + QString(command);
    const int Timeout = 300; //1 * 1000;

    QTcpSocket socket;
    socket.connectToHost(serverName, serverPort);

    if (!socket.waitForConnected(Timeout)) {
        qDebug() << "socket connect error: " + socket.errorString();
//        emit error(socket.error(), socket.errorString());
        return false;
    }
    qDebug() << "connected";

    if (socket.write(command) != QString(command).length()) {
        qDebug() << "socket write error.";
        return false;
    }
    qDebug() << "send";

    while (socket.bytesAvailable() < (int)sizeof(quint16)) {
        if (!socket.waitForReadyRead(Timeout)) {
            qDebug() << "socket read error: " + socket.errorString();
//            emit error(socket.error(), socket.errorString());
            return false;
        }
    }
    qDebug() << "read";
    mutex.lock();
    buffer = socket.readAll();
    mutex.unlock();

    socket.close();
    return true;
}

void RpcThread::run()
{
    qDebug() << "run";
    miner.hashrate_20s = "0.000";
    miner.hashrate_av = "0.000";
    miner.hashrate_cur = "0.000";

    for (int i = 0; i < 3; i ++) {
        miner.poolUrl[i] = QString("");
        miner.active[i] = QString("");
        miner.lastCommit[i] = QString("");
    }

    while (!quit) {
        qDebug () << "while";
        if (callApi("summary")) {
//            qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) + "\n" + buffer + "\n";
            parseSummary();
        } else {
            continue;
        }
        if (quit)
            break;
        if (callApi("pools")) {
//            qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) + "\n" + buffer + "\n";
            parsePools();
        } else {
            continue;
        }
        if (quit)
            break;
        emit newInfo(miner);
        for (int i = 0; i < 25; i ++) {
            msleep(200);
            if (quit)
                break;
        }
    }
}

void RpcThread::request()
{
    qDebug() << "request";

    mutex.lock();
    quit = false;
    mutex.unlock();

    if (!isRunning()) {
        start();
    } else {
        cond.wakeOne();
    }
}

void RpcThread::stop()
{
    qDebug() << "stop";

    mutex.lock();
    quit = true;
    mutex.unlock();

    cond.wakeOne();
    wait();
}

//static const char SEPARATOR = '|';
//static const char COMMA = ',';
//static const char EQ = '=';

void RpcThread::parseSummary()
{
    QStringList eachSeparator = buffer.split("|");

    foreach (const QString &separator, eachSeparator) {
        QStringList comma = separator.split(",");
        foreach (const QString &para, comma) {
            if (para.contains("MHS av=")) {
                QString tmp = para;
                bool ok;
                double value = tmp.replace("MHS av=", "").toDouble(&ok);
                if (ok) {
                    qDebug() << para;
                    miner.hashrate_av = QString::number(value / 1000.0, 'f', 3);
                } else {
                    qDebug() << "convert av failed";
                    miner.hashrate_av = QString("0.000");
                }
            }
            if (para.contains("MHS 20s=")) {
                QString tmp = para;
                bool ok;
                double value = tmp.replace("MHS 20s=", "").toDouble(&ok);
                if (ok) {
                    qDebug() << para;
                    miner.hashrate_20s = QString::number(value / 1000.0, 'f', 3);
                } else {
                    qDebug() << "convert 20s failed";
                    miner.hashrate_20s = QString("0.000");
                }
            }
        }
    }
}

void RpcThread::parsePools()
{
    QStringList eachSeparator = buffer.split("|");

    for (int i = 0; i < 3; i ++) {
        foreach (const QString &separator, eachSeparator) {
            if (!separator.contains(QRegExp(QString("^POOL=%1").arg(QString::number(i)))))
                continue;
            qDebug() << "Pool number: " + QString::number(i);
            QStringList comma = separator.split(",");
            foreach (const QString &para, comma) {
                if (para.contains(QRegExp("^URL="))) {
                    QString tmp = para;
                    miner.poolUrl[i] = tmp.replace("URL=", "");
                }
                if (para.contains(QRegExp("^Stratum Active="))) {
                    QString tmp = para;
                    miner.active[i] = tmp.replace("Stratum Active=", "");
                }
                if (para.contains(QRegExp("^Last Share Time="))) {
                    QString tmp = para;
                    bool ok;
                    uint value = tmp.replace("Last Share Time=", "").toUInt(&ok);
                    if (ok && (value != 0)) {
                        qDebug() << para;
                        miner.lastCommit[i] = QDateTime::fromTime_t(value).toString("HH:mm:ss");
                    } else {
                        qDebug() << "convert time failed";
                        miner.lastCommit[i] = QString("");
                    }
                }
            }
        }
        qDebug() << "url" + QString::number(i) + ":" + miner.poolUrl[i];
        qDebug() << "active" + QString::number(i) + ":" + miner.active[i];
        qDebug() << "lastCommit" + QString::number(i) + ":" + miner.lastCommit[i];
    }

}
