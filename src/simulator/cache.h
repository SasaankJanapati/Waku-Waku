#include <cmath>
#include <bitset>
#include <iostream>
#include <bits/stdc++.h>

int bin_to_int(std::string bin)
{
    int n = bin.length();
    int r = 0;
    int w = 1;
    for (int i = n - 1; i >= 0; i--)
    {
        if (i == 1)
        {
            if (bin[i] == '1')
            {
                r += w;
            }
            w *= -2;
            continue;
        }
        else if (bin[i] == '1')
        {
            r += w;
        }
        w *= 2;
    }
    return r;
}
class Cache
{
private:
    /* data */
    int cache_size;
    int block_size;
    int associativity;
    int sets;
    int number_of_index_bits;
    int number_of_offset_bits;
    int number_of_tag_bits;
    int hits;
    int misses;
    int policy;
    std::vector<std::vector<std::pair<int, std::pair<std::vector<int>, int>>>> cache; // each vector is a block and has it's own recency

public:
    Cache(int cache_size, int block_size, int associativity, int policy);
    std::pair<int, bool> read(int address, int *memory);
    void write(int address, int data);
    void LRU(std::vector<int> block, std::vector<std::pair<int, std::pair<std::vector<int>, int>>> &set, int tag);
    void RRP(std::vector<int> block, std::vector<std::pair<int, std::pair<std::vector<int>, int>>> &set, int tag);
};

Cache::Cache(int cache_size, int block_size, int associativity, int policy)
{
    this->cache_size = cache_size;
    this->block_size = block_size;
    this->associativity = associativity;
    this->sets = cache_size / block_size / associativity;
    this->number_of_index_bits = (int)log2(sets);
    this->number_of_offset_bits = (int)log2(block_size);
    this->number_of_tag_bits = 32 - this->number_of_index_bits - this->number_of_offset_bits;
    this->policy = policy;
#ifdef DEBUG
    std::cout << "ib " << number_of_index_bits << std::endl;
    std::cout << "ob " << number_of_offset_bits << std::endl;
    std::cout << "tb " << number_of_tag_bits << std::endl;
#endif
    hits = 0;
    misses = 0;
    for (int i = 0; i < sets; i++)
    {
        std::vector<std::pair<int, std::pair<std::vector<int>, int>>> temp;
        cache.push_back(temp);
    }
}

std::pair<int, bool> Cache::read(int address, int *memory)
{
    std::bitset<32> bin_address(address);
    std::string address_string = bin_address.to_string();
    std::string tag_string = address_string.substr(0, number_of_tag_bits);
    int tag = std::stoi(tag_string, nullptr, 2);

    int offset = 0, index = 0;
    if (number_of_offset_bits != 0)
    {
        std::string offset_string = address_string.substr(number_of_tag_bits + number_of_index_bits, number_of_offset_bits);
        offset = std::stoi(offset_string, nullptr, 2);
    }
    if (number_of_index_bits != 0)
    {
        std::string index_string = address_string.substr(number_of_tag_bits, number_of_index_bits);
        index = std::stoi(index_string, nullptr, 2);
    }
    bool found = false;
    int tag2 = 0;
    for (int i = 0; i < cache[index].size(); i++)
    {
        if (cache[index][i].first == tag)
        {
            tag2 = i;
            found = true;
        }
    }
    if (found)
    {
        hits++;
        for (auto block = cache[index].begin(); block != cache[index].end(); block++)
        {
            block->second.second++;
        }
        cache[index][tag2].second.second = 1;
        return {cache[index][tag2].second.first[offset], true};
    }
    misses++;
    address = (address / block_size) * block_size;
    std::vector<int> block;
    for (int i = address; i < address + block_size; i++)
    {
        block.push_back(memory[i]);
    }

    if (cache[index].size() == associativity)
    {
        if (policy == 0)
            LRU(block, cache[index], tag);
        else if (policy == 1)
            RRP(block, cache[index], tag);
    }
    else
    {
        cache[index].push_back({tag, {block, 1}});
    }
    for (auto block = cache[index].begin(); block != cache[index].end(); block++)
    {
        block->second.second++;
    }
    for (int i = 0; i < cache[index].size(); i++)
    {
        if (cache[index][i].first == tag)
        {
            tag2 = i;
            found = true;
        }
    }
    cache[index][tag2].second.second = 1;
#ifdef DEBUG
    for (auto i : cache)
    {
        for (auto j : i)
        {
            auto x = j.second;
            for (auto k : x.first)
            {
                std::cout << k << " " << x.second << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
#endif
    return {cache[index][tag2].second.first[offset], false};
}

void Cache::LRU(std::vector<int> block, std::vector<std::pair<int, std::pair<std::vector<int>, int>>> &set, int tag)
{
    auto itr = set.begin();
    int MRU = itr->second.second;
    for (auto i = set.begin(); i != set.end(); i++)
    {
        if (i->second.second > MRU)
        {
            MRU = i->second.second;
            itr = i;
        }
    }
    set[itr - set.begin()] = {tag, {block, 1}};
}

void Cache::RRP(std::vector<int> block, std::vector<std::pair<int, std::pair<std::vector<int>, int>>> &set, int tag)
{
    int index = rand() % associativity;
    set[index] = {tag, {block, 1}};
}

void Cache::write(int address, int data)
{
    std::bitset<32> bin_address(address);
    std::string address_string = bin_address.to_string();
    std::string tag_string = address_string.substr(0, number_of_tag_bits);
    int tag = std::stoi(tag_string, nullptr, 2);

    int offset = 0, index = 0;
    if (number_of_offset_bits != 0)
    {
        std::string offset_string = address_string.substr(number_of_tag_bits + number_of_index_bits, number_of_offset_bits);
        offset = std::stoi(offset_string, nullptr, 2);
    }
    if (number_of_index_bits != 0)
    {
        std::string index_string = address_string.substr(number_of_tag_bits, number_of_index_bits);
        index = std::stoi(index_string, nullptr, 2);
    }
    bool found = false;
    int tag2 = 0;

    for (int i = 0; i < cache[index].size(); i++)
    {
        if (cache[index][i].first == tag)
        {
            tag2 = i;
            found = true;
        }
    }
    if (found)
    {
        hits++;
        for (auto block = cache[index].begin(); block != cache[index].end(); block++)
        {
            block->second.second++;
        }
        cache[index][tag2].second.second = 1;
        cache[index][tag2].second.first[offset] = data;
        return;
    }
    misses++;
    // We are not fetching if it is not there. We just update memory.
}
