//
// Created by machercraft on 8/2/2026.
//

#ifndef low_LANG_low_LANG_API_H
#define low_LANG_low_LANG_API_H
#pragma once

#include <functional>

#include "string"
#include "memory"
#include "map"
#include "vector"

class Low_Bind {
public:
    virtual void Bind(){}
    virtual ~Low_Bind() = default;
};

extern  std::vector<Low_Bind*> Binds;
void Register_Bind(Low_Bind* b);
void Binder_load();
enum mem_plot_type{N,INT,STRING,STRUCK,BOOL,FLOAT};

class mem_plot {
    public:
    std::string index ;
    mem_plot_type type = N ;

    virtual ~mem_plot() = default;
};
class mem_plot_int : public mem_plot {
    public:
    int value = 0;
};class mem_plot_float : public mem_plot {
    public:
    float value = 0.0;
};
class mem_plot_string : public mem_plot {
    public:
    std::string value;
};
class mem_plot_bool : public mem_plot {
    public:
    bool value = false;
};
class mem_plot_struc : public mem_plot {
    public:
    std::string structypename;
    std::map<std::string,std::unique_ptr<mem_plot>*> struc;
};

void Exicute_lang(std::string pro);
void Trace_stat();
mem_plot_int* plot_as_parse_int(std::unique_ptr<mem_plot> *p);
mem_plot_string* plot_as_parse_string(std::unique_ptr<mem_plot> *p);
mem_plot_bool* plot_as_parse_bool(std::unique_ptr<mem_plot> *p);
std::string plot_parse_as_struing_alltype(std::unique_ptr<mem_plot> *p);
class low_function_binded {
public:
    std::function<void(std::vector< std::unique_ptr<mem_plot>*> inputs)> func;
};
void index_function_wrap(low_function_binded bind,std::string callname);
class low_struct_class{
public:
    std::string iden;
    mem_plot_type type;
    std::string val;
};
class low_struc_template {
public:
    std::string structtypename;
    std::vector<low_struct_class> patern ;
};
void Index_Struct(low_struc_template &tem);

#endif //low_LANG_low_LANG_API_H
