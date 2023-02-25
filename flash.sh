PARAM1="A"

#assign Parameters to local variables for future use
if [ "$#" = 1 ];then
	PARAM1=$1
else
	echo "Only one parameter at a time is permitted, use the parameter '-help' for more information"
	exit 1
fi

#defines the funtion for compiling and flashing the code onto the STM board
function executeFunction {
	echo "Compiling code..."
	rodos-executable.sh discovery $PARAM1 

	echo "Creating binary executable..."
	arm-none-eabi-objcopy -S -O binary tst myExe.bin

	echo "Flashing board..."
	cp myExe.bin /media/$USER/DIS_F407VG

	echo "Removing temporary files..."
	rm myExe.bin
	rm tst

	exit 0
}

#defines the help function
function helpFunction {
	echo "Prerequisits:"
	echo "1. Make sure this terminal knows where RODOS is by going into your RODOS directory and running the command 'source setenvs.sh' "
	echo "2. Make sure that RODOS is build for the STM32F4 Discovery Board"
	echo " "
	echo "How to use:"
	echo "Run './falsh.sh' followed by the cpp file you want to flash onto the Discovery Board."
	echo "Example: './flash.sh testFile.cpp' flashes the file 'testFile.cpp' onto the Board."
	echo "To flash all cpp files in the current directory onto the Board, use *.cpp."

	exit 0
}

#decide which method to execute based on entered parameter
if [ "$PARAM1" = "-help" ];then
	helpFunction
else
	executeFunction
fi