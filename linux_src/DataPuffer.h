#if !defined(__DATABUFFER__)
#define __DATABUFFER__

#include <mutex>

#include <chrono>

namespace je{
template<typename T>
class CommBuffer
{
private:
    T data;
    bool new_data;
    bool written_to;
    std::chrono::_V2::system_clock::time_point last_modified; //necessary for KF predictor

    inline void unsafe_set(T data){
        last_modified = std::chrono::high_resolution_clock::now();
        this -> data = data;
        this -> new_data = true;
    }

public:
    bool newData(){
        return new_data;
    }

    void set(T data){ //mutex does not compile, because it is neither movable, nor copyable (not sure why thats a problem, but i guess a bool will have to sufice)
        if(written_to) return;
        written_to = true;
        unsafe_set(data);
        written_to = false;
    }
    
    void get(T& data){
        data = this-> data;
        new_data = false;
    }
    void get(T& data,std::chrono::_V2::system_clock::time_point& last_modified){
        if(written_to) return;
        written_to = true;
        unsafe_get(data);
        last_modified = this-> last_modified;
        written_to = false;
    }
};

/*template<typename T>
class SmartCommBuffer : CommBuffer<T>
{
private:
    std::thread thread;
public:
    set_and_resume(T& data){
        m.lock();
        CommBuffer::unsafe_set(T data);
        //thread.resume(); 
        m.unlock();
    }
};*/
}
#endif 