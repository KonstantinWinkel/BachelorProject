#include "Linear_regression_filter.h"
#include <chrono>
#include <iomanip>

Linear_regression_filter::Linear_regression_filter(Identifier axis, double camera_latency, double lidar_latencysize_t,size_t size_x, size_t size_phi)
    :lr_x(size_x),lr_phi(size_phi),file(path){
    file << std::fixed;
    file << std::setprecision(2);
    Filter::state = {0,0,0,0};
    Filter::axis = axis;
    Linear_regression_filter::camera_latency = camera_latency;
    Linear_regression_filter::lidar_latency = lidar_latency;
    Linear_regression_filter::start_time = std::chrono::high_resolution_clock::now();
    Linear_regression_filter::time_now = 0;
	Linear_regression_filter::path = ((std::string)"filter_data/filter") + (axis == Identifier::X ? "X" : "Y") + ".txt";
	//std::ofstream file(path);
    file.open(path, std::ofstream::out | std::ofstream::trunc);
	if(!file){
		file.open(path,  std::fstream::in | std::fstream::out | std::fstream::trunc);
		file << "\n";
		file.close();
	}
	else file.close();
}

void Linear_regression_filter::write_to_file(double new_value, std::array<double,4> state){
    file.open(path, std::ios_base::app);
	file << "new_value: " << new_value << " state: " << state[0] << ", " << state[1] << ", " << state[2] << ", " << state[3] << std::endl;
	file.close();
}

double Linear_regression_filter::time_since_start(std::chrono::_V2::system_clock::time_point time){
    std::chrono::duration<double, std::milli> duration = time-start_time;
    return (duration).count()/1000;
}

inline void Linear_regression_filter::update_pos(){
    state[1] = lr_x.get_slope();
    state[0] = lr_x.get_intercept();// + (time_now+lidar_latency)*state[1];
}

inline void Linear_regression_filter::update_angle(){
    state[3] = lr_phi.get_slope();
    state[2] = lr_phi.get_intercept();// + (time_now+camera_latency)*state[3];
}

void Linear_regression_filter::update_pos(uint16_t pos_uint, double last_u, double state[4]){
    time_now = time_since_start(std::chrono::high_resolution_clock::now());
    lr_x.set(datapoint{to_m(pos_uint),time_now});
    update_angle();
    update_pos();
    file.open(path, std::ios_base::app);
	//file <<  "#T: " << time_now << "###\tnew_pos: " << to_m(pos_uint) << "\tstate:" << state[0] << ",\t" << state[1] << ",\t" << state[2] << ",\t" << state[3] << std::endl;
	file.close();
    for(int i = 0;i<4;++i) Filter::state[i] = state[i];
}

void Linear_regression_filter::update_angle(double phi, double last_u, double state[4]){
    time_now = time_since_start(std::chrono::high_resolution_clock::now());
    lr_phi.set(datapoint{phi,time_now});
    update_angle();
    update_pos();
    file.open(path, std::ios_base::app);
	file << "#T: " << time_now <<"###\tnew_angle: " << phi << "\tstate: " << state[0] << ",\t" << state[1] << ",\t" << state[2] << ",\t" << state[3] << std::endl;
    file << "phi buffer: " << lr_phi.to_csv()<<std::endl;
	file.close();
    for(int i = 0;i<4;++i) Filter::state[i] = state[i];
}