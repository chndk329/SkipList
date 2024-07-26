/*
 *  程序功能：时间性能测试
 *
 *  作者：DK
 *  日期：2024/07/26
 *
*/

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <time.h>
#include <cstring>
#include <chrono>
#include <vector>
#include <map>
#include "src/skiplist.h"

const int N = 1000000;

skiplist<int, std::string> slist;
std::map<int, std::string> stlmap;

int main()
{
    std::cout << std::endl;
    std::cout << "Data count: " << N << std::endl << std::endl;
    // insert
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < N; i ++) slist.insert(i, "Test");
        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "SkipList insert elapsed: " << elapsed.count() << "s" << std::endl;
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < N; i ++) stlmap.insert({i, "Test"});
        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Map insert elapsed: " << elapsed.count() << "s" << std::endl << std::endl;
    }

    // query
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < N; i ++) slist.find(i);
        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "SkipList query elapsed: " << elapsed.count() << "s" << std::endl;
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < N; i ++) stlmap.find(i);
        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Map query elapsed: " << elapsed.count() << "s" << std::endl << std::endl;
    }

    // erase
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < N; i ++) slist.erase(i);
        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "SkipList erase elapsed: " << elapsed.count() << "s" << std::endl;
    }
    {
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < N; i ++) stlmap.erase(i);
        auto finish = std::chrono::high_resolution_clock::now(); 
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Map erase elapsed: " << elapsed.count() << "s" << std::endl << std::endl;
    }

}
