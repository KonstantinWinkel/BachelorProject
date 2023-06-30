//c++ includes
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <math.h>

void replace_value (float  &original, float replace_value, int &counter){
    original = replace_value;
    counter++;
}

void initFile(std::fstream &filestream, std::string filename){
    filestream.open(filename, std::ios::out);

    if (!filestream) {
		std::cout << "File" << filename << " could not be created!" << std::endl;
        exit(1);
	}
}

void generateCleanReport(int line_counter, std::string input_name, double execution_time, int rep_counter){
    std::cout << "------------------------------------" << std::endl;
    std::cout << "Analized " << line_counter << " from " << input_name << " in " << execution_time << " secounds." << std::endl;
    std::cout << "Replaced " << rep_counter << " values. " << std::endl;
}

void cleanCameraFile(std::string input_name, std::string output_name, float thresh_angle, float thresh_velocity, float rep_angle, float rep_velocity){
    
    std::ifstream input_file(input_name);
    std::fstream output_file;
    
    initFile(output_file, output_name);

    std::string line = "";
    std::string newline;
    float time, angle, velocity;
    int line_counter = 0;
    int rep_counter = 0;

    std::chrono::_V2::system_clock::time_point start_time = std::chrono::high_resolution_clock::now();

    while(input_file >> line){
        sscanf(line.c_str(), "%f,%f,%f", &time, &angle, &velocity);

        if(std::fabs(angle) > thresh_angle) replace_value(angle, rep_angle, rep_counter);
        if(std::fabs(velocity) > thresh_velocity) replace_value(velocity, rep_velocity, rep_counter);
        line_counter++;

        newline = std::to_string(time) + "," + std::to_string(angle) + "," + std::to_string(velocity) + "\n";
        output_file << newline;
    }

    output_file.close();

    std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - start_time;
	double execution_time = (ms_double.count()/1000);

    generateCleanReport(line_counter, input_name, execution_time, rep_counter);

}

void cleanFilterFile(){
    //TODO
}

void cleanControllerFile(){
    //TODO
}

void cleanUARTFile(std::string input_name, std::string output_name, float thresh_force, float thresh_LIDAR, float rep_force, float rep_LIDAR){
    
    std::ifstream input_file(input_name);
    std::fstream output_file;
    
    initFile(output_file, output_name);

    std::string line = "";
    std::string newline;
    float time, xforce, yforce, xlidar_filtered, ylidar_filtered, xlidar_raw, ylidar_raw;
    int line_counter = 0;
    int rep_counter = 0;

    std::chrono::_V2::system_clock::time_point start_time = std::chrono::high_resolution_clock::now();

    while(input_file >> line){
        sscanf(line.c_str(), "%f,%f,%f,%f,%f,%f,%f", &time, &xforce, &yforce, &xlidar_filtered, &ylidar_filtered, &xlidar_raw, &ylidar_raw);

        if(std::fabs(xforce) > thresh_force) replace_value(xforce, rep_force, rep_counter);
        if(std::fabs(yforce) > thresh_force) replace_value(yforce, rep_force, rep_counter);

        if(std::fabs(xlidar_filtered) > thresh_LIDAR) replace_value(xlidar_filtered, rep_LIDAR, rep_counter);
        if(std::fabs(ylidar_filtered) > thresh_LIDAR) replace_value(ylidar_filtered, rep_LIDAR, rep_counter);
        if(std::fabs(xlidar_raw) > thresh_LIDAR) replace_value(xlidar_raw, rep_LIDAR, rep_counter);
        if(std::fabs(ylidar_raw) > thresh_LIDAR) replace_value(ylidar_raw, rep_LIDAR, rep_counter);
        

        line_counter++;

        newline = std::to_string(time) + "," + std::to_string(xforce) + "," + std::to_string(yforce) + ",";
        newline += std::to_string(xlidar_filtered) + "," + std::to_string(ylidar_filtered) + "," + std::to_string(xlidar_raw) + "," + std::to_string(ylidar_raw) + "\n";
        output_file << newline;
    }

    output_file.close();

    std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - start_time;
	double execution_time = (ms_double.count()/1000);

    generateCleanReport(line_counter, input_name, execution_time, rep_counter);

}

int main(int argc, char** argv){

    std::string fileNameToClean = "";

    const std::string input_CameraXFileName = "camera_x_output.csv";
    const std::string input_CameraYFileName = "camera_y_output.csv";
    const std::string input_FilterXFileName = "filter_x_output.csv";
    const std::string input_FilterYFileName = "filter_y_output.csv";
    const std::string input_ControllerXFileName = "controller_x_output.csv";
    const std::string input_ControllerYFileName = "controller_y_output.csv";
    const std::string input_UARTFileName = "uart_output.csv";

    const std::string output_CameraXFileName = "camera_x_output_clean.csv";
    const std::string output_CameraYFileName = "camera_y_output_clean.csv";
    const std::string output_FilterXFileName = "filter_x_output_clean.csv";
    const std::string output_FilterYFileName = "filter_y_output_clean.csv";
    const std::string output_ControllerXFileName = "controller_x_output_clean.csv";
    const std::string output_ControllerYFileName = "controller_y_output_clean.csv";
    const std::string output_UARTFileName = "uart_output_clean.csv";

    std::chrono::_V2::system_clock::time_point total_start_time = std::chrono::high_resolution_clock::now();

    cleanCameraFile(input_CameraXFileName, output_CameraXFileName, 1000.0, 1000.0, 90.0, 0.0);
    cleanCameraFile(input_CameraYFileName, output_CameraYFileName, 1000.0, 1000.0, 90.0, 0.0);

    cleanControllerFile();
    cleanControllerFile();

    cleanFilterFile();
    cleanFilterFile();

    cleanUARTFile(input_UARTFileName, output_UARTFileName, 1000.0, 1000.0, 0.0, -10.0);

    std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - total_start_time;
	double execution_time = (ms_double.count()/1000);

    std::cout << std::endl;
    std::cout << "Cleaned all files in " << execution_time << " secounds." << std::endl;

}