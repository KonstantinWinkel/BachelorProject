#include "rodos.h"

#include "../include/topics.h"


#define Speed 5

class Demo : public StaticThread <> {
    private:

        ANGLES angles_data;
        CONTROLLERDATA controller_data;

    public:
        Demo() : StaticThread("Demo", 100) {}

        void init(){

        }

        void run(){
            while(1){
                controller_data.yValue = Speed;
                controller_data.xValue = Speed;
                Controller_Topic.publish(controller_data);
                suspendCallerUntil(NOW() + 1*SECONDS);

                controller_data.yValue = -Speed;
                controller_data.xValue = -Speed;
                Controller_Topic.publish(controller_data);
                suspendCallerUntil(NOW() + 1*SECONDS);
            }
        }
} controller;