//
// Created by machercraft on 7/23/2026.
//

#ifndef TEST_low_PLOTIZER_H
#define TEST_low_PLOTIZER_H
#pragma once

#include "string"
#include "memory"
#include "vector"
#include "low_lang_api.h"
//class and enum

enum platform {Windows,Linux};

enum low_token_types
{
    eolt,remt,intt,floatt,text,ident,keyt,deft,pot,
    outt,inpt,addt,subt,mult,mulmt,divit,eqult,eequalt,nott,
    noteqt,gretthant,lowthant,lowthanoreqt,gretthanoreqt,
    ift,bopt,bclt,whilet,rett,fct,passlt,passrt,
    structt,andt
};


class low_token {
public:
    low_token_types type;
    std::string val;
};


class low_statement {
public:
    virtual ~low_statement() = default;
};

class low_statement_def_plot_struc : public low_statement {
public:
    std::string iden;
    std::string structypename;
    std::vector<low_token*>vals;
};
class low_statement_def_plot : public low_statement {
public:
    std::string iden;
    mem_plot_type type;
    std::string val;
};

class low_statement_rem : public low_statement {
public:
    bool func = false;
    std::string ident;
};
class low_statement_pointer_point : public low_statement {
public:
    bool point_on_struc = false;
    std::string passpoint;
    std::string point;
};
class low_statement_bi_ex : public low_statement {
public:
    low_token_types op;
    low_token_types type;
    std::string val;
};
class low_statement_comparison : public low_statement {
public:
    bool re = false;
    bool returntob = false;
    low_token_types op;
    low_token_types type;
    std::string val;
    std::string reboolname;
};
class low_statement_out : public low_statement {
public:
    bool valpoint = false;
    low_token_types type;
    std::string data;
};

class low_statement_in : public low_statement {
public:
};

class low_function {
public:
    std::vector<std::unique_ptr<low_statement>> statements;
};

class low_call_function :public low_statement {
public:
    std::string functionname;
};
class low_call_function_binded :public low_statement {
public:
    std::string functionname;
    std::vector<std::string>inputs;
};
class low_statement_if : public low_statement {
public:
    bool expect = true;
    bool valpoint = false;
    low_statement_comparison *dynamiccom;
    std::string conname;
    std::string bodyfunc;
};
class low_statement_while : public low_statement {
public:
    bool expect = true;
    bool valpoint = false;
    low_statement_comparison *dynamiccom;
    std::string conname;
    std::string bodyfunc;
};
#endif //TEST_low_PLOTIZER_H
