// boutique.cpp

#include <string>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <functional>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include "lib/RtMidi.h"
#include <sys/time.h>
#include <sys/types.h> 

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KMAG  "\x1B[35m"

std::string my_port = "UM-ONE";

RtMidiIn *boutique_midi_in = new RtMidiIn();
RtMidiOut *boutique_midi_out = new RtMidiOut();
RtMidiIn *daw_midi_in = new RtMidiIn();
RtMidiOut *daw_midi_out = new RtMidiOut();

void boutique_midi_callback(double deltatime, std::vector< unsigned char > *message, void *userData) {
    unsigned int num_bytes = message->size();

    if (num_bytes < 16) {
        return;
    }

    unsigned char p_1 = (unsigned char)message->at(10);
    unsigned char p_2 = (unsigned char)message->at(11);
    unsigned char v_1 = (unsigned char)message->at(12);
    unsigned char v_2 = (unsigned char)message->at(13);

    unsigned char controller = ((p_1 << 4) | p_2) / 2;
    unsigned char value = ((v_1 << 4) | v_2) / 2;

    // printf("\ngot [%d, %d]", controller, value);

    // send the message over MIDI

    std::vector<unsigned char> new_message(3);
    new_message[0] = 176;
    new_message[1] = controller;
    new_message[2] = value;

    daw_midi_out->sendMessage(&new_message);

    return;
}

// todo: pass on noteon/noteoff
void daw_midi_callback(double deltatime, std::vector< unsigned char > *message, void *userData) {
    unsigned int num_bytes = message->size();

    unsigned char message_type = (unsigned char)message->at(0);

    if (num_bytes != 3 || message_type != 176) {
        return;
    }

    unsigned char controller = (unsigned char)message->at(1);
    unsigned char value = (unsigned char)message->at(2);

    printf("\ngot [%d, %d]", controller, value);







    

    return;
}

int main(int argc, char* argv[]) {

    int found_port = 0;

    std::string port_name;
    for ( unsigned int i=0; i<boutique_midi_in->getPortCount(); i++ ) {
        port_name = boutique_midi_in->getPortName(i);
        if (port_name == my_port) {
            printf("found port!\n");
            found_port = 1;
            boutique_midi_in->openPort(i);
        }
    }

    for ( unsigned int i=0; i<boutique_midi_out->getPortCount(); i++ ) {
        port_name = boutique_midi_out->getPortName(i);
        if (port_name == my_port) {
            printf("found port!\n");
            found_port = 1;
            boutique_midi_out->openPort(i);
        }
    }

    if (!found_port) {
        printf("unable to find port\n");
        return 0;
    }

    boutique_midi_in->setCallback(&boutique_midi_callback);

    daw_midi_out->openVirtualPort("Boutique Control");
    daw_midi_in->openVirtualPort("Boutique Control");
    daw_midi_in->setCallback(&daw_midi_callback);

    // Don't ignore sysex, timing, or active sensing messages.
    boutique_midi_in->ignoreTypes( false, false, false );
    printf("\nReading MIDI input ... press <enter> to quit.\n");
    char input;
    std::cin.get(input);

    return 0;
}
