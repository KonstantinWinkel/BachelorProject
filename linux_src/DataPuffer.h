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
        private:
        size_t size;
        int current_size;
        const std::function<double(int)> weight_func;
        T* data;
        int ptr; //might have been smarter with a queue? coz with a q we could keep track of its head+tail, which would make every application of current_size more intuitive
        double normalize; 

        public:
        void update_normalized(){
            double sum = 0;
            int i = 0;
            while(i < current_size){
                sum += weight_func(i++);
            }
            normalize = 1/sum;
        }

        inline int get_ptr(int index){
            int ret = (ptr-index)%size;
            return ret<0?ret+size:ret;
        }

        RingBuffer<T>(size_t size,const std::function<double(int)>& weight_func = [](int index){return 1;}): 
            data(new T[size]), size(size), ptr(-1), current_size(0), weight_func(weight_func){ };

        ~RingBuffer(){
            delete data;
        }

        void set(T new_data){
            current_size = ++current_size>size?size:current_size;
            ptr = (ptr+1)%size;
            data[ptr] = new_data;
        }

        inline int get(T& data_ref, int index = 0){
            if(!(index<current_size)) {
                std::cout << "index: " << index << " current_cize: " << current_size << std::endl;
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

        std::string to_string(){ //returns contents in .csv format
            std::stringstream s;
            T buf;
            for(int i = 0; i<current_size;++i){
                if(get(buf,i)) std::cout << "index " << i << " out of bounds (size=" << current_size << ')' << std::endl;
                //get(buf,i);
                s << buf << ((i==current_size-1)?"":",");
            }
            return s.str();
        }
    };
}
#endif 