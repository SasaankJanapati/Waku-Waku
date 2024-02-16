#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>

class Assembler
{
private:
    std::map<std::string, std::string> lookup_table;

public:
    std::ifstream File;

    void display(std::string file);
    std::vector<int> assemble(std::string file);
    Assembler();
};
Assembler::Assembler()
{
    lookup_table["x0"] = "00000";
    lookup_table["x1"] = "00001";
    lookup_table["x2"] = "00010";
    lookup_table["x3"] = "00011";
    lookup_table["x4"] = "00100";
    lookup_table["x5"] = "00101";
    lookup_table["x6"] = "00110";
    lookup_table["x7"] = "00111";
    lookup_table["x8"] = "01000";
    lookup_table["x9"] = "01001";
    lookup_table["x10"] = "01010";
    lookup_table["x11"] = "01011";
    lookup_table["x12"] = "01100";
    lookup_table["x13"] = "01101";
    lookup_table["x14"] = "01110";
    lookup_table["x15"] = "01111";
    lookup_table["x16"] = "10000";
    lookup_table["x17"] = "10001";
    lookup_table["x18"] = "10010";
    lookup_table["x19"] = "10011";
    lookup_table["x20"] = "10100";
    lookup_table["x21"] = "10101";
    lookup_table["x22"] = "10110";
    lookup_table["x23"] = "10111";
    lookup_table["x24"] = "11000";
    lookup_table["x25"] = "11001";
    lookup_table["x26"] = "11010";
    lookup_table["x27"] = "11011";
    lookup_table["x28"] = "11100";
    lookup_table["x29"] = "11101";
    lookup_table["x30"] = "11110";
    lookup_table["x31"] = "11111";
}
void Assembler::display(std::string file)
{
    File.open(file);
    std::string line;
    while (std::getline(File, line))
    {
        std::cout << line << std::endl;
    }
}

std::vector<int> Assembler::assemble(std::string file)
{
    std::vector<int> result;
    File.open(file);
    std::string line;
    while (std::getline(File, line))
    {
        if (line == "")
        {
            continue;
        }
        std::vector<std::string> tokens;
        std::stringstream stream(line);
        std::string token;
        while (std::getline(stream, token, ' '))
        {
            tokens.push_back(token);
        }
        if (tokens[0] == "add")
        {
            std::string opcode = "0110011";
            std::string rd = lookup_table[tokens[1]];
            std::string func3 = "000";
            std::string rs1 = lookup_table[tokens[2]];
            std::string rs2 = lookup_table[tokens[3]];
            std::string func7 = "0000000";
            std::string instruction = func7 + rs2 + rs1 + func3 + rd + opcode;
            int bin_instruction = std::stoi(instruction, nullptr, 2);
            result.push_back(bin_instruction);
        }
    }
    return result;
}
