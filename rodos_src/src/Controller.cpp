#include "rodos.h"

#include "../include/topics.h"

CommBuffer<ANGLES> Angles_Buffer;

Subscriber Angles_Subscriber(Angles_Topic, Angles_Buffer);

class Controller : public StaticThread <> {
    private:

        ANGLES angles_data;
        ANGLES last_data;
        CONTROLLERDATA controller_data;

        float PID[3] = {0.1, 0.0, 0.001};

    public:
        Controller() : StaticThread("Controller", 100) {}

        void init(){

        }

        void run(){
            TIME_LOOP(0*SECONDS, 5*MILLISECONDS){
                last_data = angles_data;
                Angles_Buffer.get(angles_data);

                controller_data.xValue = angles_data.xAngle * PID[0] + (last_data.xAngle-angles_data.xAngle) * PID[2];
                controller_data.yValue = angles_data.yAngle * PID[0] + (last_data.yAngle-angles_data.yAngle) * PID[2];

                Controller_Topic.publish(controller_data);
            }
        }
} controller;