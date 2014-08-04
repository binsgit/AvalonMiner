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
//    qDebug() << "Call bfgminer RPC with command : " + QString(command);
    const int Timeout = 300; //1 * 1000;

    QTcpSocket socket;
    socket.connectToHost(serverName, serverPort);

    if (!socket.waitForConnected(Timeout)) {
        qDebug() << "socket connect error: " + socket.errorString();
//        emit error(socket.error(), socket.errorString());
        return false;
    }
    emit rpcConnected();

    if (socket.write(command) != QString(command).length()) {
        qDebug() << "socket write error.";
        return false;
    }

    while (socket.bytesAvailable() < (int)sizeof(quint16)) {
        if (!socket.waitForReadyRead(Timeout)) {
            qDebug() << "socket read error: " + socket.errorString();
//            emit error(socket.error(), socket.errorString());
            return false;
        }
    }
    mutex.lock();
    buffer = socket.readAll();
    mutex.unlock();

    socket.close();
    return true;
}

void RpcThread::run()
{
    miner.hashrate_20s = "0";
    miner.hashrate_av = "0";
    miner.hashrate_cur = "0";

    for (int i = 0; i < 3; i ++) {
        miner.poolUrl[i] = QString("");
        miner.active[i] = QString("");
        miner.lastCommit[i] = QString("");
    }

    while (!quit) {
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
        /* sleep 5 seconds (25 * 200) ==> sleep 1 second (5 * 200) */
//        for (int i = 0; i < 25; i ++) {
        for (int i = 0; i < 5; i ++) {
            msleep(200);
            if (quit)
                break;
        }
    }
}

void RpcThread::request()
{
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
    mutex.lock();
    quit = true;
    mutex.unlock();

    cond.wakeOne();
    wait();
}

//static const char SEPARATOR = '|';
//static const char COMMA = ',';
//static const char EQ = '=';

QString RpcThread::getBfgValue(const char *str)
{
    return getBfgValue(QString(str));
}

/* "str" should be sth. like "MHS av", "MHS 20s", etc. */
QString RpcThread::getBfgValue(const QString &str)
{
    QStringList eachSeparator = buffer.split("|");

    foreach (const QString &separator, eachSeparator) {
        QStringList comma = separator.split(",");
        foreach (const QString &para, comma) {
            if (para.contains(QRegExp("^" + str + "="))) {
                QString tmp = para;
                return tmp.replace(str + "=", "");
            }
        }
    }

    return QString("");
}

QString RpcThread::getBfgValue(const char *first, const char *second)
{
    return getBfgValue(QString(first), QString(second));
}

QString RpcThread::getBfgValue(const QString &first, const char *second)
{
    return getBfgValue(first, QString(second));
}

QString RpcThread::getBfgValue(const char *first, const QString &second)
{
    return getBfgValue(QString(first), second);
}

/*
 * "first" should be string like "POOL=0", "STATUS=S", etc.
 * "second" should be a string to get value, such as "URL", "Stratum Active", etc.
 */
QString RpcThread::getBfgValue(const QString &first, const QString &second)
{
    QStringList eachSeparator = buffer.split("|");

    foreach (const QString &separator, eachSeparator) {
        if (!separator.contains(QRegExp(QString("^" + first))))
            continue;
        QStringList comma = separator.split(",");
        foreach (const QString &para, comma) {
            if (para.contains(QRegExp("^" + second + "="))) {
                QString tmp = para;
                return tmp.replace(second + "=", "");
            }
        }
    }
    return QString("");
}

void RpcThread::addToPlot(bool isPlot, plotInfo plotValue)
{
    if (!isPlot) {
        qDebug() << "ERROR, we should not plot";
        return;
    }
//    qDebug() << "plotValue when=" + QString::number(plotValue.when);
//    qDebug() << "plotValue 20s=" + QString::number(plotValue.hashrate_20s);
//    qDebug() << "plotValue av=" + QString::number(plotValue.hashrate_av);
//    qDebug() << "plotValue cur=" + QString::number(plotValue.hashrate_cur);
    emit newValue(plotValue);
}

void RpcThread::parseSummary()
{
    mutex.lock();
    QString When = getBfgValue("When");
    QString MHS_av = getBfgValue("MHS av");
    QString MHS_20s = getBfgValue("MHS 20s");
    mutex.unlock();

    bool ok;
    bool isPlot = true;
    plotInfo plotValue;

    qint64 value64 = When.toLongLong(&ok);
    if (ok) {
        plotValue.when = value64 * 1000;
        miner.when = QDateTime::fromMSecsSinceEpoch(plotValue.when).toString("yyyy-MM-dd hh:mm:ss");
    } else {
        qDebug() << "when = () [convert when failed]";
        miner.when = QString();
        isPlot = false;
    }
//    qDebug() << "When = " + miner.when;

    double value = MHS_av.toDouble(&ok);
    if (ok) {
        plotValue.value[value_av] = value / 1000.0;
        miner.hashrate_av = QString::number(plotValue.value[value_av], 'f', 3);
    } else {
        qDebug() << "GHS av = 0 [convert av failed]";
        plotValue.value[value_av] = 0;
        miner.hashrate_av = QString("0");
    }
//    qDebug() << "GHS av = " + miner.hashrate_av;

    value = MHS_20s.toDouble(&ok);
    if (ok) {
        plotValue.value[value_20s] = value / 1000.0;
        miner.hashrate_20s = QString::number(plotValue.value[value_20s], 'f', 3);
    } else {
        qDebug() << "GHS 20s = 0 [convert 20s failed]";
        plotValue.value[value_20s] = 0;
        miner.hashrate_20s = QString("0");
    }
//    qDebug() << "GHS 20s = " + miner.hashrate_20s;

    /* MHS Cur = Diff1 Work * (Difficulty Accepted/(Difficulty Accepted+Difficulty Rejected+Difficulty Stale)*60/(Device Elapsed)*71582788/(1000000) */
    mutex.lock();
    QString Diff1_Work = getBfgValue("Diff1 Work");
    QString Difficulty_Accepted = getBfgValue("Difficulty Accepted");
    QString Difficulty_Rejected = getBfgValue("Difficulty Rejected");
    QString Difficulty_Stale = getBfgValue("Difficulty Stale");
    QString Elapsed = getBfgValue("Elapsed");
    mutex.unlock();
//    qDebug() << "Diff1_Work=" + Diff1_Work;
//    qDebug() << "Difficulty_Accepted=" + Difficulty_Accepted;
//    qDebug() << "Difficulty_Rejected=" + Difficulty_Rejected;
//    qDebug() << "Difficulty_Stale=" + Difficulty_Stale;
//    qDebug() << "Elapsed=" + Elapsed;
    if (Diff1_Work.isEmpty() || Difficulty_Accepted.isEmpty() || Difficulty_Rejected.isEmpty() || Difficulty_Stale.isEmpty() || Elapsed.isEmpty()) {
        qDebug() << "GHS cur = 0 [String for CUR is empty]";
        plotValue.value[value_cur] = 0;
        miner.hashrate_cur = QString("0");
        return addToPlot(isPlot, plotValue);
    }
    bool ok_Diff1_Work, ok_Difficulty_Accepted, ok_Difficulty_Rejected, ok_Difficulty_Stale, ok_Elapsed;
    double val_Diff1_Work = Diff1_Work.toDouble(&ok_Diff1_Work);
    double val_Difficulty_Accepted = Difficulty_Accepted.toDouble(&ok_Difficulty_Accepted);
    double val_Difficulty_Rejected = Difficulty_Rejected.toDouble(&ok_Difficulty_Rejected);
    double val_Difficulty_Stale = Difficulty_Stale.toDouble(&ok_Difficulty_Stale);
    double val_Elapsed = Elapsed.toDouble(&ok_Elapsed);
    if (!(ok_Diff1_Work && ok_Difficulty_Accepted && ok_Difficulty_Rejected && ok_Difficulty_Stale && ok_Elapsed)) {
        qDebug() << "GHS cur = 0 [Convert vale for CUR failed]";
        plotValue.value[value_cur] = 0;
        miner.hashrate_cur = QString("0");
        return addToPlot(isPlot, plotValue);
    }
    if (((val_Difficulty_Accepted + val_Difficulty_Rejected + val_Difficulty_Stale) == 0) || (val_Elapsed == 0)) {
        qDebug() << "GHS cur = 0 [Divided by ZERO]";
        plotValue.value[value_cur] = 0;
        miner.hashrate_cur = QString("0");
        return addToPlot(isPlot, plotValue);
    }
    value = val_Diff1_Work / (val_Difficulty_Accepted / (val_Difficulty_Accepted + val_Difficulty_Rejected + val_Difficulty_Stale)) * 60 / val_Elapsed * 71582788 / 1000000 / 1000.0;
    plotValue.value[value_cur] = value;
    miner.hashrate_cur = QString::number(plotValue.value[value_cur], 'f', 3);
//    qDebug() << "GHS cur = " + miner.hashrate_cur;
    return addToPlot(isPlot, plotValue);
}

void RpcThread::parsePools()
{
    for (int i = 0; i < 3; i ++) {

        mutex.lock();
        miner.poolUrl[i] = getBfgValue(QString("POOL=%1").arg(QString::number(i)), "URL");
        miner.active[i] = getBfgValue(QString("POOL=%1").arg(QString::number(i)), "Stratum Active");
        QString Last_Share_Time = getBfgValue(QString("POOL=%1").arg(QString::number(i)), "Last Share Time");
//        QString Last_Share_Time = getBfgValue(QString("STATUS=S"), "When");  /* For Debug Only */
        mutex.unlock();
        bool ok;
        uint value = Last_Share_Time.toUInt(&ok);
        if (ok && (value != 0)) {
            miner.lastCommit[i] = QDateTime::fromTime_t(value).toString("HH:mm:ss");
        } else {
            qDebug() << "Last Commit =  [convert time failed]";
            miner.lastCommit[i] = QString("");
        }

//        qDebug() << "url" + QString::number(i) + ":" + miner.poolUrl[i];
//        qDebug() << "active" + QString::number(i) + ":" + miner.active[i];
//        qDebug() << "lastCommit" + QString::number(i) + ":" + miner.lastCommit[i];
    }
}
