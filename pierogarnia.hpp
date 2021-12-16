#ifndef PIEROGARNIA_HPP
#define PIEROGARNIA_HPP

#include <pthread.h>
#include "buffer.hpp"

class Pierogarnia {
    public:
        Buffer ciasto;
        Buffer mieso;
        Buffer kapusta;
        Buffer ser;

        Pierogarnia(int buffer_size) {
            this->ciasto = Buffer("Ciasto", buffer_size);
            this->mieso = Buffer("Mięso", buffer_size);
            this->kapusta = Buffer("Kapusta", buffer_size);
            this->ser = Buffer("Ser", buffer_size);
        }
};

#endif