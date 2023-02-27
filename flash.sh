#	EXIT CODES:
#	0: No Error
#	1: Parameter Error
#	2: Compilation Error
#	3: RODOS not found
#	127: Unkown Error

PARAM1="A"
ORIGINAL_DIR=$(pwd)

#defines the funtion for compiling and flashing the code onto the STM board, also does lots of error handling
function executeFunction {

	cd rodos_src

	echo "Compiling code..."
	rodos-executable.sh discovery *.cpp 2> CompilationLog 


	if grep -q "command not found" "CompilationLog" ; then
    	echo "Compiler not found, trying to source RODOS..."
		cd ../rodos
		source setenvs.sh 1> /dev/null 	2> /dev/null || RODOS_NOT_FOUND=true #this should work, be careful though
		cd $ORIGINAL_DIR/rodos_src

		if [ "$RODOS_NOT_FOUND" = true ];then
			echo "RODOS was not found, aborting..."
			exit 3
		else
			echo "RODOS successfully sourced."
			echo "Retrying compilation..."
			COMPILER_NOT_FOUND=false
			rodos-executable.sh discovery *.cpp 2> /dev/null || COMPILER_NOT_FOUND=true

			if [ "$COMPILER_NOT_FOUND" = true ]; then
				echo "Unknown error, check CompilationLog for more information"
				exit 127
			fi
		fi
	fi

	CHECK_FOR_FILE=$(ls -1)
	if [[ $CHECK_FOR_FILE != *"tst"* ]]; then
  		echo "Compilation error, check CompilationLog for more information"
		exit 2
	fi


	echo "Creating binary executable..."
	arm-none-eabi-objcopy -S -O binary tst myExe.bin

	echo "Flashing board..."
	cp myExe.bin /media/$USER/DIS_F407VG

	echo "Removing temporary files..."
	rm myExe.bin
	rm tst
	rm CompilationLog
	cd .. #also return to original directory

	exit 0
}

#defines the help function
function helpFunction {

	echo "Prerequisits:"
	echo "1. Make sure RODOS is in its correct location"
	echo "2. Make sure that RODOS is build for the STM32F4 Discovery Board"
	echo "3. Move all files you want to compile and flash into the 'rodos_src' directory"
	echo -e "4. Make sure RODOS is sourced by going into the RODOS directory and running 'source setenvs.sh'. \n"

	echo -e "Note: \nStep 4 is not necessary, as the script will try and source RODOS itself if it doesnt find the compiler. \nHowever, sourcing RODOS once manually is far more efficient. \n"

	echo "How to use:"
	echo "Run './falsh.sh' to flash all .cpp files in the directory 'rodos_src' onto the Discovery Board."
	echo "If you have certain files you don't want to compile and flash, move them into the 'ignore' directory"

	exit 0
}

#decide which method to execute based on entered parameter
if [ "$#" = 0 ];then
	executeFunction
elif [ "$#" = 1 ];then
	if [ "$1" = "-help" ];then
		helpFunction
	else
		echo "If ./flash.sh is used with a parameter, only '-help' is permitted. Use this parameter for more information"
		exit 1
	fi
else
	echo "No parameters are permitted appart from '-help', use this parameter for more information"
	exit 1
fi