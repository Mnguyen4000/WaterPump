
#include "struct.h"
#include <string.h>
#include <stdlib.h>

/*
    Deciphers the JSON string from the PC Web Dashboard
    Returns 0 if successful, 1 if error.
*/
int decipher_json(struct State state, char* string) {
    // Format
    // {"Pump": 0, "Valve": 1}
    // Where 0 is OFF, 1 is ON

    char string[30] = "{\"Pump\": 0, \"Valve\": 1}\0";
    char copy[30];

    strcpy(copy, string);

    char* buff1;
    char* buff2;
    
    // Obtain value for Pump
    buff1 = strtok(string, ",");
    buff1 = strtok(buff1, " ");
    buff2 = strtok(NULL, " ");
    state->pump = atoi(buff2);
    
    // Obtain value for Valve
    buff1 = strtok(copy, ",");
    buff1 = strtok(NULL, ",");
    buff2 = strtok(buffer, " ");
    buff2 = strtok(NULL, " ");
    state->valve = atoi(buff2);

    return 0;
}

