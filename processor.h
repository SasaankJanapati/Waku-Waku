#include <iostream>
#include "core.h"
#include "assembler.h"

class Processor
{
private:
    int memory[1024] = {0};
    Core cores[2] = {Core(0), Core(858)}; // .text is of size 84 words
    int clock;
    Assembler assembler;

public:
    Processor(std::string file1, std::string file2);
};

Processor::Processor(std::string file1, std::string file2)
{
    clock = 0;
    int dataloc1 = 84;
    int dataloc2 = 943;
    std::pair<std::vector<int>, std::vector<int>> result1 = assembler.assemble(file1);
    std::pair<std::vector<int>, std::vector<int>> result2 = assembler.assemble(file2);

    std::vector<int> instructions1 = result1.first;
    std::vector<int> instructions2 = result2.first;

    std::vector<int> data1 = result1.second;
    std::vector<int> data2 = result2.second;

    for (int index = 0; index < instructions1.size(); index++)
    {
        memory[index] = instructions1.at(index);
    }
    for (int index = 858; index - 858 < instructions2.size(); index++)
    {
        memory[index] = instructions2.at(index - 858);
    }

    memory[8] = 7;
    int i = 0, j = 0;
    while (i < instructions1.size() && j < instructions2.size())
    {
        cores[0].fetch(memory);
        cores[1].fetch(memory);
        cores[0].decode();
        cores[1].decode();
        i = cores[0].execute();
        j = cores[1].execute() - 858;
        cores[0].mem(memory);
        cores[1].mem(memory);
        clock++;
    }
    while (i < instructions1.size())
    {
        cores[0].fetch(memory);
        cores[0].decode();
        i = cores[0].execute();
        cores[0].mem(memory);
        clock++;
    }
    while (j < instructions2.size())
    {
        cores[1].fetch(memory);
        cores[1].decode();
        j = cores[1].execute() - 858;
        cores[1].mem(memory);
        clock++;
    }
}
