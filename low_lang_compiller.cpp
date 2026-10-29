//
// Created by machercraft on 8/14/2026.
//
#include <filesystem>
#include <iostream>
#include "low_lang_compiller.h"
#include <fstream>

void Compiler (std::string prof,platform plat) {
    std::string pro;
    std::cout <<"[Compilling] " << prof << std::endl;
    auto dr = std::filesystem::directory_iterator(prof);
    for (auto & script: dr) {

        if (script.path().extension().string() == ".l") {
            std::cout << "["<<script.path().string()<<"]"<<std::endl;

            std::ifstream program_file(script.path().string());

            while (!program_file.eof()) {
                std::string cline;

                std::getline(program_file,cline);

                for (int i = 0; i < cline.size(); ++i) {
                    pro += cline[i];

                    if (cline[i+1] == '"') {
                        pro += "\\";
                    }
                }
            }

            program_file.close();
        }

    }



    // ./low_lang "path to .l file" "path to export"
    if (!exists(std::filesystem::path(prof+"/compile"))) {
        std::filesystem::create_directory(prof+"/compile");
    }

    std::filesystem::create_directory(prof+"/build");

    std::filesystem::create_directory(prof+"/build/lib");

    if (plat == Windows) {
        std::filesystem::copy("build/liblow_windows.a",prof+"/build/lib");
    }
    else if (plat == Linux) {
        std::filesystem::copy("build/liblow_linux.a",prof+"/build/lib");
    }

    std::filesystem::copy("build/low_lang_api.h",prof+"/build/lib");

    std::ofstream bootf (prof+"/build/lib/_boot.cpp");
    bootf.clear();
    std::string bc = "#include <format>\n"
    "#include <iostream>\n"
    "#include <vector>\n"
    "#include <map>\n"
    "#include <string>\n"
    "#include <sstream>\n"
    "#include <fstream>\n"
    "#include <filesystem>\n"
    "#include \"low_lang_api.h\"\n"
    "int main() {\n"
    "std::string program = \" "+pro+" \";\n"
    "Exicute_lang(program);\n"
    "return 0;\n"
    "}\n";

    bootf << bc ;
    bootf.close();

    std::string fn = std::filesystem::path(prof).filename().string();

    std::string comand;

    if (plat == Windows) {
        comand += "x86_64-w64-mingw32-";
    }

    comand += "g++ -std=c++20 ";
    if (exists(std::filesystem::path(prof+"/bindings"))) {
        comand += prof+"/bindings/*.cpp ";
    }
    comand+=prof+"/build/lib/_boot.cpp ";

    if (plat == Linux) {
        comand+=prof+"/build/lib/liblow_linux.a ";
    }
    else if (plat == Windows) {
        comand+=prof+"/build/lib/liblow_windows.a ";
    }

    comand+=" -I "+prof+"/build/lib"
            " -o "+prof+"/compile/"+fn;

    if (plat == Windows) {
        comand+=".exe";
    }

    if (system(comand.c_str()) == 0) {
        std::string runc = prof+"/compile/"+fn;

        std::cout << "Compile Done." << std::endl;
        std::filesystem::remove_all(prof+"/build");

#if _WIN32
                system("cls");
#elif __linux__
                system("clear");
#endif

        system(runc.c_str());
    } else {
        std::filesystem::remove_all(prof+"/build");
        std::filesystem::remove_all(prof+"/compile");
    }
}
