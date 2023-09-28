#include "rodos.h"
#include "../rodos_src/include/util.h"

#include <thread>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <signal.h>

#include "FileWriter.h"
#include "DataPuffer.h"
#include "ImageProcessing.h"
#include "UARTInterface.h"
#include "PID_Controller.h"
#include "Weighted_median_filter.h"
#include "Linear_regression_filter.h"
#include "KF.h"
#include "DemoProgram.h"

#include "Pass_filter.h"
#include "P_Controller.h"

#define MessageLength 29 //28

int accepted_messages = 0;
int all_messages_in = 0;
int all_messages_out = 0;

namespace RODOS {
	extern HAL_UART uart_stdout(UART_IDX4);
}

FileWriter filewriter;

void trapCTRLC(sig_atomic_t s){
	filewriter.handleCTRLC();
	exit(128);
}

class LinuxMain: public StaticThread<>{

    public:
        LinuxMain() : StaticThread("Linux Main Thread", 1000) {}

        void init(){

        }

        void run(){

            bool is_demo = true;

            Kalman_Filter filterX(Identifier::X);
            Kalman_Filter filterY(Identifier::Y);

            PID_Controller controllerX(&filewriter, &filterX, Identifier::X, is_demo);
            PID_Controller controllerY(&filewriter, &filterY, Identifier::Y, is_demo);

            //trapping CTRL-C so that Filewriter can do its thing
            signal(SIGINT, trapCTRLC);

            //UARTInterface uartinterface(&filewriter, &controllerX, &controllerY, deviceName, 115200);

            ImageProcessing x(6, "X", &filewriter, &controllerX, Identifier::X);
            ImageProcessing y(2, "Y", &filewriter, &controllerY, Identifier::Y);

            //Creation of required threads
            std::thread xThread(&ImageProcessing::run, x);
            std::thread yThread(&ImageProcessing::run, y);
            //std::thread uartThread(&UARTInterface::run, uartinterface);

            //join all required threads
            xThread.join();
            yThread.join();
            //uartThread.join();

            suspendCallerUntil(NOW() + 3 * SECONDS);

            if(is_demo) {
                DemoProgram demoprogram(&filewriter);

                std::thread demoThread(&DemoProgram::run, demoprogram);
                is_demo = true;
                demoThread.join();
            }

            suspendCallerUntil(EOF);
        }

} linuxmain;

class UartReceiver: public StaticThread<>{

    protected:
        float xForce = 0;
		float yForce = 0;
		float zForce = 0;

		int16_t xLidarRaw = 0;
		int16_t yLidarRaw = 0;

		int16_t xLidarFiltered = 0;
		int16_t yLidarFiltered = 0;

        float AnglesForce[3] = {0.0, 0.0, 0.0};
        float AnglesAngular[3] = {0.0, 0.0, 0.0};
        float AnglesMagnetic[3] = {0.0, 0.0, 0.0};

        char all[MessageLength];
        int pos = 0;

        char response[MessageLength];

	public:
		UartReceiver() :	StaticThread("UART Reciever", 1000) {}

		void clearMessage(){
			for(int i = 0; i < MessageLength; i++){
				all[i] = 'z';
			}
		}

		void init(){
			uart_stdout.init();
			uart_stdout.config(UART_PARAMETER_ENABLE_DMA,MessageLength);
			clearMessage();

            signal(SIGINT, trapCTRLC);  
		}

        void run() {
			
			while (1) {
				size_t readLength = uart_stdout.read(all,1);
                //PRINTF("%d: %s\n", readLength, all);
                response[pos] = all[0];
                pos++;
                if(all[0] == '#'){
                    PRINTF("all: %s\n", response);
                    pos = 0;

                    accepted_messages++;
                    all_messages_in++;

                    char xResponse[8] = "";
                    char yResponse[8] = "";
                    char zResponse[8] = "";

                    for(int i = 0;i<8;i++) xResponse[i]=response[i+2];
                    for(int i = 0;i<8;i++) yResponse[i]=response[i+11];
                    for(int i = 0;i<8;i++) zResponse[i]=response[i+20];
                    
                    switch (response[0])
                    {
                        case '0':
                            xForce = toFloat(xResponse);
                            yForce = toFloat(yResponse);
                            //PRINTF("FORCE: %f %f\n", xForce, yForce);
                            break;

                        case '1':
                            xLidarFiltered = toUINT16(xResponse);
                            yLidarFiltered = toUINT16(yResponse);
                            break;

                        case '2':
                            xLidarRaw = toUINT16(xResponse);
                            yLidarRaw = toUINT16(yResponse);
                            //PRINTF("LIDAR: %d %d\n", xLidarRaw, yLidarRaw);
                            break;

                        case '3':
                            //TODO
                            break;
                        
                        case '4':
                            //TODO
                            break;

                        case '5':
                            AnglesForce[0] = toFloat(xResponse);
                            AnglesForce[1] = toFloat(yResponse);
                            AnglesForce[2] = toFloat(zResponse);
                            //TODO Jo mach was damit
                            break;
                        
                        case '6':
                            AnglesAngular[0] = toFloat(xResponse);
                            AnglesAngular[1] = toFloat(yResponse);
                            AnglesAngular[2] = toFloat(zResponse);
                            //TODO Jo mach was damit
                            break;

                        case '7':
                            AnglesMagnetic[0] = toFloat(xResponse);
                            AnglesMagnetic[1] = toFloat(yResponse);
                            AnglesMagnetic[2] = toFloat(zResponse);
                            //TODO Jo mach was damit
                            break;

                        default:
                            break;
                    }

                    filewriter.writeUARTInfo(xForce, yForce, xLidarFiltered, yLidarFiltered, xLidarRaw, yLidarRaw);
                }

                if(pos >= MessageLength) {
                    PRINTF("DROPPED: %s\n", response);
                    pos = 0;
                    all_messages_in++;
                }

                filewriter.writeUARTMessageInfo(all_messages_in, accepted_messages, all_messages_out);

                uart_stdout.suspendUntilDataReady();
            }
        }
} receiver;

class UartTransmitter : public StaticThread <> {

    protected:
        float xValue = 0; // currently changed to float, was double
		float yValue = 0; // maybe float is enough accuarcy?

        char * outputString;
	    char xString[9] = ""; 
	    char yString[9] = "";

    public:
        UartTransmitter() : StaticThread("UART Transmitter", 1000) {}

        void init(){

        }

        void run(){
            while(1){
                xValue = filewriter.readControllerValues(Identifier::X);
	            yValue = filewriter.readControllerValues(Identifier::Y);

                toChars(xValue, xString);
	            toChars(yValue, yString);

	            asprintf(&outputString, "X%sY%s#", xString, yString);

                uart_stdout.write(outputString, 19);
                all_messages_out++;

                filewriter.writeUARTMessageInfo(all_messages_in, accepted_messages, all_messages_out);

                suspendCallerUntil(NOW() + 5*MILLISECONDS);
            }
        }

} transmitter;