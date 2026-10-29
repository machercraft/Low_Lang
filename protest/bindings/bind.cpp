#include <iostream>
#include <istream>
#include "low_lang_api.h"
int main() {
    low_function_binded bind;
    bind.func = [](std::vector<std::unique_ptr<mem_plot> *> inputs) {
        std::string m = plot_as_parse_string(inputs[0])->value;
        std::cout << m <<std::endl;
    };

    //index bind
    index_function_wrap(bind,"count");

    return 0;
}