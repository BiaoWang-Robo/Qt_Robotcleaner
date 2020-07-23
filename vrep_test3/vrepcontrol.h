#ifndef VREPCONTROL_H
#define VREPCONTROL_H

#define _USE_MATH_DEFINES

extern "C"{
#include "extApi.h"
}
#include <mutex>
#include<iostream>
#include<QObject>
#include<cmath>

#define VREP_SIGNAL_LENGTH 370
#define ROBOT_NUMBER 3

struct wheel
{
    int handle;
    float targetVel;
};

class vrepControl: public QObject
{
    Q_OBJECT
private:
    int             port = 20020;
    int             clientID;
    std::mutex      vrepmtx;
    std::mutex      buffermtx;
    bool            moveflag;
    bool            connectflag;

public:
    static int      paintFlag[ROBOT_NUMBER];
    static int      threadFlag[ROBOT_NUMBER];
    static float    position[ROBOT_NUMBER][360][2];
    float           signalBuffer[VREP_SIGNAL_LENGTH];
    simxUChar*      ssv;
    std::thread*    processThread;
    std::thread*    detectThread;
    int             length;
    wheel           leftWheel;
    wheel           rightWheel;
    int             robotIndex;

    void setLeftWheelHandle(const char* name);
    void setRightWheelHandle(const char* name);

    void setLeftWheelVel(float vel);
    void setRightWheelVel(float vel);

    void detectSignal();

    vrepControl();

    int vrepConnect();
    void breakConnection();

    void startThread();
    void vrepProcessThread();
    void vrepDetectThread();

    void move();
    void stop();

    void collisionemit(int collision);
    void cliffemit(int cliff);
    void distanceemit(float dis);
    void laseremit();
    void imuemit(float xAccel, float yAccel, float zAccel, float angularVarX, float angularVarY, float angularVarZ);

    void loadModel();
    void setPort(int input);

signals:
    void collisionsignal(int collision);
    void cliffsignal(int cliff);
    void distancesignal(float dis);
    void lasersignal();
    void imusignal(float xAccel, float yAccel, float zAccel, float angularVarX, float angularVarY, float angularVarZ);
};


#endif // VREPCONTROL_H
