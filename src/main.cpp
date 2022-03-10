#include "UserInterface.h"

#include <QApplication>

//Basic includes
#include<iostream>
#include<string>
#include<stdlib.h>
#include <thread>
#include <chrono>

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
char *port = (char*) "\\\\.\\COM4";


void wait_to_keep_freq(std::chrono::steady_clock::time_point t_start, float freq, bool perfect = false);

void wait_to_keep_freq(std::chrono::steady_clock::time_point t_start, float freq, bool perfect){
    int sampleTime = static_cast<int>(1.0f / freq * 1e6f);
    bool sleep = true;
    while(sleep){
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - t_start).count();
        if (perfect){
            if(elapsed > sampleTime)
                sleep = false;
        }else{
            std::this_thread::sleep_for(std::chrono::microseconds(sampleTime - elapsed));
            sleep = false;
        }
    }
}

//---------------------------------------------------------------------------------
//Temp functions to send to Arduino

//Temp coef
int k = 0;
std::string dataGenerator(){
    int a = rand()%125 + 910;
    std::cout<< a << std::endl;
    return std::to_string(a);
}

std::string roundTrip(){

    if (k%2){
        k++;
        return "910";

    }
    else {
        k++;
        return "1025";
    }
}
//---------------------------------------------------------------------------------


#define REFRESH_INTERVAL  0.01   // sec


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UserInterface w;
    std::cout<<"app started"<<std::endl;

    bool threadCondition = true;



    // EXIT button connection
    //    QObject::connect(&w, &UserInterface::close, [&threadCondition, &a](){threadCondition = false; a.quit();});

    PC2Arduino arduino(port);


    //falcon control

    double px, py, pz;
    double fx, fy, fz;
    double freq   = 0.0;
    double t1,t0  = dhdGetTime ();
    int    done   = 0;

    // center of workspace
    double nullPose[DHD_MAX_DOF] = { 0.0, 0.0, 0.0,  // base  (translations)
                                     0.0, 0.0, 0.0,  // wrist (rotations)
                                     0.0 };          // gripper

    // message
    printf ("Force Dimension - Auto Center Gravity %s\n", dhdGetSDKVersionStr());
    printf ("Copyright (C) 2001-2021 Force Dimension\n");
    printf ("All Rights Reserved.\n\n");

    // open the first available device
    if (drdOpen () < 0) {
        printf ("error: cannot open device (%s)\n", dhdErrorGetLastStr ());
        dhdSleep (2.0);
        return -1;
    }

    // print out device identifier
    if (!drdIsSupported ()) {
        printf ("unsupported device\n");
        printf ("exiting...\n");
        dhdSleep (2.0);
        drdClose ();
        return -1;
    }
    printf ("%s haptic device detected\n\n", dhdGetSystemName ());

    // perform auto-initialization
    if (!drdIsInitialized () && drdAutoInit () < 0) {
        printf ("error: auto-initialization failed (%s)\n", dhdErrorGetLastStr ());
        dhdSleep (2.0);
        return -1;
    }
    else if (drdStart () < 0) {
        printf ("error: regulation thread failed to start (%s)\n", dhdErrorGetLastStr ());
        dhdSleep (2.0);
        return -1;
    }

    // move to center
    drdMoveTo (nullPose);

    // stop regulation thread (but leaves forces on)
    drdStop (true);

    // display instructions
    printf ("press 'q' to quit\n");
    printf ("      'c' to re-center end-effector (all axis)\n");
    printf ("      'p' to re-center position only\n");
    printf ("      'r' to re-center rotation only\n");
    printf ("      'g' to close gripper only\n\n");

    std::thread falconThread([&](){
        // haptic loop
        while (!done) {
            // apply zero force
            if (dhdSetForceAndTorqueAndGripperForce (0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0) < DHD_NO_ERROR) {
                printf ("error: cannot set force (%s)\n", dhdErrorGetLastStr ());
                done = 1;
            }

            // display refresh rate and position at 10Hz
            t1 = dhdGetTime ();
            if ((t1-t0) > REFRESH_INTERVAL) {

                // retrieve information to display
                freq = dhdGetComFreq ();
                t0   = t1;

                // write down position
                if (dhdGetPosition (&px, &py, &pz) < 0) {
                    printf ("error: cannot read position (%s)\n", dhdErrorGetLastStr());
                    done = 1;
                }
                if (dhdGetForce (&fx, &fy, &fz) < 0) {
                    printf ("error: cannot read force (%s)\n", dhdErrorGetLastStr());
                    done = 1;
                }
                printf ("p (%+0.03f %+0.03f %+0.03f) m  |  f (%+0.01f %+0.01f %+0.01f) N  |  freq [%0.02f kHz]       \r", px, py, pz, fx, fy, fz, freq);

                // user input
                if (dhdKbHit ()) {
                    switch (dhdKbGet ()) {
                    case 'q': done = 1; break;
                    case 'c':
                        drdRegulatePos  (true);
                        drdRegulateRot  (true);
                        drdRegulateGrip (true);
                        drdStart();
                        drdMoveTo (nullPose);
                        drdStop(true);
                        break;
                    case 'p':
                        drdRegulatePos  (true);
                        drdRegulateRot  (false);
                        drdRegulateGrip (false);
                        drdStart();
                        drdMoveToPos (0.0, 0.0, 0.0);
                        drdStop(true);
                        break;
                    case 'r':
                        drdRegulatePos  (false);
                        drdRegulateRot  (true);
                        drdRegulateGrip (false);
                        drdStart();
                        drdMoveToRot (0.0, 0.0, 0.0);
                        drdStop(true);
                        break;
                    case 'g':
                        drdRegulatePos  (false);
                        drdRegulateRot  (false);
                        drdRegulateGrip (true);
                        drdStart();
                        drdMoveToGrip (0.0);
                        drdStop(true);
                        break;
                    }
                }
            }
        }

    });


//     close the connection
    printf ("cleaning up...                                                           \n");
    drdClose ();

    // happily exit
//    printf ("\ndone.\n");



    //    w.show();
    std::thread falcon2Arduino([&](){
        std::cout<<"Thread begun with condition :"<<threadCondition<<std::endl;
        while(threadCondition){
            std::chrono::steady_clock::time_point tStart = std::chrono::steady_clock::now();
            //Read data from the Falcon
            //            std::string data = dataGenerator();
            std::string data = roundTrip();
            //Process the data to an int between motor min & motor max for the servo
            //            arduino.formatData();
            //Send data to Arduino
            arduino.sendData(data);
            wait_to_keep_freq(tStart,15);
        }
    });

    //    threadCondition = false;
    falcon2Arduino.join();
    falconThread.join();

    return a.exec();
}

