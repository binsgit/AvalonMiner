#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bfgconf.h"
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    processStatus = false;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setColumnWidth(0, 300);
    ui->tableWidget->setColumnWidth(1, 60);
    ui->tableWidget->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignCenter);

    plot = new MinerPlot(ui->widget_cur);
//    plot->setTitle("History");
    const int margin = 5;
    plot->setContentsMargins(margin, margin, margin, margin);
    QVBoxLayout *layout = new QVBoxLayout(ui->widget_cur);
    layout->addWidget(plot);

    connect(ui->pushButton_start, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(saveClicked()));
    connect(this, SIGNAL(addPlotValue(plotInfo)), plot, SIGNAL(addPlotValue(plotInfo)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete plot;
}

void MainWindow::runClicked()
{
    if (serialPara.isEmpty()) {
        QMessageBox::critical(this, tr("Avalon Miner"), tr("No Avalon Nano detected!\t\n"), QMessageBox::Ok);
    } else {
        emit runProcess(!processStatus, serialPara);
    }
}

void MainWindow::processStarted()
{
    qCritical() << "bfgminer process started.";
    processStatus = true;
    ui->label_status->setText("PREPARING");
    ui->graphicsView_status->setStyleSheet("background-image: url(:/images/workingbg.png)");
    ui->pushButton_start->setText("Stop Mining");
    ui->pushButton_save->setDisabled(true);
}

void MainWindow::processFinished()
{
    qCritical() << "bfgminer process has been finished.";
    processStatus = false;
    ui->label_status->setText("NOT MINING");
    ui->graphicsView_status->setStyleSheet("background-image: url(:/images/notworkingbg.png)");
    ui->pushButton_start->setText("Start Mining");
    ui->pushButton_save->setEnabled(true);

    ui->label_20s->setText("0 Gh/s");
    ui->label_av->setText("0 Gh/s");
    ui->label_cur->setText("0 Gh/s");
    for (int i = 0; i < 3; i ++) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(""));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(""));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(""));
    }

    plot->clearCurve();
}

void MainWindow::updateInfo(minerInfo miner)
{
    ui->label_20s->setText(miner.hashrate_20s + " Gh/s");
    ui->label_av->setText(miner.hashrate_av + " Gh/s");
    ui->label_cur->setText(miner.hashrate_cur + " Gh/s");

    for (int i = 0; i < 3; i ++) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(miner.poolUrl[i]));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(miner.active[i]));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(miner.lastCommit[i]));
        ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignCenter);
    }
}

void MainWindow::rpcConnected()
{
    ui->label_status->setText("MINING");
}

void MainWindow::saveClicked()
{
    bfgConf bfgConfig;

    bfgConfig.info[0].url = ui->lineEdit_url1->text();
    bfgConfig.info[0].port = ui->spinBox_port1->value();
    bfgConfig.info[0].name = ui->lineEdit_name1->text();
    bfgConfig.info[0].pass = ui->lineEdit_pass1->text();
    bfgConfig.info[1].url = ui->lineEdit_url2->text();
    bfgConfig.info[1].port = ui->spinBox_port2->value();
    bfgConfig.info[1].name = ui->lineEdit_name2->text();
    bfgConfig.info[1].pass = ui->lineEdit_pass2->text();
    bfgConfig.info[2].url = ui->lineEdit_url3->text();
    bfgConfig.info[2].port = ui->spinBox_port3->value();
    bfgConfig.info[2].name = ui->lineEdit_name3->text();
    bfgConfig.info[2].pass = ui->lineEdit_pass3->text();

    emit saveConf(&bfgConfig);
}

void MainWindow::confChanged(bfgConf *bfgConfig)
{
    ui->lineEdit_url1->setText(bfgConfig->info[0].url);
    ui->spinBox_port1->setValue(bfgConfig->info[0].port);
    ui->lineEdit_name1->setText(bfgConfig->info[0].name);
    ui->lineEdit_pass1->setText(bfgConfig->info[0].pass);
    ui->lineEdit_url2->setText(bfgConfig->info[1].url);
    ui->spinBox_port2->setValue(bfgConfig->info[1].port);
    ui->lineEdit_name2->setText(bfgConfig->info[1].name);
    ui->lineEdit_pass2->setText(bfgConfig->info[1].pass);
    ui->lineEdit_url3->setText(bfgConfig->info[2].url);
    ui->spinBox_port3->setValue(bfgConfig->info[2].port);
    ui->lineEdit_name3->setText(bfgConfig->info[2].name);
    ui->lineEdit_pass3->setText(bfgConfig->info[2].pass);
}

void MainWindow::confSaved()
{
    QMessageBox msgBox;
    msgBox.setText("Configuration has been saved.");
    msgBox.exec();
}

void MainWindow::serialReady(QStringList para)
{
    if ((!para.isEmpty()) && (serialPara == para))
        return;

    if (processStatus == true) {
        qCritical() << "Serial ports changed during bfgminer is running. Kill bfgminer now!";
        emit runProcess(!processStatus, serialPara);
    }

    serialPara = para;

    if (para.size() == 0) {
        qCritical() << "Serial port is empty, no Avalon Nano detected!";
        QMessageBox::critical(this, tr("Avalon Miner"), tr("No Avalon Nano detected!\t\n"), QMessageBox::Ok);
        return;
    } else {
        QTimer::singleShot(1000, this, SLOT(runClicked()));
    }
}
