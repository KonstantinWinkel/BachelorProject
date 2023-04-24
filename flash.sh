#	Author: Konstantin Winkel

#	EXIT CODES:
#	0: No Error
#	1: Parameter Error
#	2: Compilation Error
#	3: RODOS not found
#	127: Unkown Error

ORIGINAL_DIR=$(pwd)
KEEP_COMPILATION_LOG=false
SHOW_COMPILATION_OUTPUT=false
COMPILE_ALL=true
FILES_TO_COMPILE="*.cpp"


function compileFiles {
	if [ "$SHOW_COMPILATION_OUTPUT" = true ]; then
		rodos-executable.sh discovery $FILES_TO_COMPILE 2>&1 | tee -a CompilationLog
	else
		rodos-executable.sh discovery $FILES_TO_COMPILE 2> CompilationLog
	fi
}

#defines the funtion for compiling and flashing the code onto the STM board, also does lots of error handling
function executeFunction {

	cd rodos_src

	echo "Compiling code..."
	compileFiles

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
			compileFiles || COMPILER_NOT_FOUND=true

			if [ "$COMPILER_NOT_FOUND" = true ]; then
				echo "Unknown error, check CompilationLog for more information"
				exit 127
			fi
		fi
	fi

	CHECK_FOR_FILE=$(ls -1)
	if [[ $CHECK_FOR_FILE != *"tst"* ]]; then
  		echo "Compilation error, check CompilationLog for more information"
		mv CompilationLog $ORIGINAL_DIR
		exit 2
	fi

	echo "Creating binary executable..."
	arm-none-eabi-objcopy -S -O binary tst myExe.bin

	echo "Flashing board..."
	cp myExe.bin /media/$USER/DIS_F407VG

	echo "Removing temporary files..."
	rm myExe.bin
	rm tst
	if [ "$KEEP_COMPILATION_LOG" = false ]; then
		rm CompilationLog
	else 
		mv CompilationLog $ORIGINAL_DIR
	fi
	cd .. #also return to original directory

	exit 0
}

#defines the help function
function helpFunction {

	echo "Prerequisits:"
	echo "1. Make sure RODOS is in its correct location"
	echo "2. Make sure that RODOS is build for the STM32F4 Discovery Board"
	echo "3. Move all files you want to compile and flash into the 'rodos_src' directory"
	echo "4. Make sure RODOS is sourced by going into the RODOS directory and running 'source setenvs.sh'"
	echo -e "\n"
	echo -e "Note: \nStep 4 is not necessary, as the script will try and source RODOS itself if it doesnt find the compiler. \nHowever, sourcing RODOS once manually is far more efficient. \n"

	echo "How to use:"
	echo "Run './falsh.sh' to flash all .cpp files in the directory 'rodos_src' onto the Discovery Board."
	echo "If you only want to flash certain files, for example test.cpp, add then name as a parameter: './flash.sh test.cpp'"
	echo -e "\n"
	echo "Parameters:"
	echo "	-help	shows this text explaination but does nothing else"
	echo "	-l	doesnt remove the log file after compilation"
	echo "	-s	shows the compiler output during compilation"

	exit 0
}

#decide which method to execute based on entered parameter
for var in "$@"
do
	if [ "$var" = -help ]; then
		helpFunction
		exit 0
	elif [ "$var" = "-l" ]; then # -l to keep the compilation log
		KEEP_COMPILATION_LOG=true
	elif [ "$var" = "-s" ]; then # -s to show compilation output in console and write to file
		SHOW_COMPILATION_OUTPUT=true
	elif [[ $var =~ .*cpp.* ]]; then
		if [ "$COMPILE_ALL" = true ]; then
			COMPILE_ALL=false
			FILES_TO_COMPILE=$var
		else
			FILES_TO_COMPILE="$FILES_TO_COMPILE $var"
		fi
	fi
done

executeFunction