#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "monitor.hpp"
#include <iostream>
#include <pthread.h>

using namespace std;

class Buffer : public Monitor {
    protected:
        string item_name;
        int count;
        int buffer_size;
        Condition empty;
        Condition full;
    
    public:
        Buffer() {
            this->item_name = "";
            this->count = 0;
            this->buffer_size = 0;
        }

        Buffer(string name, int buffer_size) {
            this->item_name = name;
            this->count = 0;
            this->buffer_size = buffer_size;
        }

        int put() {
            enter();

            if (count >= this->buffer_size)
                wait(full);
            count++;
            int return_count = count;
            if (count == 1)
                signal(empty);

            leave();
            return return_count;
        }

        int get() {
            enter();

            if (count <= 0)
                wait(empty);
            count--;
            int return_count = count;
            if (count == this->buffer_size - 1)
                signal(full);
            
            leave();
            return return_count;
        }

        string get_item_name() {
            return item_name;
        }

        int get_count() {
            return count;
        }
};

#endif
