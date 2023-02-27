#	EXIT CODES:
#	0: No Error
#	1: Parameter Error
#	2: Compilation Error
#	3: RODOS not found
#	127: Unkown Error

PARAM1="A"
ORIGINAL_DIR=$(pwd)

#assign Parameters to local variables for future use
if [ "$#" = 1 ];then
	PARAM1=$1
else
	echo "Only one parameter at a time is permitted, use the parameter '-help' for more information"
	exit 1
fi

#defines the funtion for compiling and flashing the code onto the STM board, also does lots of error handling
function executeFunction {

	#i would like to use this to check if RODOS has been sourced, but it doesn't work because it is an alias :(
	#cdrodos 2> /dev/null || COMPILER_NOT_EXISTS=true


	echo "Compiling code..."
	rodos-executable.sh discovery $PARAM1 2> /dev/null || COMPILER_NOT_FOUND=true


	if [ "$COMPILER_NOT_FOUND" = true ]; then
    	echo "Compiler not found, trying to source RODOS..."
		cd rodos
		source setenvs.sh 1> /dev/null 	2> /dev/null || RODOS_NOT_FOUND=true
		cd $ORIGINAL_DIR

		if [ "$RODOS_NOT_FOUND" = true ];then
			echo "RODOS was not found, aborting..."
			exit 3
		else
			echo "RODOS successfully sourced."
			echo "Retrying compilation..."
			COMPILER_NOT_FOUND=false
			rodos-executable.sh discovery $PARAM1 2> /dev/null || COMPILER_NOT_FOUND=true

			if [ "$COMPILER_NOT_FOUND" = true ]; then
				echo "Unknown error"
				exit 127
			fi
		fi
	fi

	CHECK_FOR_FILE=$(ls -1)
	if [[ $CHECK_FOR_FILE != *"tst"* ]]; then
  		echo "Compilation error, check compiler output for more information"
		exit 2
	fi


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
	echo "1. Make sure RODOS is in its correct location"
	echo "2. Make sure that RODOS is build for the STM32F4 Discovery Board"
	echo -e "3. Make sure RODOS is sourced by going into the RODOS directory and running 'source setenvs.sh'. \n"

	echo -e "Note: \nStep 3 is not necessary, as the script will try and source RODOS itself if it doesnt find the compiler. \nHowever, sourcing RODOS once manually is far more efficient. \n"

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