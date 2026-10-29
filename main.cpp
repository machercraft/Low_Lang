#include <format>
#include <iostream>
#include <ostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "low_lang.h"
#include "low_lang_api.h"
#include "low_lang_compiller.h"

//<> vec2 [ x 0.0 , y 0.0] $ @ main [<< v -> vec2 [2.5,0.0] ;  << a 3.0; @a / 2.0  #< a ; @ <> v -> x + 2.5 ] #?
// binded function

int main(int argc, char ** argv) {

    std::string p ="";

    if (argv[1] != nullptr)
    p = argv[1];

    if (p == "run") {
        std::string co = argv[2];
        std::ifstream program_file(co+"/main.l");

        std::stringstream probuff;

        probuff << program_file.rdbuf();

        program_file.close();

        std::string program = probuff.str();

        Exicute_lang(program);
    }
    else if (p == "compile") {
        platform pl = Linux;

        std::string pls = argv[2];
        std::string co;

        if (pls == "linux") {
            pl = Linux;
            co = argv[3];
        }else if (pls == "windows") {
            pl = Windows;
            co = argv[3];
        }else {
            co = argv[2];
#if WIN32
            pl = Windows;
#elif __linux__
            pl = Linux;
#endif
        }

        std::cout << "all done"<<std::endl;

        Compiler(co,pl);
    }
    else if (p == "CLI") {
        std::cout << "------|[Low_Lang CLI]|------"<<std::endl;
        std::cout << "============================="<<std::endl;
        while (true) {
            std::cout << "[L]<_ ";
            std::string co;
            std::getline(std::cin,co);

            if (co == "clear") {
#if _WIN32
                system("cls");
#elif __linux__
                system("clear");
#endif
            } else {
                Exicute_lang(co);
            }
        }
    }
    else {
        exit(888);
    }
}

