#include "rodos.h"

#include "../include/topics.h"

CommBuffer<ANGLES> Angles_Buffer;

Subscriber Angles_Subscriber(Angles_Topic, Angles_Buffer);

class Controller : public StaticThread <> {
    private:

        ANGLES angles_data;
        CONTROLLERDATA controller_data;

        float PID[3] = {10.0, 0.0, 0.0};

    public:
        Controller() : StaticThread("Controller", 100) {}

        void init(){

        }

        void run(){
            TIME_LOOP(0*SECONDS, 200*MILLISECONDS){
                Angles_Buffer.get(angles_data);

                controller_data.xValue = angles_data.xAngle * PID[0];
                controller_data.yValue = angles_data.yAngle * PID[0];

                Controller_Topic.publish(controller_data);
            }
        }
} controller;