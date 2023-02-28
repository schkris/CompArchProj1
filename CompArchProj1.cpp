#include "CompArchProj1.h"

using namespace std;

int main()
{
    string line;
    string disLine;
    string fileName = "";
    string fileObj;
    string fileAsm;
    disReturn disassembleRet; // The disassembleLine function return struct (a bool and string)
    bool errorsPres = false; // If errors are present in the file set to true
    int lineNum = 1; // Keeps track of the line number

    // Prompts the user for the name of the file and stores it
    cout << "Please input the name of the .obj file: " << endl;
    cin >> fileName;

    // Concatenates the string to obj and asm file
    fileObj.append(fileName);
    fileObj.append(".obj");
    fileAsm.append(fileName);
    fileAsm.append(".asm");

    // Opens fileObj to read from and fileAsm to write to
    ifstream objStream(fileObj);
    ofstream asmStream(fileAsm);

    // If either of the files fail to open, returns this
    if (!objStream.is_open() || !asmStream.is_open())
    {
        cout << "Failed to open file!" << endl;
        return 1;
    }

    // Goes through each line in the obj file and uses disassembleLine() to disassemble 1 line at a time than prints it to .asm file
    while (std::getline(objStream, line))
    {
        disassembleRet = disassembleLine(line);
        if (disassembleRet.errorFound == true)
        {
            errorsPres = true;
            cout<<"Cannot disassemble "<< line <<" at line "<< lineNum << endl;
        }
        else
        {
            asmStream << disassembleRet.returnLine << endl;
        }
        lineNum++;
    }

    // Closes out files and finishes program
    asmStream.close();
    objStream.close();
    return 0;
}

disReturn disassembleLine(string line)
{
    disReturn disassembleRet;

    // Checks the length and returns an error if its not 8
    if (line.length() != 8)
    {
        cout << "Invalid Length" << endl;
        disassembleRet.errorFound = true;
        return disassembleRet;
    }

    // Converts line into binary and stores it in returnLine string
    disassembleRet = convertBinary(line);

    // Now if the error is found it is due to an invalid character
    if (disassembleRet.errorFound == true)
    {
        cout << "Invalid Char Present" << endl;
        return disassembleRet;
    }

    disassembleRet = convertInstruct(disassembleRet.returnLine);

    return disassembleRet;
}

disReturn convertBinary(string line)
{
    disReturn disassembleRet;
    string binary = "";
    string binaryChar;
    char currChar;

    // For loop goes through each character
    for (int i = 0; i < 8; i++)
    {
        currChar = line.at(i);
        disassembleRet.errorFound = !validity(currChar);
        // If an error is not found converts it to binary and adds it to the return line
        if (!disassembleRet.errorFound)
        {
            binaryChar = hexToBinary(currChar);
            binary.append(binaryChar);
        }
        else
        {
            return disassembleRet;
        }
    }
    disassembleRet.returnLine = binary;
    return disassembleRet;
}

bool validity (char curr)
{
    if (curr == '0' || curr == '1' || curr == '2' || curr == '3' || curr == '4' || curr == '5' || curr == '6' || curr == '7' || curr == '8' || curr == '9' || curr == 'a' || curr == 'b' || curr == 'c' || curr == 'd' || curr == 'e' || curr == 'f')
    {
        return true;
    }
    return false;
}

string hexToBinary(char hexChar)
{
    string binaryStr;
    switch (hexChar)
    {
    case '0': binaryStr = "0000"; break;
    case '1': binaryStr = "0001"; break;
    case '2': binaryStr = "0010"; break;
    case '3': binaryStr = "0011"; break;
    case '4': binaryStr = "0100"; break;
    case '5': binaryStr = "0101"; break;
    case '6': binaryStr = "0110"; break;
    case '7': binaryStr = "0111"; break;
    case '8': binaryStr = "1000"; break;
    case '9': binaryStr = "1001"; break;
    case 'a': binaryStr = "1010"; break;
    case 'b': binaryStr = "1011"; break;
    case 'c': binaryStr = "1100"; break;
    case 'd': binaryStr = "1101"; break;
    case 'e': binaryStr = "1110"; break;
    case 'f': binaryStr = "1111"; break;
    default: binaryStr = ""; break;
    }
    return binaryStr;
}

disReturn convertInstruct(string binaryLine)
{
    disReturn disassembleRet;
    string opcode;
    instructFormat format;

    // Picks out the first 6 digits for the opcode
    opcode = binaryLine.substr(0, 6);
    opcode.insert(0, "00");

    // Returns Instruction format
    format = instructType(opcode);

    if (format == instructFormat::r_type)
    {
        disassembleRet = rDecoder(binaryLine);
    }
    else
    {
        disassembleRet = iDecoder(binaryLine, opcode);
    }

    return disassembleRet;
}

instructFormat instructType(string opcode)
{
    if (opcode == "00000000")
    {
        return instructFormat::r_type;
    }
    return instructFormat::i_type;
}

disReturn rDecoder(string binaryLine)
{
    string rs = binaryLine.substr(6, 5);
    string rt = binaryLine.substr(11, 5);
    string rd = binaryLine.substr(16, 5);
    string shamt = binaryLine.substr(21, 5);
    string funct = binaryLine.substr(26, 6);
    disReturn disassembleRet;
    functReturn functRet;
    disReturn rdRet;
    disReturn rtRet;
    disReturn rsRet;
    int shamtRet;
    disassembleRet.returnLine = "";

    // If an error is found here the funct is not valid and returns funct return with error
    functRet = findfunct(funct);
    if (functRet.errorFound)
    {
        disassembleRet.returnLine = functRet.returnLine;
        disassembleRet.errorFound = functRet.errorFound;
        return disassembleRet;
    }

    // If the funct is valid it adds it to disassembleRet
    disassembleRet.returnLine.append(functRet.returnLine);

    // Uses the functReturn to figure out how to assess instruction (shamt or no shamt)
    if (functRet.shamtNeeded)
    {
        // Only 2 registers assessed and shamt
        rdRet = registerVal(rd);
        rtRet = registerVal(rt);
        shamtRet = shamtVal(shamt);
        disassembleRet.returnLine.append(" " + rdRet.returnLine);
        disassembleRet.returnLine.append(", " + rtRet.returnLine);
        disassembleRet.returnLine.append(", " + to_string(shamtRet));
    }
    else
    {
        // 3 registers assessed
        rdRet = registerVal(rd);
        rsRet = registerVal(rs);
        rtRet = registerVal(rt);
        disassembleRet.returnLine.append(" " + rdRet.returnLine);
        disassembleRet.returnLine.append(", " + rsRet.returnLine);
        disassembleRet.returnLine.append(", " + rtRet.returnLine);
    }


    return disassembleRet;
}

disReturn iDecoder(string binaryLine, string opcode)
{
    string opcodeStr1;
    string opcodeStr2;
    char hex1;
    char hex2;
    disReturn rsRet;
    disReturn rtRet;
    string rs = binaryLine.substr(6, 5);
    string rt = binaryLine.substr(11, 5);
    string immediateStr = binaryLine.substr(16, 16);
    int immNum;
    disReturn disassembleRet;
    opcodeReturn opcodeRet;
    disassembleRet.returnLine = "";

    // Converts opcode into 2 hex characters for further assessment
    opcodeStr1 = opcode.substr(0,4);
    opcodeStr2 = opcode.substr(4, 4);
    hex1 = binaryToHex(opcodeStr1);
    hex2 = binaryToHex(opcodeStr2);

    // Converts 2 hex digits into I instruction opcode
    opcodeRet = hexOpcode(hex1, hex2);

    // If an error is found here the funct is not valid and returns funct return with error
    if (opcodeRet.errorFound)
    {
        disassembleRet.returnLine = opcodeRet.returnLine;
        disassembleRet.errorFound = opcodeRet.errorFound;
        return disassembleRet;
    }

    // If the opcode is valid it adds it to disassembleRet
    disassembleRet.returnLine.append(opcodeRet.returnLine);

    // Finds the rs and rt strings, adds rt
    rsRet = registerVal(rs);
    rtRet = registerVal(rt);
    disassembleRet.returnLine.append(" " + rtRet.returnLine);

    // Finds the immediate value
    immNum = immVal(immediateStr);
    
    if (opcodeRet.offset == true)
    {
        // Adds the immediate value and rs in parenthesis
        disassembleRet.returnLine.append(", " + to_string(immNum));
        disassembleRet.returnLine.append("(" + rsRet.returnLine + ")");
    }
    else
    {
        // Normal format, just register, immediate
        disassembleRet.returnLine.append(", " + rsRet.returnLine);
        disassembleRet.returnLine.append(", " + to_string(immNum));
    }

    return disassembleRet;
}


opcodeReturn hexOpcode(char hex1, char hex2)
{
    opcodeReturn opcodeRet;
    if (hex1 == '0')
    {
        if (hex2 == '4')
        {
            opcodeRet.returnLine = "beq";
        }
        else if (hex2 == '5')
        {
            opcodeRet.returnLine = "bne";
        }
        else if (hex2 == '8')
        {
            opcodeRet.returnLine = "addi";
        }
        else if (hex2 == '9')
        {
            opcodeRet.returnLine = "addiu";
        }
        else if (hex2 == 'a')
        {
            opcodeRet.returnLine = "slti";
        }
        else if (hex2 == 'b')
        {
            opcodeRet.returnLine = "sltiu";
        }
        else if (hex2 == 'c')
        {
            opcodeRet.returnLine = "andi";
        }
        else if (hex2 == 'd')
        {
            opcodeRet.returnLine = "ori";
        }
        else if (hex2 == 'f')
        {
            opcodeRet.returnLine = "lui";
        }
        else
        {
            opcodeRet.returnLine = "Opcode not found";
            opcodeRet.errorFound = true;
        }
    }
    else if (hex1 == '2')
    {
        if (hex2 == '3')
        {
            opcodeRet.returnLine = "lw";
        }
        else if (hex2 == '4')
        {
            opcodeRet.returnLine = "lbu";
        }
        else if (hex2 == '5')
        {
            opcodeRet.returnLine = "lhu";
        }
        else if (hex2 == '8')
        {
            opcodeRet.returnLine = "sb";
        }
        else if (hex2 == '9')
        {
            opcodeRet.returnLine = "sh";
        }
        else if (hex2 == 'b')
        {
            opcodeRet.returnLine = "sw";
        }
        else
        {
            opcodeRet.returnLine = "Opcode not found";
            opcodeRet.errorFound = true;
        }
        opcodeRet.offset = true;
    }
    else if (hex1 == '3')
    {
        if (hex2 == '0')
        {
            opcodeRet.returnLine = "ll";
        }
        else if (hex2 == '8')
        {
            opcodeRet.returnLine = "sc";
        }
        else
        {
            opcodeRet.returnLine = "Opcode not found";
            opcodeRet.errorFound = true;
        }
        opcodeRet.offset = true;
    }
    else
    {
        opcodeRet.returnLine = "Opcode not found";
        opcodeRet.errorFound = true;
    }
    return opcodeRet;
}

disReturn registerVal(string reg)
{
    disReturn regReturn;
    int regNum = 0;

    // Picks out the register number from binary string
    for (int i = 0; i<5; i++)
    {
        if (reg[i] == '1')
        {
            regNum = regNum + (int)(pow(2, (4 - i)));
        }
    }

    // Makes sure the register number is valid
    if (regNum<0 || regNum>31)
    {
        regReturn.errorFound = true;
        return regReturn;
    }

    // Takes in register number and outputs register string
    regReturn.returnLine = regConvert(regNum);

    //cout << regReturn.returnLine << endl;
    return regReturn;
}

string regConvert(int regNum)
{
    switch (regNum) {
    // 0 and at 0-1
    case 0: return "$zero";
    case 1: return "$at";
    // value registers 2-3
    case 2: return "$v" + to_string(regNum - 2);
    case 3: return "$v" + to_string(regNum - 2);
    // argument registers 4-7
    case 4: return "$a" + to_string(regNum - 4);
    case 5: return "$a" + to_string(regNum - 4);
    case 6: return "$a" + to_string(regNum - 4);
    case 7: return "$a" + to_string(regNum - 4);
    // temporary registers 8-15
    case 8: return "$t" + to_string(regNum - 8);
    case 9: return "$t" + to_string(regNum - 8);
    case 10: return "$t" + to_string(regNum - 8);
    case 11: return "$t" + to_string(regNum - 8);
    case 12: return "$t" + to_string(regNum - 8);
    case 13: return "$t" + to_string(regNum - 8);
    case 14: return "$t" + to_string(regNum - 8);
    case 15: return "$t" + to_string(regNum - 8);
    // saved temporary registers 16-23
    case 16: return "$s" + to_string(regNum - 16);
    case 17: return "$s" + to_string(regNum - 16);
    case 18: return "$s" + to_string(regNum - 16);
    case 19: return "$s" + to_string(regNum - 16);
    case 20: return "$s" + to_string(regNum - 16);
    case 21: return "$s" + to_string(regNum - 16);
    case 22: return "$s" + to_string(regNum - 16);
    case 23: return "$s" + to_string(regNum - 16);
    // more temporary registers 24-25
    case 24: return "$t" + std::to_string(regNum - 16);
    case 25: return "$t" + std::to_string(regNum - 16);
    // Reserved for OS kernel
    case 26: return "$k" + std::to_string(regNum - 26);
    case 27: return "$k" + std::to_string(regNum - 26);
    // $gp
    case 28: return "$gp";
    // $sp
    case 29: return "$sp";
    // $fp
    case 30: return "$fp";
    // $ra
    case 31: return "$ra";
    default: return "";
    }
}

int immVal(string immediateStr)
{
    int immNum = 0;
    // Neg Condition
    cout << immediateStr << endl;
    if (immediateStr[0] == '1')
    {
        // Goes through bottom 15 binary digits and makes them the positive value
        for (int i = 1; i < 16; i++)
        {
            if (immediateStr[i] == '1')
            {
                immNum = immNum + (int)(pow(2, (15 - i)));
            }
        }
        // Subtracts positive value from 32768
        cout << immNum << endl;
        immNum = 32768 - immNum;
        // Makes value negative
        immNum = immNum * -1;
    }
    else
    {
        // Goes through bottom 15 binary digits and makes them the positive value
        for (int i = 1; i < 16; i++)
        {
            if (immediateStr[i] == '1')
            {
                immNum = immNum + (int)(pow(2, (15 - i)));
            }
        }
    }
    return immNum;
}

int shamtVal(string shamt)
{
    int shamtNum = 0;
    // Neg Condition
    if (shamt[0] == '1')
    {
        // Goes through bottom 4 binary digits and makes them the positive value
        for (int i = 1; i < 5; i++)
        {
            if (shamt[i] == '1')
            {
                shamtNum = shamtNum + (int)(pow(2, (4 - i)));
            }
        }
        // Subtracts positive value from 16 
        shamtNum = 16 - shamtNum;
        // Makes value negative
        shamtNum = shamtNum * -1;
    }
    else
    {
        // Goes through bottom 4 binary digits and makes them the positive value
        for (int i = 1; i < 5; i++)
        {
            if (shamt[i] == '1')
            {
                shamtNum = shamtNum + (int)(pow(2, (4 - i)));
            }
        }
    }
    return shamtNum;
}

functReturn findfunct(string funct)
{
    functReturn functRet;
    string digit1;
    string digit2;
    char hex1;
    char hex2;

    // Converts the funct string into 2 hex digits for evaluation
    funct.insert(0, "00");
    digit1 = funct.substr(0, 4);
    digit2 = funct.substr(4, 7);
    hex1 = binaryToHex(digit1);
    hex2 = binaryToHex(digit2);

    // Maps the hex digits to an instruction also means shamt is assessed
    if (hex1 == '0')
    {
        if (hex2 == '0') // 0 / 00
        {
            funct = "sll";
        }
        else if (hex2 == '2') // 0 / 02
        {
            funct = "srl";
        }
        else
        {
            funct = "Function not found";
            functRet.errorFound = true;
        }
        functRet.shamtNeeded = true;
    }
    else if (hex1 == '2')
    {
        if (hex2 == '0') // 0 / 20
        {
            funct = "add";
        }
        else if (hex2 == '1') // 0 / 21
        {
            funct = "addu";
        }
        else if (hex2 == '2') // 0 / 22
        {
            funct = "sub";
        }
        else if (hex2 == '3') // 0 / 23
        {
            funct = "subu";
        }
        else if (hex2 == '4') // 0 / 24
        {
            funct = "and";
        }
        else if (hex2 == '5') // 0 / 25
        {
            funct = "or";
        }
        else if (hex2 == '7') // 0 / 27
        {
            funct = "nor";
        }
        else if (hex2 == 'a') // 0 / 2a
        {
            funct = "slt";
        }
        else if (hex2 == 'b') // 0 / 2b
        {
            funct = "sltu";
        }
        else
        {
            funct = "Function not found";
            functRet.errorFound = true;
        }
        functRet.shamtNeeded = false;
    }
    else
    {
        funct = "Function not found";
        functRet.errorFound = true;
    }

    functRet.returnLine = funct;
    //cout << funct << endl;

    return functRet;
}

char binaryToHex(string binaryStr)
{
    char hexChar;
    if (binaryStr == "0000")
    {
        hexChar = '0';
    }
    else if (binaryStr == "0001")
    {
        hexChar = '1';
    }
    else if (binaryStr == "0010")
    {
        hexChar = '2';
    }
    else if (binaryStr == "0011")
    {
        hexChar = '3';
    }
    else if (binaryStr == "0100")
    {
        hexChar = '4';
    }
    else if (binaryStr == "0101")
    {
        hexChar = '5';
    }
    else if (binaryStr == "0110")
    {
        hexChar = '6';
    }
    else if (binaryStr == "0111")
    {
        hexChar = '7';
    }
    else if (binaryStr == "1000")
    {
        hexChar = '8';
    }
    else if (binaryStr == "1001")
    {
        hexChar = '9';
    }
    else if (binaryStr == "1010")
    {
        hexChar = 'a';
    }
    else if (binaryStr == "1011")
    {
        hexChar = 'b';
    }
    else if (binaryStr == "1100")
    {
        hexChar = 'c';
    }
    else if (binaryStr == "1101")
    {
        hexChar = 'd';
    }
    else if (binaryStr == "1110")
    {
        hexChar = 'e';
    }
    else if (binaryStr == "1111")
    {
        hexChar = 'f';
    }
    else
    {
        hexChar = '\0';

    }
    return hexChar;
}
