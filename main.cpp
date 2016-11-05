#include <iostream>
#include <string>
#include "library.h"

struct people {
    std::string     m_name;
    unsigned short  m_age;
};

struct people Pterosaur{"Pterosaur", 24};

extern "C" int add(int a, int b) {
    return a + b;
}

int main() {

    dlfcncpp::library mainProgram;
    if (!mainProgram.open()) {
        std::cerr << mainProgram.error() << std::endl;
        return 1;
    }
    auto padd = mainProgram.symbol<int(int,int)>("add");
    if (!padd) {
        std::cerr << mainProgram.error() << std::endl;
        return 1;
    }
    std::cout << "2 + 3 = " << padd(2,3) << std::endl;
    auto ppeople = mainProgram.symbol<struct people *>("Pterosaur");
    if (!ppeople) {
        std::cerr << mainProgram.error() << std::endl;
        return 1;
    }
    std::cout << "name : " << ppeople->m_name << "  "
              << "age : " << ppeople->m_age << std::endl;


    dlfcncpp::library explamelib("./libexamplelib.so");
    if (!explamelib) {
        std::cerr << explamelib.error() << std::endl;
        return 1;
    }
    auto pmultiplication = explamelib.symbol<int(int,int)>("multiplication");
    if (!pmultiplication) {
        std::cerr << explamelib.error() << std::endl;
        return 1;
    }
    std::cout << "2 * 3 = " << pmultiplication(2,3) << std::endl;


    return 0;
}