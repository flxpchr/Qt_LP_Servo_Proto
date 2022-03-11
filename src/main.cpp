#include "UserInterface.h"

#include <QApplication>

//Basic includes
#include<iostream>
#include<string>
#include<stdlib.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <algorithm>

//Arduino files
#include"SerialPort.h"
#include "PC2Arduino.h"

//File to have rand() function
#include <cstdlib>

//Falcon files
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "dhdc.h"
#include "drdc.h"

// change the name of the port with the port name of your computer
// must remember that the backslashes are essential so do not remove them

#define SERVO_MAX 150
#define SERVO_MIN 0

char* port = (char*)"\\\\.\\COM4";


void wait_to_keep_freq(std::chrono::steady_clock::time_point t_start, float freq, bool perfect = false);

void wait_to_keep_freq(std::chrono::steady_clock::time_point t_start, float freq, bool perfect) {
    int sampleTime = static_cast<int>(1.0f / freq * 1e6f);
    bool sleep = true;
    while (sleep) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - t_start).count();
        if (perfect) {
            if (elapsed > sampleTime)
                sleep = false;
        }
        else {
            std::this_thread::sleep_for(std::chrono::microseconds(sampleTime - elapsed));
            sleep = false;
        }
    }
}


#define REFRESH_INTERVAL  0.01   // sec


//Function to format data to send into a string representing an integer between SERVO_MAX and SERVO_MIN
std::string computeServoPosition(double position) {
    int clampPosition = std::clamp(int(position), SERVO_MIN, SERVO_MAX);
    return std::to_string(abs(SERVO_MAX - clampPosition));
}


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    UserInterface w;

    std::cout << "app started" << std::endl;

    //MUTEX AND THREADS
    bool threadCondition = true;
    std::mutex m_arduino, m_falcon;
    std::condition_variable cond_var_arduino, cond_var_falcon;

    PC2Arduino arduino(port);


    //falcon control

    double px, py, pz;
    double fx, fy, fz;
    double freq = 0.0;
    double t1, t0 = dhdGetTime();
    int    done = 0;

    std::string data;

    // center of workspace
    double nullPose[DHD_MAX_DOF] = { 0.0, 0.0, 0.0,  // base  (translations)
                                     0.0, 0.0, 0.0,  // wrist (rotations)
                                     0.0 };          // gripper

    printf("Falcon Initialisation \n");

    // open the first available device
    if (drdOpen() < 0) {
        printf("error: cannot open device (%s)\n", dhdErrorGetLastStr());
        dhdSleep(2.0);
        return -1;
    }

    // print out device identifier
    if (!drdIsSupported()) {
        printf("unsupported device\n");
        printf("exiting...\n");
        dhdSleep(2.0);
        drdClose();
        return -1;
    }
    printf("%s haptic device detected\n\n", dhdGetSystemName());

    // perform auto-initialization
    if (!drdIsInitialized() && drdAutoInit() < 0) {
        printf("error: auto-initialization failed (%s)\n", dhdErrorGetLastStr());
        dhdSleep(2.0);
        return -1;
    }
    else if (drdStart() < 0) {
        printf("error: regulation thread failed to start (%s)\n", dhdErrorGetLastStr());
        dhdSleep(2.0);
        return -1;
    }

    // move to center
    drdMoveTo(nullPose);

    // stop regulation thread (but leaves forces on)
    drdStop(true);

    std::thread falconThread([&]() {
        // haptic loop
        while (!done) {
            // apply zero force
            if (dhdSetForceAndTorqueAndGripperForce(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0) < DHD_NO_ERROR) {
                printf("error: cannot set force (%s)\n", dhdErrorGetLastStr());
                done = 1;
            }

            // display refresh rate and position at 10Hz
            //t1 = dhdGetTime();
            //if ((t1 - t0) > REFRESH_INTERVAL) {

                // retrieve information to display
            freq = dhdGetComFreq();
            t0 = t1;

            // write down position
            if (dhdGetPosition(&px, &py, &pz) < 0) {
                printf("error: cannot read position (%s)\n", dhdErrorGetLastStr());
                done = 1;
            }
            if (dhdGetForce(&fx, &fy, &fz) < 0) {
                printf("error: cannot read force (%s)\n", dhdErrorGetLastStr());
                done = 1;
            }
            printf("p (%+0.03f %+0.03f %+0.03f) m  |  f (%+0.01f %+0.01f %+0.01f) N  |  freq [%0.02f kHz]       \r", abs(px*10000), py, pz, fx, fy, fz, freq);

            std::unique_lock<std::mutex> lock(m_falcon);
            data = computeServoPosition(abs(px * 10000));
            lock.unlock();
            cond_var_falcon.notify_all();
            //}
        }
        });





    //    w.show();
    std::thread falcon2Arduino([&]() {
        std::cout << "Arduino Thread Started : " << threadCondition << std::endl << std::endl;
        while (threadCondition) {
            std::chrono::steady_clock::time_point tStart = std::chrono::steady_clock::now();

            std::unique_lock<std::mutex> lockArduino(m_arduino);
            arduino.sendData(data);
            lockArduino.unlock();
            cond_var_arduino.notify_all();
            //Send data to Arduino

            wait_to_keep_freq(tStart, 1000);
        }
        });

    //    threadCondition = false;
    falcon2Arduino.join();
    falconThread.join();

    return a.exec();
}

