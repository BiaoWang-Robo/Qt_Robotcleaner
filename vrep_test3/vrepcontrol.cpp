#include "vrepcontrol.h"
#include "extApi.h"
#include <iostream>
#include <QDebug>
#include <Windows.h>

using namespace std;

int vrepControl::threadFlag[ROBOT_NUMBER] = {0};
int vrepControl::paintFlag[ROBOT_NUMBER] = {0};
float vrepControl::position[ROBOT_NUMBER][360][2] = {0};

vrepControl::vrepControl()
{
    moveflag            = false;
    connectflag         = true;
    processThread       = NULL;
    detectThread        = NULL;
    signalBuffer[VREP_SIGNAL_LENGTH]   = {0};
}

void vrepControl::collisionemit(int collision)
{
    emit collisionsignal(collision);
}

void vrepControl::cliffemit(int cliff)
{
    emit cliffsignal(cliff);
}

void vrepControl::distanceemit(float dis)
{
    emit distancesignal(dis);
}

void vrepControl::laseremit()
{
    emit lasersignal();
}

void vrepControl::imuemit(float xAccel, float yAccel, float zAccel, float angularVarX, float angularVarY, float angularVarZ)
{
    emit imusignal(xAccel, yAccel, zAccel, angularVarX, angularVarY, angularVarZ);
}

void vrepControl::loadModel()
{
    qDebug() << "trying to load";
    simxInt temp = 1;
    simxInt* loadModelHandle = &temp;
    if(simxLoadModel(clientID, "C:\Program Files\V-REP3\V-REP_PRO_EDU\models\myrobot\LDSweeprRobot1.ttm", 1, loadModelHandle, simx_opmode_blocking)==simx_return_ok)
        qDebug() << "load model successfully!";
    else
        qDebug() << "fail to load model";
    qDebug() << "finish the loading";
}

void vrepControl::setPort(int input)
{
    port = input;
}

void vrepControl::setLeftWheelHandle(const char *name)
{
    if(simxGetObjectHandle(clientID, name, &leftWheel.handle, simx_opmode_blocking)==simx_return_ok)
        qDebug() << "get the handle of" << name << "successfully!";
}

void vrepControl::setRightWheelHandle(const char *name)
{
    if(simxGetObjectHandle(clientID, name, &rightWheel.handle, simx_opmode_blocking)==simx_return_ok)
        qDebug() << "get the handle of" << name << "successfully!";
}

void vrepControl::setLeftWheelVel(float vel)
{
    leftWheel.targetVel = vel;
    if(moveflag){
        vrepmtx.lock();
//        simxSetJointTargetVelocity(clientID, leftWheel.handle, leftWheel.targetVel, simx_opmode_blocking);
        simxSetJointTargetVelocity(clientID, leftWheel.handle, leftWheel.targetVel, simx_opmode_oneshot);
        vrepmtx.unlock();
    }
}

void vrepControl::setRightWheelVel(float vel)
{
    rightWheel.targetVel = vel;
    if(moveflag){
        vrepmtx.lock();
//        simxSetJointTargetVelocity(clientID, rightWheel.handle, rightWheel.targetVel, simx_opmode_blocking);
        simxSetJointTargetVelocity(clientID, rightWheel.handle, rightWheel.targetVel, simx_opmode_oneshot);
        vrepmtx.unlock();
    }
}

void vrepControl::detectSignal()
{
    vrepmtx.lock();
    simxGetStringSignal(clientID, ("laser2d ranges"+to_string(robotIndex)).c_str(), &ssv, &length, simx_opmode_blocking);
    vrepmtx.unlock();
}


int vrepControl::vrepConnect()
{
    if(connectflag){
        vrepmtx.lock();
        clientID=simxStart("127.0.0.1", port, true, true, 2000, 5);
        vrepmtx.unlock();
        if(clientID != -1){
            qDebug() << "v_rep connect!";
        }else
            qDebug() << "fail to connect.";
        connectflag = false;
        return 1;
    }
    return 0;
}

void vrepControl::breakConnection()
{
    if(!connectflag){
        vrepmtx.lock();
        simxFinish(clientID);
        vrepmtx.unlock();
        connectflag = true;
        moveflag = false;
        delete detectThread;
        detectThread = NULL;
        delete processThread;
        processThread = NULL;
    }
}

void vrepControl::move()
{
    if(!moveflag){
        moveflag = true;
        vrepmtx.lock();
        simxSetJointTargetVelocity(clientID, leftWheel.handle, leftWheel.targetVel, simx_opmode_oneshot);
        simxSetJointTargetVelocity(clientID, rightWheel.handle, rightWheel.targetVel, simx_opmode_oneshot);
        vrepmtx.unlock();
        qDebug() << "move";
    }
}

void vrepControl::stop()
{
    if(moveflag){
        moveflag = false;
        vrepmtx.lock();
        simxSetJointTargetVelocity(clientID, leftWheel.handle, 0, simx_opmode_oneshot);
        simxSetJointTargetVelocity(clientID, rightWheel.handle, 0, simx_opmode_oneshot);
        vrepmtx.unlock();
        qDebug() << "stop";
    }
}

void vrepControl::startThread()
{
    if(detectThread  == NULL){
        detectThread  = new std::thread(std::mem_fn(&vrepControl::vrepDetectThread), this);
        detectThread->detach();
    }
    if(processThread == NULL){
        processThread = new std::thread(std::mem_fn(&vrepControl::vrepProcessThread), this);
        processThread->detach();
    }
}

void vrepControl::vrepDetectThread()
{
    while(true){
        if(threadFlag[robotIndex+1] == 1)
            break;

        detectSignal();

        buffermtx.lock();
        memcpy(signalBuffer, ssv, VREP_SIGNAL_LENGTH*4);
        buffermtx.unlock();

        extApi_sleepMs(25);
    }
}

void vrepControl::vrepProcessThread()
{
    LARGE_INTEGER t1,t2,t3,tc;
    float vrepSignal[VREP_SIGNAL_LENGTH]={0};
    QueryPerformanceFrequency(&tc);
    qDebug() << "start the thread";
    while(true){
        if(threadFlag[robotIndex+1] == 1)
            break;
        if(vrepControl::paintFlag[robotIndex+1] == 0){
            QueryPerformanceCounter(&t1);
            double operation_time=(double)(t1.QuadPart-t2.QuadPart)/(double)tc.QuadPart;
            if(operation_time < 0.125 && operation_time > 0){
                double delay_time = 0.125 - operation_time;
                extApi_sleepMs(delay_time*1000);
            }
            QueryPerformanceCounter(&t2);
            double time2=(double)(t2.QuadPart-t3.QuadPart)/(double)tc.QuadPart;
            t3 = t2;
            if(1/time2 < 7.9)
                qDebug() << "paint frequency = " << 1/time2;

            buffermtx.lock();
            memcpy(vrepSignal, signalBuffer, VREP_SIGNAL_LENGTH*4);
            buffermtx.unlock();

            int degree = 0;
            for(int i = 0 ; i < 360 ; i++){
                position[robotIndex+1][i][0] = vrepSignal[i] * cos(M_PI*degree/180);
                position[robotIndex+1][i][1] = vrepSignal[i] * sin(M_PI*degree/180);
                degree++;
            }

            laseremit();
            collisionemit((int)vrepSignal[361]);
            cliffemit((int)vrepSignal[362]);
            distanceemit(vrepSignal[363]);
            imuemit(vrepSignal[364],vrepSignal[365],vrepSignal[366],vrepSignal[367],vrepSignal[368],vrepSignal[369]);
        }
    }
    qDebug() << "finish the thread";
}
