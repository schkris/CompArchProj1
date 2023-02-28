#pragma once

#ifndef COMPARCHPROJ1_HPP
#define COMPARCHPROJ1_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

using namespace std;

// The final return that is tranlsted through functions, an error variable and the string
struct disReturn
{
	string returnLine = "";
	bool errorFound = false;
	bool printAddress = false; // true with branch functions
	int addressOffset = 0;
};

// Needed an extra bool for funct return to figure out is shamt is assesed
struct functReturn
{
	string returnLine = "";
	bool errorFound = false;
	bool shamtNeeded = false;
};

// Needs an extra bool because the printing format varies between offset and just normal immediate value
struct opcodeReturn
{
	string returnLine = "";
	bool errorFound = false;
	bool offset = false;
	bool printAddress = false;
};

struct printInstruct
{
	string instruction = "";
	int addressOffset = 0;
	bool printAddress = false; // true with branch functions
};

// Labels for i type and r type instructions
enum class instructFormat {i_type, r_type};

// Algorithm to disasemble the current line while also checking for errors
disReturn disassembleLine(string line);

// Algorithm to convert the line into binary while checking that no invalid characters are present
disReturn convertBinary(string line);

// Returns true if char is valid and false if not
bool validity(char curr);

// Converts hex char to binary string
string hexToBinary(char hexChar);

// Converts the binary into instructions while checking that the instruction is valid
disReturn convertInstruct(string binaryLine);

// Takes in opcode and returns instruction type
instructFormat instructType(string opcode);

// Checks r-type instruction in binary form for errors and prints out MIPS equivalent
disReturn rDecoder(string binaryLine);

// Checks i-type instruction in binary form for errors and prints out MIPS equivalent
disReturn iDecoder(string binaryLine, string opcode);

// Converts 2 hex digits into an I-instruction opcode, also checks for invalid opcode
opcodeReturn hexOpcode(char hex1, char hex2);

// Finds the funct from the binary value, finds if shamt or a 3rd register is assessed, spits out an error if not a valid funct
functReturn findfunct(string funct);

// Finds the register from the binary value, will spit out an error if not a valid register
disReturn registerVal(string reg);

// Converts immediateStr to corresponding int value
int immVal(string immediateStr);

// Converts shamt string to corresponding int value
int shamtVal(string shamt);

// Converts regNum to register string
string regConvert(int regNum);

// Converts binary string to hex char
char binaryToHex(string binaryStr);

#endif
