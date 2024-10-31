#ifndef JSON_H_
#define JSON_H_

/*
    Deciphers the JSON string from the PC Web Dashboard
    Returns 0 if successful, 1 if error.
*/
int decipher_json(struct State state, char* string);

#endif