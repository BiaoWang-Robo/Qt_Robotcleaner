#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <cmath>
#include"vrepcontrol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
//    static float  position[360][2];
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonconnect_clicked();

    void on_pushButtonbreak_clicked();

    void on_pushButtonmove_clicked();

    void on_pushButtonstop_clicked();

    void on_verticalSliderLeftMotor_sliderMoved(int position);

    void on_verticalSliderRightMotor_sliderMoved(int position);

    void collisionShow(int collision);

    void cliffShow(int cliff);

    void distanceShow(float distance);

    void laserShow();

    void imuShow(float accX, float accY, float accZ, float angVarX, float angVarY, float angVarZ);

    void on_pushButtonload_clicked();


    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    vrepControl *mSim;
    int inputPort;
};
#endif // MAINWINDOW_H
