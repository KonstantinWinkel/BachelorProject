#if !defined(__DATABUFFER__)
#define __DATABUFFER__

#include "bachelor_debug.h"

#if defined(_DEBUG_DATAPUFFER_ON_)
#define _debug_print_buffer_(x) std::cout << x << std::endl
#else
#define _debug_print_buffer_(x)
#endif

#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <chrono>
#include <atomic>

namespace je{
    template<typename T> class RingBuffer
    {
        protected:
        size_t size;
        int current_size;
        const std::function<double(int)> weight_func;
        T* data;
        int ptr; //might have been smarter with a queue? coz with a q we could keep track of its head+tail, which would make every application of current_size more intuitive
        double normalize; 

        public:
        int temp_ptr(){
            return ptr;
        }

        int get_current_size(){
            return current_size;
        }

        void update_normalized(){
            double sum = 0;
            int i = 0;
            while(i < current_size){
                sum += weight_func(i++);
            }
            normalize = 1/sum;
        }

        inline int get_ptr(int index){
            int ret = (ptr-index+size)%size;
            return ret;
        }

        RingBuffer(size_t size,const std::function<double(int)>& weight_func = [](int index){return 1;}): 
            data(new T[size]), size(size), ptr(-1), current_size(0), weight_func(weight_func){ };

        ~RingBuffer(){
            delete data;
        }

        void set(T new_data){
            //current_size = (++current_size)>size?size:current_size;
            if(current_size < size){
                current_size++;
                //std::cout << "WE GOING UP" << current_size << std::endl;
            }
            else current_size = size;
            ptr = (ptr+1)%size;
            data[ptr] = new_data;
        }

        inline int get(T& data_ref, int index = 0){
            if(!(index<current_size)) {
                //std::cout << "index: " << index << " current_cize: " << current_size << std::endl;
                return -1;
            }
            data_ref = data[get_ptr(index)];
            return 0;
        }

        T get_weighted_sum(){
            update_normalized();
            T sum;
            for(int i = 0; i<current_size;++i) 
            {
                sum += weight_func(i)*data[i];
            }
            return sum;
        }

        T get_sum(){
            update_normalized();
            T sum(0);
            for(int i = 0; i<current_size;++i) 
            {
                sum += data[i];
            }
            return sum;
        }

        T get_weighted_average(){
            update_normalized();
            T sum(0);
            for(int i = 0; i<current_size;++i) 
            {
                sum += weight_func(i)*data[i];
            }
            return sum*normalize;
        }

        T get_average(){
            update_normalized();
            T sum(0);
            for(int i = 0; i<current_size;++i) 
            {
                sum += data[i];
            }
            return sum/current_size;
        }

        /*virtual std::string to_string(){ //returns contents in .csv format
            std::stringstream s;
            T buf;
            for(int i = 0; i<current_size;++i){
                if(get(buf,i)) std::cout << "index " << i << " out of bounds (size=" << current_size << ')' << std::endl;
                //get(buf,i);
                s << buf << ((i==current_size-1)?"":",");
            }
            return s.str();
        }*/
    };

    struct datapoint{
        double value;
        double time;

        datapoint operator +(const datapoint& a){ //implemented because I dont want ringbuffermethods to throw an exception. never rly use it.
            return datapoint{this->value+a.value,this->time+a.time};
        }
        datapoint operator /(const double& b){
            return datapoint{this->value/b,this->time/b};
        }
        std::ostream& operator<<(std::ostream& os)
        {
            return os << "value: " << this->value << " time: " << this->time;
        }
        std::string to_string(){
            std::stringstream s;
            s << "value: " << this->value << " time: " << this->time;
            return s.str();
        }
    };

    class linear_regression : public RingBuffer<datapoint> {
        
        protected:
        double sum(size_t size, const std::function<double(int)>& inner_func){
            double ret_sum = 0;
            for(int i=0;i<size;i++)ret_sum += inner_func(i);
            return ret_sum;
        }

        std::string make_jank(datapoint d)
        {
            std::stringstream s;
            s << "value: " << d.value << " time: " << d.time;
            return s.str();
        }

        public:
        linear_regression(size_t size):
            RingBuffer(size){ };

        double get_slope(){
            if(current_size == 0) return 0;
            if(current_size == 1) return 0;
            double s_x = sum(current_size,[=](int i){return data[i].value;});
            double s_xx = sum(current_size,[=](int i){return data[i].value*data[i].value;});
            double s_xy = sum(current_size,[=](int i){return data[i].value*data[i].time;});
            double s_y = sum(current_size,[=](int i){return data[i].time;});

            if(current_size*s_xx-s_x*s_x==0) return 0;
        
            return (current_size * s_xy - s_x * s_y)/(current_size*s_xx-s_x*s_x);
        }

        double get_intercept(){
            if(current_size == 0) return 0;
            double s_x = sum(current_size,[=](int i){return data[i].value;});
            double s_xx = sum(current_size,[=](int i){return data[i].value*data[i].value;});
            double s_xy = sum(current_size,[=](int i){return data[i].value*data[i].time;});
            double s_y = sum(current_size,[=](int i){return data[i].time;});

            if(current_size*s_xx-s_x*s_x == 0) return data[1].value;
            if(s_xx == 0.0) return data[0].value;

            return (s_y*s_xx-s_x*s_xy)/(current_size*s_xx-s_x*s_x);
        }

        std::string to_csv(){
            std::stringstream s;
            datapoint buf;
            for(int i = 0; i<current_size;++i){
                get(buf,i); 
                s << make_jank(buf) << ((i==current_size-1)?"":",");
            }
            return s.str();
        }
    };
}
#endif 