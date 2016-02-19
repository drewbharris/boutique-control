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

    if (num_bytes != 3 || message_type != 178) { // 176 - channel 1, 177 - channel 2, 178 - channel 3
        boutique_midi_out->sendMessage(message);
        return;
    }

    unsigned char controller = (unsigned char)message->at(1);
    unsigned char value = (unsigned char)message->at(2);

    std::vector<unsigned char> new_message(16);

    // start system exclusive
    new_message[0] = 0xF0;

    // boilerplate
    new_message[1] = 0x41;
    new_message[2] = 0x10;
    new_message[3] = 0x00;
    new_message[4] = 0x00;
    new_message[5] = 0x00;
    new_message[6] = 0x1D; // 1D: juno, 1E: jx, 1C: jupiter
    new_message[7] = 0x12;
    new_message[8] = 0x03;
    new_message[9] = 0x00;

    printf("\ncontroller, value: %d, %d\n", controller, value);

    // parameter
    unsigned char p_1 = (controller / 2) >> 4;
    unsigned char p_2 = (controller / 2) & 0x0F;
    new_message[10] = p_1;
    new_message[11] = p_2;

    printf("p_1, p_2: %d, %d\n", p_1, p_2);

    // value
    unsigned char v_1 = (value / 2) >> 4;
    unsigned char v_2 = (value / 2) & 0x0F;
    new_message[12] = v_1;
    new_message[13] = v_2;

    // checksum
    unsigned char cs = (100 - ((0x03 + 0x00 + p_1 + p_2 + v_1 + v_2) & 0xFF)) & 0x7F;
    new_message[14] = cs;

    // end
    new_message[15] = 0xF7;

    // bytes = [0xF0, 0x41, 0x10, 0x00, 0x00, 0x00, 0x1C, 0x12, 0x03, 0x00,  P1,  P2,   V1,  V2,   CS,   F7]
    //         [0xf0, 0x41, 0x10, 0x00, 0x00, 0x00, 0x1d, 0x12, 0x3,  0x0,  0x0, 0x0, 0xc0, 0xc, 0x15, 0xf7,]

    // debug
    printf("message: [");
    for (int i = 0; i < new_message.size(); i++) {
        printf("0x%x,", new_message[i]);
    }
    printf("]\n");

    boutique_midi_out->sendMessage(&new_message);

    return;
}

void print_usage() {
    printf("Usage:\n");
    printf("boutique-ctrl --input=(port number) --output=(port number) [--help]\n\n");
}

int main(int argc, char* argv[]) {

    for (unsigned int i = 0; i < argc; i++) {
        if (strncmp(argv[i], "--input", 7) == 0) {
            char * port_array = strtok(argv[i],"--input="); 
            int port_number = port_array[0] - 48;
            printf("opening input port %d\n", port_number);
            boutique_midi_in->openPort(port_number);
        }
        else if (strncmp(argv[i], "--output", 8) == 0) {
            char * port_array = strtok(argv[i],"--output="); 
            int port_number = port_array[0] - 48;
            printf("opening output port %d\n", port_number);
            boutique_midi_out->openPort(port_number);
        }
        else if (strncmp(argv[i], "--list", 6) == 0) {
            std::string portName;

            printf("MIDI inputs:\n");
            for (unsigned int i=0; i < boutique_midi_in->getPortCount(); i++ ) {
                portName = boutique_midi_in->getPortName(i);
                printf("%d: %s\n", i, portName.c_str());
            }

            printf("MIDI outputs:\n");
            for (unsigned int i=0; i < boutique_midi_out->getPortCount(); i++ ) {
                portName = boutique_midi_out->getPortName(i);
                printf("%d: %s\n", i, portName.c_str());
            }

            return 0;
        }
        else if (strncmp(argv[i], "--help", 6) == 0) {
            print_usage();
            return 0;
        }
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
