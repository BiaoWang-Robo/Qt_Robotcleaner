#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <iostream>
extern "C" {
#include "extApi.h"
}
#include"vrepcontrol.h"

using namespace std;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , inputPort(0)
{
    ui->setupUi(this);
    mSim = new vrepControl();
    mSim->leftWheel.targetVel = 0;
    mSim->rightWheel.targetVel = 0;
    connect(mSim, &vrepControl::collisionsignal, this, &MainWindow::collisionShow);
    connect(mSim, &vrepControl::cliffsignal, this, &MainWindow::cliffShow);
    connect(mSim, &vrepControl::distancesignal, this, &MainWindow::distanceShow);
    connect(mSim, &vrepControl::lasersignal, this, &MainWindow::laserShow);
    connect(mSim, &vrepControl::imusignal, this, &MainWindow::imuShow);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonconnect_clicked()
{
    if(inputPort != 0){
        mSim->setPort(inputPort);
        mSim->robotIndex = inputPort - 20021;
        ui->widgetpaint->posIndex = mSim->robotIndex + 1;
        if(mSim->vrepConnect() == 1){
            vrepControl::threadFlag[ui->widgetpaint->posIndex] = 0;
            mSim->setLeftWheelHandle(("CleanRobotWheelLeftMotor#"+to_string(0)).c_str());
            mSim->setRightWheelHandle(("CleanRobotWheelRightMotor#"+to_string(mSim->robotIndex)).c_str());

            mSim->startThread();
        }
    }else {
        mSim->robotIndex = -1;
        ui->widgetpaint->posIndex = 0;
        if(mSim->vrepConnect() == 1){
            vrepControl::threadFlag[ui->widgetpaint->posIndex] = 0;
            mSim->setLeftWheelHandle("CleanRobotWheelLeftMotor");
            mSim->setRightWheelHandle("CleanRobotWheelRightMotor");

            mSim->startThread();
        }
    }
}

void MainWindow::on_pushButtonbreak_clicked()
{
    vrepControl::threadFlag[ui->widgetpaint->posIndex] = 1;
    mSim->breakConnection();
}

void MainWindow::on_pushButtonmove_clicked()
{
   mSim->move();
}


void MainWindow::on_pushButtonstop_clicked()
{
    mSim->stop();
}


void MainWindow::on_verticalSliderLeftMotor_sliderMoved(int vel)
{
   mSim->setLeftWheelVel(-vel);
}

void MainWindow::on_verticalSliderRightMotor_sliderMoved(int vel)
{
   mSim->setRightWheelVel(-vel);
}

void MainWindow::collisionShow(int collision)
{
    if (collision == 1)
        ui->labelfront->setStyleSheet("QLabel{color:red;}");
    else
        ui->labelfront->setStyleSheet("QLabel{color:black;}");

    if (collision == 2)
        ui->labelright->setStyleSheet("QLabel{color:red;}");
    else
        ui->labelright->setStyleSheet("QLabel{color:black;}");

    if (collision == 3)
        ui->labelleft->setStyleSheet("QLabel{color:red;}");
    else
        ui->labelleft->setStyleSheet("QLabel{color:black;}");
}

void MainWindow::cliffShow(int cliff)
{
    if (cliff == 1)
        ui->labelcliff->setStyleSheet("QLabel{color:red;}");
    else
        ui->labelcliff->setStyleSheet("QLabel{color:black;}");
}

void MainWindow::distanceShow(float distance)
{
    QString ss = "";
    if(distance >= 0 ){
        ss = QString("%1").arg(distance);
    }else {
        ss = "NULL";
    }
    ui->textBrowserdistance->setPlainText(ss);
}

void MainWindow::laserShow()
{
    if(vrepControl::paintFlag[ui->widgetpaint->posIndex] == 0){
        update();
        vrepControl::paintFlag[ui->widgetpaint->posIndex] = 1;
    }
}

void MainWindow::imuShow(float accX, float accY, float accZ, float angVarX, float angVarY, float angVarZ)
{
    QString ss = "";
    ss = QString("%1").arg(accX);
    ui->textBrowserAccelX->setPlainText(ss);
    ss = QString("%1").arg(accY);
    ui->textBrowserAccelY->setPlainText(ss);
    ss = QString("%1").arg(accZ);
    ui->textBrowserAccelZ->setPlainText(ss);
    ss = QString("%1").arg(angVarX);
    ui->textBrowserAngVarX->setPlainText(ss);
    ss = QString("%1").arg(angVarY);
    ui->textBrowserAngVarY->setPlainText(ss);
    ss = QString("%1").arg(angVarZ);
    ui->textBrowserAngVarZ->setPlainText(ss);
}




void MainWindow::on_pushButtonload_clicked()
{
    mSim->loadModel();
}


void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    inputPort = arg1.toInt();
    qDebug() << inputPort;
}
