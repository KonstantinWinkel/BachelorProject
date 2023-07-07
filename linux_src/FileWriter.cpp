//class header
#include "FileWriter.h"

//constructor, creates a new object
FileWriter::FileWriter(){
	starttime = std::chrono::high_resolution_clock::now();

	FileWriter::ControllerXValue = 0;
	FileWriter::ControllerYValue = 0;
}

//destructor left empty
FileWriter::~FileWriter(){
	std::cout << "dead" << std::endl;
}

void FileWriter::initFile(std::fstream &filestream, std::string filename){
	filestream.open(filename, std::ofstream::out | std::ofstream::trunc);

	if(!filestream){
		filestream.open(filename,  std::fstream::in | std::fstream::out | std::fstream::trunc);
		filestream << "\n";
		filestream.close();
	}
	else filestream.close();
}

void FileWriter::writeToFile(std::fstream &filestream, std::string filename, std::stringstream &stringstream){
	filestream.open(filename, std::ios_base::app);
	filestream << stringstream.str();
	filestream.close();
}

float FileWriter::readControllerValues(Identifier identifier){
	switch (identifier)
	{
	case Identifier::X: return FileWriter::ControllerXValue;
	case Identifier::Y: return FileWriter::ControllerYValue;
	default:return 0;
	}
}

void FileWriter::writeCameraInfo(Identifier identifier, float position, float velocity){

	std::string line = "";

	//Add Time, angular position and angular velocity
	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - starttime;
	line += std::to_string(ms_double.count()/1000) + ",";
	line += std::to_string(position) + "," + std::to_string(velocity) + "\n";

	//Write line to File
	switch (identifier){
		case Identifier::X: CameraXStream << line; break;
		case Identifier::Y: CameraYStream << line; break;
		default: break;
	}

}

void FileWriter::writeControllerInfo(Identifier identifier, float value){
	
	std::string line = "";

	//Add Time and controller value
	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - starttime;
	line += std::to_string(ms_double.count()/1000) + ",";
	line += std::to_string(value) + "\n";

	//Write line to File
	switch (identifier){
		case Identifier::X: 
			ControllerXStream << line;
			FileWriter::ControllerXValue = value;
			break;
		case Identifier::Y: 
			ControllerYStream << line;
			FileWriter::ControllerYValue = value;
			break;
		default: break;
	}

}

void FileWriter::writeFilterInfo(Identifier identifier, float angle, float ang_vel, float position, float velocity){
	std::string line = "";

	//Add Time and controller value
	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - starttime;
	line += std::to_string(ms_double.count()/1000) + ",";
	line += std::to_string(angle) + "," + std::to_string(ang_vel) + ",";
	line += std::to_string(position) + "," + std::to_string(velocity) + "\n";

	//Write line to File
	switch (identifier){
		case Identifier::X: FilterXStream << line; break;
		case Identifier::Y: FilterYStream << line; break;
		default: break;
	}
}

void FileWriter::writeUARTInfo(float xForce, float yForce, uint16_t xLidarFiltered, uint16_t yLidarFiltered, uint16_t xLidarRaw, uint16_t yLidarRaw){

	std::string line = "";

	//Add Time, Forces, filtered and raw Lidar data
	std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - starttime;
	line += std::to_string(ms_double.count()/1000) + ",";
	line += std::to_string(xForce) + "," + std::to_string(yForce) + ",";
	line += std::to_string(xLidarFiltered) + "," + std::to_string(yLidarFiltered) + ",";
	line += std::to_string(xLidarRaw) + "," + std::to_string(yLidarRaw) + "\n";

	//Write line to File
	UARTStream << line;
}

//writes all stringstreams to file
void FileWriter::handleCTRLC(){

	std::fstream CameraXFile;
	std::fstream CameraYFile;
	std::fstream FilterXFile;
	std::fstream FilterYFile;
	std::fstream ControllerXFile;
	std::fstream ControllerYFile;
	std::fstream UARTFile;

	//check if files exists, if yes: clear, if no: create
	initFile(CameraXFile, CameraXFileName);
	initFile(CameraYFile, CameraYFileName);
	initFile(FilterXFile, FilterXFileName);
	initFile(FilterYFile, FilterYFileName);
	initFile(ControllerXFile, ControllerXFileName);
	initFile(ControllerYFile, ControllerYFileName);
	initFile(UARTFile, UARTFileName);

	writeToFile(CameraXFile, CameraXFileName, CameraXStream);
	writeToFile(CameraYFile, CameraYFileName, CameraYStream);
	writeToFile(FilterXFile, FilterXFileName, FilterXStream);
	writeToFile(FilterYFile, FilterYFileName, FilterYStream);
	writeToFile(ControllerXFile, ControllerXFileName, ControllerXStream);
	writeToFile(ControllerYFile, ControllerYFileName, ControllerYStream);
	writeToFile(UARTFile, UARTFileName, UARTStream);

	std::cout << "Files written" << std::endl;
}