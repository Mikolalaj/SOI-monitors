
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "buffer.hpp"
#include "pierogarnia.hpp"

using namespace std;

struct arg_struct {
    Buffer *buffer;
    Buffer *buffer_ciasto;
    int index;
    int buffer_size;
};

void *produce(void *arguments)
{
    arg_struct *args = (arg_struct *)arguments;
    Buffer *buffer = args->buffer;
    int index = args->index;
    int buffer_size = args->buffer_size;

    while (true) {
        sleep(1);
        int count = buffer->put();
        
        printf("Producer %d: Created %s (%d/%d)\n", 
               index, buffer->get_item_name().c_str(), count, buffer_size);
    }
}

void *consume(void *arguments)
{
    arg_struct *args = (arg_struct *)arguments;
    Buffer *buffer_nadzienie = args->buffer;
    Buffer *buffer_ciasto = args->buffer_ciasto;
    int index = args->index;
    int buffer_size = args->buffer_size;

    while (true) {
        sleep(1);

        int count_nadzienie = buffer_nadzienie->get();
        int count_ciasto = buffer_ciasto->get();
            
        printf("Consumer %d: Made pierog with %s (%d/%d) (Ciasto %d/%d)\n",
               index, buffer_nadzienie->get_item_name().c_str(), count_nadzienie,
               buffer_size, count_ciasto, buffer_size);
    }
}

Buffer* get_random_buffer(Pierogarnia *pierogarnia) {
    int index = rand()%4;

    Buffer* random_buffer;

    if (index == 0) {
        random_buffer = &pierogarnia->ciasto;
    }
    else if (index == 1) {
        random_buffer = &pierogarnia->mieso;
    }
    else if (index == 2) {
        random_buffer = &pierogarnia->ser;
    }
    else if (index == 3) {
        random_buffer = &pierogarnia->kapusta;
    }

    return random_buffer;
}

Buffer* get_random_buffer_nadzienie(Pierogarnia *pierogarnia) {
    int index = rand()%3;

    Buffer* random_buffer;

    if (index == 0) {
        random_buffer = &pierogarnia->mieso;
    }
    else if (index == 1) {
        random_buffer = &pierogarnia->ser;
    }
    else if (index == 2) {
        random_buffer = &pierogarnia->kapusta;
    }

    return random_buffer;
}

void fill_buffers(Pierogarnia *pierogarnia, Buffer* buffers[], int producers)
{
    // we must have all the ingredients for a pierog
    buffers[0] = &pierogarnia->ciasto;
    buffers[1] = &pierogarnia->mieso;
    buffers[2] = &pierogarnia->ser;
    buffers[3] = &pierogarnia->kapusta;

    // if we have more availabe producers, then we want more "ciasto" produced
    if (producers > 4) {
        buffers[4] = &pierogarnia->ciasto;
    }

    // any more available producers will be random
    for (int i=5; i<producers; i++) {        
        buffers[i] = get_random_buffer(pierogarnia);
    }
}

void fill_buffers_nadzienie(Pierogarnia *pierogarnia, Buffer* buffers[], int consumers)
{
    buffers[0] = &pierogarnia->mieso;
    buffers[1] = &pierogarnia->ser;
    buffers[2] = &pierogarnia->kapusta;

    for (int i=3; i<consumers; i++) {        
        buffers[i] = get_random_buffer_nadzienie(pierogarnia);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        cout << "You need to pass 3 arguments: number of producers, number of consumers, and buffer size" << endl;
        return 1;
    }

    int producers = atoi(argv[1]);
    int consumers = atoi(argv[2]);
    int buffer_size = atoi(argv[3]);

    srand(time(NULL));

    pthread_t th_producers[producers], th_consumers[consumers];

    Pierogarnia pierogarnia = Pierogarnia(buffer_size);

    arg_struct args;
    args.buffer_ciasto = &pierogarnia.ciasto;
    args.buffer_size = buffer_size;

    arg_struct args_list_producers[producers], args_list_consumers[consumers];    

    int max = (consumers > producers ? consumers : producers);
    
    int indexes[max]; // Just used for numbering the producer and consumer
    for (int i = 0; i < max; i++) {
        indexes[i] = i+1;
    }

    Buffer* buffers[producers];
    Buffer* buffers_nadzienie[consumers];
    fill_buffers(&pierogarnia, buffers, producers);
    fill_buffers_nadzienie(&pierogarnia, buffers_nadzienie, consumers);

    for (int i = 0; i < producers; i++) {
        args.buffer = buffers[i];
        args.index = indexes[i];
        args_list_producers[i] = args;
        pthread_create(&th_producers[i], NULL, produce, (void *)&args_list_producers[i]);
        sleep(1);
    }
    for (int i = 0; i < consumers; i++) {
        args.buffer = buffers_nadzienie[i];
        args.index = indexes[i];
        args_list_producers[i] = args;
        pthread_create(&th_consumers[i], NULL, consume, (void *)&args_list_producers[i]);
        sleep(1);
    }

    for (int i = 0; i < producers; i++) {
        pthread_join(th_producers[i], NULL);
    }
    for (int i = 0; i < consumers; i++) {
        pthread_join(th_consumers[i], NULL);
    }

    return 0;
}