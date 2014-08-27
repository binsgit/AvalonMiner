#include <QSettings>
#include <QRegExp>
#include <QDebug>
#include <QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "serialwatch.h"

/* Check Aavalon Nano Device (USB->Serial) Device every 1 second */
SerialWatch::SerialWatch(QObject *parent) :
    QObject(parent)
{
    firstWatch = true;
    connect(&timer, SIGNAL(timeout()), this, SLOT(watchPorts()));
}

SerialWatch::~SerialWatch()
{
}

/* HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\USB\\VID_????&PID_????*\\*\\Device Parameters\\PortName */
static QStringList regParse(QString regPath, QRegExp regExp)
{
    QStringList result;
    QSettings settings(regPath, QSettings::NativeFormat);
    QStringList groups = settings.childGroups();

    regExp.setCaseSensitivity(Qt::CaseInsensitive);
    for(QStringList::iterator it = groups.begin(); it != groups.end(); ++it) {
        if (regExp.exactMatch(*it)) {
            result.append(*it);
        }
    }

    return result;
}

static QStringList getRegPorts()
{
    QStringList regPorts;

    QStringList result1 = regParse("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\USB", QRegExp("^VID_1FC9&PID_0083(.*)"));
    //QStringList result1 = regParse("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\USB", QRegExp("^VID_04E8&PID_6866(.*)"));
    //QStringList result1 = regParse("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\USB", QRegExp("^VID_(\\w{4})&PID_(\\w{4,})(.*)"));
    for (int i = 0; i < result1.size(); i ++) {
        QStringList result2 = regParse(QString("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\USB\\" + result1.at(i)), QRegExp("(.*)"));
        for (int j = 0; j < result2.size(); j ++) {
            QStringList result3 = regParse(QString("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\USB\\" + result1.at(i) + "\\" + result2.at(j)), QRegExp("\\Device Parameters"));
            for (int k = 0; k < result3.size(); k ++) {
                QSettings settings(QString("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Enum\\USB\\" + result1.at(i) + "\\" + result2.at(j) + "\\Device Parameters"), QSettings::NativeFormat);
                if (!settings.value("PortName").toString().isEmpty()) {
                    regPorts.append(settings.value("PortName").toString());
                    //qDebug() << "======" + result1.at(i) + "    " + regPorts.last();
                }
            }
        }
    }

    return regPorts;
}

QStringList SerialWatch::getPorts()
{
    QStringList regPorts = getRegPorts();
    QStringList ports;

    mutex.lock();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        for (int i = 0; i < regPorts.size(); i ++) {
            if (regPorts.at(i) == info.portName())
                ports.append(info.portName());
        }
    }
    ports.sort();
    oldPorts = newPorts;
    mutex.unlock();

    return ports;
}

QStringList SerialWatch::getOldPorts()
{
    return oldPorts;
}

void SerialWatch::start()
{
    oldPorts.clear();
    newPorts.clear();
    firstWatch = true;
    watchPorts();
    timer.start(1000);
}

void SerialWatch::stop()
{
    timer.stop();
    oldPorts.clear();
    newPorts.clear();
    firstWatch = true;
}

void SerialWatch::watchPorts()
{
    newPorts = getPorts();
    /* First watch should emit signal always */
    if ((oldPorts != newPorts) || (firstWatch == true)) {
//        qDebug() << "Serial ports changed from: " << oldPorts << " to: " << newPorts;
        mutex.lock();
        oldPorts = newPorts;
        emit(changed(newPorts));
        mutex.unlock();
        firstWatch = false;
    }
}

