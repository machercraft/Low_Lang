//
// Created by machercraft on 7/23/2026.
//

#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <vector>
#include <string>


#include "low_lang.h"
#include "low_lang_api.h"

// v 0.9
//memory
std::string chbuff;
low_token_types chtbuff;
std::vector<low_token> tokens;


std::map<std::string,std::unique_ptr<mem_plot>> mem_tape;
std::map<std::string, low_function> functions;
std::map<std::string, low_function_binded> functions_binded;
std::map<std::string,low_struc_template> struct_templates;

void Index_Struct(low_struc_template &tem) {
    struct_templates[tem.structtypename] = tem;
}

mem_plot_int* plot_as_parse_int(std::unique_ptr<mem_plot> *p) {
    return dynamic_cast<mem_plot_int*>(p->get());
}
mem_plot_float* plot_as_parse_float(std::unique_ptr<mem_plot> *p) {
    return dynamic_cast<mem_plot_float*>(p->get());
}
mem_plot_string* plot_as_parse_string(std::unique_ptr<mem_plot> *p) {
    return dynamic_cast<mem_plot_string *>(p->get());
}
mem_plot_bool* plot_as_parse_bool(std::unique_ptr<mem_plot> *p) {
    return dynamic_cast<mem_plot_bool *>(p->get());
}
std::string plot_parse_as_struing_alltype(std::unique_ptr<mem_plot> *p) {
    std::string str;

    if (p->get()->type == INT) {
        str = std::to_string(dynamic_cast<mem_plot_int *>(p->get())->value);
    }else if (p->get()->type == FLOAT) {
        str = std::to_string(dynamic_cast<mem_plot_float *>(p->get())->value);
    }else if (p->get()->type == STRING) {
        str = dynamic_cast<mem_plot_string *>(p->get())->value;
    }else if (p->get()->type == BOOL) {
        str = std::to_string(dynamic_cast<mem_plot_bool *>(p->get())->value);
    }

    return str;
}


//debug
std::string code_on_exi;
bool debug = false;
std::vector<std::string> code_flow;

std::string point;

void low_exit() {
    Trace_stat();
    //exit(999);
}

void low_def(std::string p, std::string val, mem_plot_type t) {
     code_flow.push_back("def");

    if (t == INT) {
        mem_plot_int pi ;
        pi.index = p;
        pi.type = t;
        pi.value = std::stoi(val);
        mem_tape[p] = std::make_unique<mem_plot_int>(pi);
    }else if (t == FLOAT) {
        mem_plot_float pi ;
        pi.index = p;
        pi.type = t;
        pi.value = std::stof(val);
        mem_tape[p] = std::make_unique<mem_plot_float>(pi);
    }else if (t == STRING) {
        mem_plot_string pi ;
        pi.index = p;
        pi.type = t;
        pi.value = val;
        mem_tape[p] = std::make_unique<mem_plot_string>(pi);
    }else if (t == BOOL) {
        mem_plot_bool pi ;
        pi.index = p;
        pi.type = t;
        if (val == "true") {
            pi.value = true;
        }if (val == "true") {
            pi.value = false;
        }
        mem_tape[p] = std::make_unique<mem_plot_bool>(pi);
    }
    else {
        low_exit();
    }
}

void index_function_wrap(low_function_binded bind, std::string callname) {
    functions_binded[callname] = bind;
}

std::unique_ptr<mem_plot> *low_point(std::string p) {
    if (!mem_tape.contains(p)) {
        low_exit();
    }
    point = p;
    return &mem_tape[point];
}
std::unique_ptr<mem_plot> *low_point(std::unique_ptr<mem_plot> * pl) {
    point = pl->get()->index;
    return &mem_tape[point];
}

std::unique_ptr<mem_plot> *low_point_val() {
    if (point.empty()) {
        low_exit();
    }
    return &mem_tape[point];
};

std::unique_ptr<mem_plot> *low_plot_val(std::string p) {
    if (!mem_tape.contains(p)) {
        low_exit();
    }
    return &mem_tape[p];
};

void token_val() {
    if (!chbuff.empty()) {
        if (chbuff == "true" ||
            chbuff == "false") {
            chtbuff = keyt;
        }
        tokens.push_back(low_token(chtbuff, chbuff));
        chbuff.clear();
    }
}
void token_val(low_token_types ty) {
    if (!chbuff.empty()) {
        if (chbuff == "true" ||
            chbuff == "false") {
            chtbuff = keyt;
        }
        tokens.push_back(low_token(ty, chbuff));
        chbuff.clear();
    }
}

void Tokenizer(std::string pro) {
    code_on_exi = pro;
    //"" capture text
    bool txop = false;

    //capture comments
    bool comcap = false;
    int i = 0;
    while (i < pro.size()) {

        if (isspace(pro[i])) {
            i++;
        }

        low_token_types tb = intt;

        while (isdigit(pro[i])) {
            chbuff += pro[i];

            if (pro[i+1] == '.') {
                chbuff += pro[i+1];
                tb = floatt;
                i+=2;
            }
            else {
                i++;
            }
        }
        token_val(tb);

        while (isalpha(pro[i])||isdigit(pro[i])) {
            chtbuff = ident;
            chbuff += pro[i];

            i++;
        }
        token_val();

        if (pro[i] == '"') {
            txop = true;
            i++;
        }
        while (txop) {
            if (pro[i] != '"') {
                chtbuff = text;
                chbuff += pro[i];
            }
            if (pro[i++] == '"') {
                txop = false;
            }
        }
        token_val();


        if (pro[i] == '/' && pro[i + 1] == '/') {
            comcap = true;
            i += 2;
        }
        while (comcap) {
            i++;
            if (pro[i] == '/' && pro[i + 1] == '/') {
                comcap = false;
                i += 2;
            }
        }


        if (!comcap) {
            if (pro[i] == '<' && pro[i + 1] == '-') {
                chtbuff = passlt;
                chbuff += "<-";
                i += 2;
            } else if (pro[i] == '-' && pro[i + 1] == '>') {
                chtbuff = passrt;
                chbuff += "->";
                i += 2;
            } else if (pro[i] == '#' && pro[i + 1] == '<') {
                chtbuff = outt;
                chbuff += "#<";
                i += 2;
            } else if (pro[i] == '#' && pro[i + 1] == '>') {
                chtbuff = inpt;
                chbuff += "#>";
                i += 2;
            } else if (pro[i] == '=' && pro[i + 1] == '=') {
                chtbuff = eequalt;
                chbuff += "==";
                i += 2;
            }else if (pro[i] == '!' && pro[i + 1] == '=') {
                chtbuff = noteqt;
                chbuff += "!=";
                i += 2;
            }else if (pro[i] == '<' && pro[i + 1] == '=') {
                chtbuff = lowthanoreqt;
                chbuff += "<=";
                i += 2;
            }else if (pro[i] == '>' && pro[i + 1] == '=') {
                chtbuff = gretthanoreqt;
                chbuff += ">=";
                i += 2;
            } else if (pro[i] == '?') {
                chtbuff = ift;
                chbuff += "?";
                i ++;
            }else if (pro[i] == '<' && pro[i + 1] == '<') {
                chtbuff = deft;
                chbuff += "<<";
                i += 2;
            }else if (pro[i] == '<' && pro[i + 1] == '>') {
                chtbuff = structt;
                chbuff += "<>";
                i += 2;
            }else if (pro[i] == '+' && pro[i + 1] == '+') {
                chtbuff = mulmt;
                chbuff += "++";
                i += 2;
            } else if (pro[i] == '#' && pro[i + 1] == '?') {
                debug = true;
                i += 2;
            } else if (pro[i] == '!' && pro[i + 1] == '!' && pro[i + 2] == '<') {
                chtbuff = remt;
                chbuff += "!!<";
                i += 3;
            } else if (pro[i] == '<') {
                chtbuff = lowthant;
                chbuff += '<';
                i++;
            }else if (pro[i] == '>') {
                chtbuff = gretthant;
                chbuff += '>';
                i++;
            } else if (pro[i] == '@') {
                chtbuff = pot;
                chbuff += pro[i];
                i++;
            } else if (pro[i] == '+') {
                chtbuff = addt;
                chbuff += pro[i];
                i++;
            } else if (pro[i] == '-') {
                chtbuff = subt;
                chbuff += pro[i];
                i++;
            } else if (pro[i] == '*') {
                chtbuff = mult;
                chbuff += pro[i];
                i++;
            } else if (pro[i] == '/') {
                chtbuff = divit;
                chbuff += pro[i];
                i++;
            } else if (pro[i] == '=') {
                chtbuff = eqult;
                chbuff += pro[i];
                i++;
            } else if (pro[i] == '!') {
                chtbuff = nott;
                chbuff += pro[i];
                i++;
            } else if (pro[i] == '[') {
                chtbuff = bopt;
                chbuff += pro[i];
                i++;
            } else if (pro[i] == ']') {
                chtbuff = bclt;
                chbuff += pro[i];
                i++;
            } else if (pro[i] == '$') {
                chtbuff = fct;
                chbuff += pro[i];
                i++;
            } else if (pro[i] == '~') {
                chtbuff = whilet;
                chbuff += pro[i];
                i++;
            } else if (pro[i] == ',') {
                chtbuff = andt;
                chbuff += pro[i];
                i++;
            }else if (pro[i] == ';') {
                chtbuff = eolt;
                chbuff += pro[i];
                i++;
            }
        }
        token_val();
    }

}

mem_plot_type tokentype_to_plottype(low_token *token) {
    mem_plot_type pt ;
    if (token->type == intt) {
        pt =  INT;
    }
    else if (token->type == floatt) {
        pt =  FLOAT;
    }
    else if (token->type == text) {
        pt =  STRING;
    }
    else if (token->val == "true" || "false") {
        pt =   BOOL;
    }
    return pt;
}
void solve_binary_ex(low_statement_bi_ex *biex) {
    code_flow.push_back("hit bi ex");

    low_token_types op = biex->op;
    low_token_types bt = biex->type;
    std::string bval = biex->val;

    if (op == addt) {
        if (bt == intt) {
            plot_as_parse_int(low_point_val())->value = plot_as_parse_int(low_point_val())->value + stoi(bval);
        }else if (bt == floatt) {
            plot_as_parse_float(low_point_val())->value = plot_as_parse_float(low_point_val())->value + std::stof(bval);
        } else if (bt == text) {
            plot_as_parse_string(low_point_val())->value = plot_as_parse_string(low_point_val())->value + bval;
        }
    } else if (op == divit) {
        if (bt == intt) {
            plot_as_parse_int(low_point_val())->value = plot_as_parse_int(low_point_val())->value / stoi(bval);
        }else if (bt == floatt) {
            plot_as_parse_float(low_point_val())->value = plot_as_parse_float(low_point_val())->value / std::stof(bval);
        }
    } else if (op == mult) {
        if (bt == intt) {
            plot_as_parse_int(low_point_val())->value = plot_as_parse_int(low_point_val())->value * stoi(bval);
        }else if (bt == floatt) {
            plot_as_parse_float(low_point_val())->value = plot_as_parse_float(low_point_val())->value * std::stof(bval);
        }
    } else if (op == subt) {
        if (bt == intt) {
            plot_as_parse_int(low_point_val())->value = plot_as_parse_int(low_point_val())->value - stoi(bval);
        }else if (bt == floatt) {
            plot_as_parse_float(low_point_val())->value = plot_as_parse_float(low_point_val())->value - std::stof(bval);
        }
    }else if (op == mulmt) {
        if (bt == intt) {
            plot_as_parse_int(low_point_val())->value = plot_as_parse_int(low_point_val())->value + 1;
        }else if (bt == floatt) {
            plot_as_parse_float(low_point_val())->value = plot_as_parse_float(low_point_val())->value + 1;
        }
    } else if (op == eqult) {
        if (low_point_val()->get()->type == BOOL) {
            if (bval == "true") {
                plot_as_parse_bool(low_point_val())->value = true;
            }else if (bval == "false") {
                plot_as_parse_bool(low_point_val())->value = false;
            }
        } else if (low_point_val()->get()->type != BOOL) {
            if (bt == ident) {
                if (bt == intt) {
                    plot_as_parse_int(low_point_val())->value = plot_as_parse_int(low_point_val())->value = plot_as_parse_int(low_plot_val(bval))->value;
                }else if (bt == floatt) {
                    plot_as_parse_float(low_point_val())->value = plot_as_parse_float(low_point_val())->value = std::stof(bval);
                }else if (bt == text) {
                    plot_as_parse_string(low_point_val())->value = plot_as_parse_string(low_point_val())->value + plot_as_parse_string(low_plot_val(bval))->value;
                }
            }else if (bt == intt) {
                plot_as_parse_int(low_point_val())->value = plot_as_parse_int(low_point_val())->value = stoi(bval);
            }else if (bt == floatt) {
                plot_as_parse_float(low_point_val())->value = plot_as_parse_float(low_point_val())->value = std::stof(bval);
            }  else if (bt == text) {
                plot_as_parse_string(low_point_val())->value = plot_as_parse_string(low_point_val())->value = bval;
            }
        }
    }
}

bool solve_comparison(low_statement_comparison *com) {
    code_flow.push_back("hit compare");

    low_token_types op = com->op;
    low_token_types bt = com->type;
    bool bval;
    if (com->val == "true") {
        bval = true;
    }else if (com->val == "false") {
        bval = false;
    }


    if (op == eequalt) {
        if (bt == intt || bt == text || bt == keyt || bt == floatt) {
            if (low_point_val()->get()->type == BOOL) {
                return plot_as_parse_bool(low_point_val())->value == bval;

            } else if (low_point_val()->get()->type != BOOL) {
                plot_as_parse_bool(low_point_val())->value == bval;
            }
        }
    }else if (op == noteqt) {
        if (bt == intt || bt == text || bt == keyt) {
            if (low_point_val()->get()->type == BOOL) {
                return plot_as_parse_bool(low_point_val())->value != bval;

            }if (low_point_val()->get()->type != BOOL) {
                plot_as_parse_bool(low_point_val())->value != bval;
            }
        }
    }else if (op == gretthant) {
        if (bt == intt || bt == floatt) {
            if (low_point_val()->get()->type == INT) {
                return plot_as_parse_int(low_point_val())->value > stoi(com->val);
            }else if (low_point_val()->get()->type == FLOAT) {
                return plot_as_parse_float(low_point_val())->value > stof(com->val);
            }
        }
    }else if (op == lowthant) {
        if (bt == intt || bt == floatt) {
            if (low_point_val()->get()->type == INT) {
                return plot_as_parse_int(low_point_val())->value < stoi(com->val);
            }else if (low_point_val()->get()->type == FLOAT) {
                return plot_as_parse_float(low_point_val())->value < stof(com->val);
            }
        }
    }else if (op == gretthanoreqt) {
        if (bt == intt || bt == floatt) {
            if (low_point_val()->get()->type == INT) {
                return plot_as_parse_int(low_point_val())->value >= stoi(com->val);
            }else if (low_point_val()->get()->type == FLOAT) {
                return plot_as_parse_float(low_point_val())->value >= stof(com->val);
            }
        }
    }else if (op == lowthanoreqt) {
        if (bt == intt || bt == floatt) {
            if (low_point_val()->get()->type == INT) {
                return plot_as_parse_int(low_point_val())->value <= stoi(com->val);
            }else if (low_point_val()->get()->type == FLOAT) {
                return plot_as_parse_float(low_point_val())->value <= stof(com->val);
            }
        }
    }else {
        low_exit();
    }
    return false;
}

void Exicute(std::string fn) {
    low_function *function = &functions[fn];
    for (int i = 0; i < function->statements.size(); ++i) {
        std::unique_ptr<low_statement> &statement = function->statements[i];

        //def
        if (low_statement_def_plot *dn = dynamic_cast<low_statement_def_plot *>(statement.get())) {
            code_flow.push_back("hit def "+dn->iden);
            low_def(dn->iden, dn->val, dn->type);
        }

        //def struc
        if (low_statement_def_plot_struc *stn = dynamic_cast<low_statement_def_plot_struc *>(statement.get())) {
            code_flow.push_back("hit def struc");
            mem_plot_struc plot;
            plot.structypename = stn->structypename;
            plot.type = STRUCK;
            plot.index = stn->iden;
            low_struc_template* temp;
            if (struct_templates.contains(stn->structypename)) {
                temp = &struct_templates[stn->structypename];
            } else {
                std::cout << "struct not found !!!!!"<< stn->structypename << std::endl;
                low_exit();
            }
            if (stn->vals.size() == temp->patern.size()) {
                for (int i = 0; i < temp->patern.size(); ++i) {
                    std::string p = stn->iden+temp->structtypename+temp->patern[i].iden;
                    low_def(p,stn->vals[i]->val,temp->patern[i].type);
                    plot.struc[temp->patern[i].iden] = &mem_tape[p];
                }
            }
            mem_tape[stn->iden] = std::make_unique<mem_plot_struc>(plot);
        }

        //rem
        if (low_statement_rem *remn = dynamic_cast<low_statement_rem *>(statement.get())) {
            if (remn->func) {
                functions.erase(remn->ident);
                code_flow.push_back("hit rem func");
            } else if (!remn->func) {
                mem_tape.erase(remn->ident);
                code_flow.push_back("hit rem plot");
            }
        }

        //call func
        if (low_call_function *mc = dynamic_cast<low_call_function *>(statement.get())) {
            code_flow.push_back("hit function call");
            Exicute(mc->functionname);
        }

        //call func binded
        if (low_call_function_binded *fcw = dynamic_cast<low_call_function_binded *>(statement.get())) {
            code_flow.push_back("hit function call");

            std::vector< std::unique_ptr<mem_plot> *> inputs;
            for (auto input: fcw->inputs) {
                inputs.push_back(&mem_tape[input]);
            }

            functions_binded[fcw->functionname].func(inputs);
        }

        //if
        if (low_statement_if *ifn = dynamic_cast<low_statement_if *>(statement.get())) {
            code_flow.push_back("hit if");
            ifn->dynamiccom = dynamic_cast<low_statement_comparison *>(function->statements[++i].get());
            if (solve_comparison(ifn->dynamiccom) == ifn->expect) {
                Exicute(ifn->bodyfunc);
            }
            functions.erase(ifn->bodyfunc);
        }

        //while
        if (low_statement_while *whilen = dynamic_cast<low_statement_while *>(statement.get())) {
            code_flow.push_back("hit while");
            whilen->dynamiccom = dynamic_cast<low_statement_comparison *>(function->statements[++i].get());

            code_flow.push_back(std::to_string(solve_comparison(whilen->dynamiccom)));

            while (solve_comparison(whilen->dynamiccom) != true) {
                Exicute(whilen->bodyfunc);
            }

            code_flow.push_back("end while");

            functions.erase(whilen->bodyfunc);
        }

        //pointer point
        if (low_statement_pointer_point *pn = dynamic_cast<low_statement_pointer_point *>(statement.get())) {
            code_flow.push_back("hit pointer point");
            if (!pn->point_on_struc) {
                low_point(pn->point);
            }else if (pn->point_on_struc) {
                mem_plot_struc *plot = dynamic_cast<mem_plot_struc *>(low_plot_val(pn->point)->get());
                low_point(plot->struc[pn->passpoint]);
            }
        }

        //biex
        if (low_statement_bi_ex *biex = dynamic_cast<low_statement_bi_ex *>(statement.get())) {
            solve_binary_ex(biex);
        }

        //com
        if (low_statement_comparison *com = dynamic_cast<low_statement_comparison *>(statement.get())) {
            com->re = solve_comparison(com);
            code_flow.push_back("hit compare");

            if (com->returntob) {
                std::unique_ptr<mem_plot> *plo = low_plot_val(com->reboolname);
                if (plo->get()->type == BOOL) {
                    plot_as_parse_bool(low_point_val())->value = com->re;
                }
            }
        }

        //in
        if (low_statement_in *inn = dynamic_cast<low_statement_in *>(statement.get())) {
            mem_plot_string *inp = plot_as_parse_string(low_plot_val(inn->iden_to_return));

            std::string in ;
            std::cout << "[L] <=";
            std::cin >> in;
            inp->value+=in;
        }
        //out
        if (low_statement_out *outn = dynamic_cast<low_statement_out *>(statement.get())) {
            if (outn->valpoint) {
                std::cout << "[L] => " + plot_parse_as_struing_alltype(low_point_val()) << std::endl;
            } else if (!outn->valpoint) {
                if (outn->type == ident) {
                    std::cout << "[L] => " + plot_parse_as_struing_alltype( low_plot_val(outn->data)) << std::endl;
                } else {
                    std::cout << "[L] => " + outn->data << std::endl;
                }
            }
        }
    }
}

std::string meinfunc = "main";
std::string func = meinfunc;

void Eval() {
    //func safe
    if (meinfunc.empty()) {
        std::cout << "main func not found !!!";
        low_exit();
    }
    //start ex
    Exicute(meinfunc);

    //run debug
    if (debug)
        Trace_stat();
}
void Parse() {
    int parsei = 0;
    int pbi = 0;
    if (parsei > pbi) {

    }
    int rfi = 0;
    std::vector<std::string> rfunc;
    while (parsei < tokens.size()) {
        low_function *function = &functions[func];

        //function
        if (tokens[parsei].type == fct) {
            parsei++;
            if (tokens[parsei].type == pot) {
                parsei++;
                if (tokens[parsei].type == ident) {
                    code_flow.push_back("set func main " + tokens[parsei].val);
                    meinfunc = tokens[parsei].val;
                    func = meinfunc;
                    rfunc.push_back(func);
                    rfi++;
                }
            } else if (tokens[parsei].type == ident) {
                if (tokens[parsei + 1].type == bopt) {
                    rfunc.push_back(func);
                    rfi++;
                    func = tokens[parsei].val;
                    code_flow.push_back("set func " + tokens[parsei].val);
                } else if (tokens[parsei + 1].type == eolt) {
                    code_flow.push_back("add  call funk in funk " + func);

                    low_call_function mcall;
                    mcall.functionname = tokens[parsei].val;
                    function->statements.push_back(std::make_unique<low_call_function>(mcall));
                    pbi++;
                } else if (tokens[parsei + 1].type == passlt) {
                    code_flow.push_back("add call binded funk in funk " + func);

                    low_call_function_binded wfcall;
                    wfcall.functionname = tokens[parsei].val;

                    parsei++;

                    while (tokens[++parsei].type == ident) {
                        wfcall.inputs.push_back(tokens[parsei].val);
                    }
                    if (tokens[parsei].type == eolt) {
                        function->statements.push_back(std::make_unique<low_call_function_binded>(wfcall));
                        pbi++;
                    } else {
                        low_exit();
                    }
                }
            } else {
                low_exit();
            }
        }
        if (tokens[parsei].type == bclt && func != meinfunc) {
            func = rfunc[--rfi];
            code_flow.push_back("point function " + func);
        }

        //index struct
        if (tokens[parsei].type == structt && tokens[parsei+2].type == bopt) {
            low_struc_template temp;
            if (tokens[parsei+1].type == ident) {
                temp.structtypename = tokens[++parsei].val;
                code_flow.push_back("assain struct " + temp.structtypename );
                if (tokens[++parsei].type == bopt) {
                        code_flow.push_back("assain patern " + temp.structtypename );
                    parsei++;
                    while (tokens[parsei].type != bclt) {
                        code_flow.push_back("assain in patern " + tokens[parsei].val);
                        low_struct_patern pt;
                        pt.iden = tokens[parsei].val;
                        pt.type = tokentype_to_plottype(&tokens[++parsei]);
                        pt.val = tokens[parsei].val;
                        temp.patern.push_back(pt);
                        if (tokens[++parsei].type == andt) {
                            parsei++;
                        }
                    }

                    struct_templates[temp.structtypename] = temp;
                }
            }
        }

        //if func
        if (tokens[parsei].type == ift) {
            code_flow.push_back("add if in funk " + func);

            low_statement_if ifn;
            if (tokens[parsei+1].type == nott) {
                ifn.expect = false;
                parsei ++;
            }
            int subi = parsei;
            while (tokens[subi].type != bopt) { subi++; }
            if (tokens[subi].type == bopt) {
                ifn.bodyfunc = "if_body_" + std::to_string(pbi);
                function->statements.push_back(std::make_unique<low_statement_if>(ifn));

                rfunc.push_back(func);
                rfi++;
                pbi++;
                func = ifn.bodyfunc;
                code_flow.push_back("set func " + func);
            } else {
                low_exit();
            }
        }

        //while
        if (tokens[parsei].type == whilet) {
            code_flow.push_back("add while in funk " + func);

            low_statement_while whilen;
            if (tokens[parsei+1].type == nott) {
                whilen.expect = false;
                parsei ++;
            }
            int subi = parsei;
            while (tokens[subi].type != bopt) { subi++; }

            if (tokens[subi].type == bopt) {
                whilen.bodyfunc = "while_body_" + std::to_string(pbi);
                function->statements.push_back(std::make_unique<low_statement_while>(whilen));

                rfunc.push_back(func);
                rfi++;
                pbi++;
                func = whilen.bodyfunc;
                code_flow.push_back("set func " + func);
            } else {
                low_exit();
            }
        }

        //point
        if (tokens[parsei].type == pot) {
            code_flow.push_back("add  point funk " + func);
            parsei++;
            low_statement_pointer_point npp;
            if (tokens[parsei].type == ident) {
                npp.point = tokens[parsei].val;
                function->statements.push_back(std::make_unique<low_statement_pointer_point>(npp));
                pbi++;
            } else if (tokens[parsei].type == structt) {
                if (tokens[++parsei].type == ident) {
                    npp.point = tokens[parsei].val;
                    if (tokens[++parsei].type == passrt) {
                        npp.point_on_struc = true;
                        npp.passpoint = tokens[++parsei].val;
                    }
                    function->statements.push_back(std::make_unique<low_statement_pointer_point>(npp));
                    pbi++;
                }
            } else {
                low_exit();
            }
        }
        if (tokens[parsei].type == inpt) {

            low_statement_in stin;

            stin.iden_to_return = tokens[++parsei].val;

            if (tokens[++parsei].type == eolt) {
                function->statements.push_back(std::make_unique<low_statement_in>(stin));
                pbi++;
            } else {
                low_exit();
            }
        }

        //out
        if (tokens[parsei].type == outt) {
            code_flow.push_back("add  out funk " + func);

            parsei++;
            low_statement_out no;
            no.type = tokens[parsei].type;
            if (tokens[parsei].type == pot) {
                no.valpoint = true;
            } else if (tokens[parsei].type == ident || tokens[parsei].type == text || tokens[parsei].type == intt|| tokens[parsei].type == floatt) {
                no.data = tokens[parsei].val;
            } else {
                low_exit();
            }

            if (tokens[++parsei].type == eolt) {
                function->statements.push_back(std::make_unique<low_statement_out>(no));
                pbi++;
            } else {
                low_exit();
            }
        }

        //def
        if (tokens[parsei].type == deft) {
            parsei++;
            if (tokens[parsei].type == ident&&tokens[parsei+2].type == eolt) {

                code_flow.push_back("add  def in funk " + func);


                low_statement_def_plot ndef;

                ndef.iden = tokens[parsei].val;
                ndef.val = tokens[++parsei].val;

                ndef.type = tokentype_to_plottype(&tokens[parsei]);

                if (tokens[++parsei].type == eolt) {
                    function->statements.push_back(std::make_unique<low_statement_def_plot>(ndef));
                    pbi++;
                } else {
                    low_exit();
                }
            }
            else if (tokens[parsei].type == ident&&tokens[parsei+1].type == passrt) {
                code_flow.push_back("add struc def in funk " + func);

                low_statement_def_plot_struc stdf;
                stdf.iden = tokens[parsei].val;
                if (tokens[++parsei].type == passrt) {
                    stdf.structypename = tokens[++parsei].val;
                    code_flow.push_back("add def struc " + stdf.structypename);
                    if (tokens[++parsei].type == bopt) {
                        parsei++;
                        while (tokens[parsei].type != bclt) {
                            stdf.vals.push_back(&tokens[parsei]);

                            code_flow.push_back("pass for struc " + tokens[parsei].val);

                            if (tokens[++parsei].type == andt) {
                                parsei++;
                            }
                        }
                        if (tokens[++parsei].type == eolt) {
                            function->statements.push_back(std::make_unique<low_statement_def_plot_struc>(stdf));
                            pbi++;
                        } else {
                            low_exit();
                        }
                    }
                }
            }
        }

        //remove things
        if (tokens[parsei].type == remt) {
            code_flow.push_back("add bi rem in funk " + func);
            low_statement_rem remn;
            parsei++;
            if (tokens[parsei].type == fct) {
                remn.func = true;
                parsei++;
            }

            if (tokens[parsei].type == ident) {
                remn.ident = tokens[parsei].val;
            } else {
                low_exit();
            }

            if (tokens[++parsei].type == eolt) {
                function->statements.push_back(std::make_unique<low_statement_rem>(remn));
                pbi++;
            } else {
                low_exit();
            }
        }
        //bi ex
        if (tokens[parsei].type == addt ||
            tokens[parsei].type == divit ||
            tokens[parsei].type == mult ||
            tokens[parsei].type == subt ||
            tokens[parsei].type == eqult||
            tokens[parsei].type == mulmt
        ) {
            code_flow.push_back("add bi ex in funk " + func);

            low_statement_bi_ex nex;
            nex.op = tokens[parsei].type;
            nex.val = tokens[++parsei].val;
            nex.type = tokens[parsei].type;

            function->statements.push_back(std::make_unique<low_statement_bi_ex>(nex));
            pbi++;
        }


        //com
        if (
            tokens[parsei].type == eequalt||
            tokens[parsei].type == noteqt||
            tokens[parsei].type == gretthant||
            tokens[parsei].type == gretthanoreqt||
            tokens[parsei].type == lowthant||
            tokens[parsei].type == lowthanoreqt
        ) {
            low_statement_comparison ncom;

            ncom.op = tokens[parsei].type;
            ncom.val = tokens[++parsei].val;
            ncom.type = tokens[parsei].type;
            parsei++;
            if (tokens[parsei].type == rett) {
                if (tokens[++parsei].type == ident) {
                    ncom.reboolname = tokens[parsei].val;
                    ncom.returntob = true;
                } else {
                    low_exit();
                }
                function->statements.push_back(std::make_unique<low_statement_comparison>(ncom));
                pbi++;
            } else if (tokens[parsei].type == passrt) {
                parsei++;
                if (tokens[parsei].type == bopt) {
                    std::string rf = rfunc[rfi - 1];
                    code_flow.push_back("add comparison in funk dinamic " + rf);
                    functions[rf].statements.push_back(std::make_unique<low_statement_comparison>(ncom));
                    pbi++;
                } else {
                    low_exit();
                }
            } else {
                low_exit();
            }
        }
        parsei++;
    }
}
void Trace_stat() {
    std::cout << " " << std::endl;
    std::cout << " " << std::endl;
    std::cout << "===+++++++++++++++++++++++===" << std::endl;
    std::cout << "   [ low_lang alpha ]" << std::endl;
    std::cout << "===+++++++++++++++++++++++===" << std::endl;
    std::cout << "        " << std::endl;
    std::cout << "        " << std::endl;
    std::cout << "-====== program ======-" << std::endl;
    std::cout << code_on_exi << std::endl;
    std::cout << "        " << std::endl;
    std::cout << "-====== tokens ======-" << std::endl;
    for (auto t: tokens) {
        std::cout << "[" << t.type << " | " << t.val << "]" << std::endl;
    }

    std::cout << "        " << std::endl;
    std::cout << "-====== flow ======-" << std::endl;
    for (auto step: code_flow) {
        std::cout << "[" + step + "]" << std::endl;
        std::cout << "   |.|" << std::endl;
    }
    std::cout << "[end]" << std::endl;
    std::cout << " " << std::endl;

    std::cout << "        " << std::endl;
    std::cout << "-====== mem_tape ======-" << std::endl;
    for (auto & pair: mem_tape) {
        std::cout << pair.second.get()->index << " ["
                << std::to_string(pair.second.get()->type) << " | "
                << plot_parse_as_struing_alltype(low_plot_val(pair.first)) << "]" << std::endl;
    }
    std::cout << "===+++++++++++++++++++++++==="

    << std::endl;std::cout << "-====== struc ======-" << std::endl;
    for (auto & pair: struct_templates) {
        std::cout <<pair.first<<" => ";
        for (auto s: pair.second.patern) {
            std::cout << "["<<s.iden<<" | " << s.val<<" | "<< s.type<< "]";
        }
        std::cout << "]"<<std::endl;
    }
    std::cout << "===+++++++++++++++++++++++===" << std::endl;

}
std::vector<Low_Bind*> Binds;
void Register_Bind(Low_Bind* b) {
    Binds.push_back(b);
}
void Binder_load() {
    for (auto bind: Binds) {
        bind->Bind();
    }
}
void Exicute_lang(std::string pro) {
    chbuff.clear();
    tokens.clear();
    functions.clear();
    functions_binded.clear();

    Binder_load();
    Tokenizer(pro);
    Parse();
    Eval();
}
