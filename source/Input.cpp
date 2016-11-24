#include "gba.h"
#include "Input.h"

void Input::update(){
	k[0] = REG_KEYINPUT ^ 0x03FF;
	k[1] = (k[0] ^ k[3]) & k[0];
	k[2] = (k[0] ^ k[3]) & k[3];
	k[3] = k[0];
}

unsigned short *Input::GetKey(){
	return k;
}

unsigned short Input::GetKey0(){
	return k[0];
}

unsigned short Input::GetKey1(){
	return k[1];
}

unsigned short Input::GetKey2(){
	return k[2];
}

bool Input::isKey0(unsigned short key){
	return (k[0] & key) != 0;
}

bool Input::isKey1(unsigned short key){
	return (k[1] & key) != 0;
}

bool Input::isKey2(unsigned short key){
	return (k[2] & key) != 0;
}
