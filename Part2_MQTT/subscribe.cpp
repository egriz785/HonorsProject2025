#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include "MQTTClient.h"
#include "display/LCDCharacterDisplay.h"

using namespace std;
using namespace exploringBB;

#define ADDRESS     "127.0.0.1:1883"
#define CLIENTID    "Beagle2"
#define TOPIC       "lcd"
#define AUTHMETHOD  "UserName"
#define AUTHTOKEN   "PassWord"
#define QOS         1
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt) {
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

// This is the only function different than in the template
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    int i;
    char* payloadptr;
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    payloadptr = (char*) message->payload;

    cout << string(payloadptr) << endl;

    // Added code, everything outside of this is paho template
    SPIDevice *busDevice = new SPIDevice(0,0);      //Using SPI0.0
    busDevice->setSpeed(1000000);                   // Have access to SPI Device object
    
    LCDCharacterDisplay display(busDevice, 16, 2);  // Construct 16x2 LCD Display
    display.clear();                                // Clear the character LCD module
    
    // Create stringstream so we can read the payload
    stringstream ss;
    string token;    
    ss << payloadptr << endl;

    // Shut off the cursor, just visual anyways
    display.setCursorOff(true);
    display.setCursorBlink(false);

    // Counts if this should be on the top line or bottom line
    // If for some reason the user inputs more than 2 lines, it will
    // Just print the first one on the first line 
    // And the last one on the bottom line
    int linecounter = 0;

    // Tokenize by '\n'
    while(getline(ss,token)) {
      // Print the line to the console
      cout << linecounter << ": " << token << endl;

      // Print line to LCD
      display.setCursorPosition(linecounter,0);
	    display.print(token);
	    
      // Move to bottom line
      linecounter = 1;
    }
    // End of added code, everything else is paho template

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause) {
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    opts.keepAliveInterval = 20;
    opts.cleansession = 1;
    opts.username = AUTHMETHOD;
    opts.password = AUTHTOKEN;
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);

    do {
        ch = getchar();
    } while(ch!='Q' && ch != 'q');
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}

