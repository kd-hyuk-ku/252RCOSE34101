#include <random>
#include <iostream>

int main() {
unsigned int seed =2020250010;
std::mt19937 rng(seed);
std::uniform_int_distribution<int> delay(1,5); 
std::uniform_int_distribution<int> throughput(1,3);

int delay_option = delay(rng); 
int throughput_option = throughput(rng); 

std::cout<<"Student number : "<<seed<<std::endl;
std::cout<<"Delay_option : "<<delay_option<<std::endl;
std::cout<<"Throughput_option : "<<throughput_option<<std::endl;

    return 0;
}

