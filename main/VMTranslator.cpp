#include <string>
#include <sstream>
#include "VMTranslator.h"

using namespace std;

/**
 * VMTranslator constructor
 */
VMTranslator::VMTranslator() {
    // Your code here
    ss.str(string());
    name = "TheModule";
    symbolindex = 0;
    table.clear();
    table.insert(pair<string, VMsegments>("constant", seg_constant));
    table.insert(pair<string, VMsegments>("static", seg_static));
    table.insert(pair<string, VMsegments>("temp", seg_temp));
    table.insert(pair<string, VMsegments>("pointer", seg_pointer));
    table.insert(pair<string, VMsegments>("local", seg_local));
    table.insert(pair<string, VMsegments>("argument", seg_argument));
    table.insert(pair<string, VMsegments>("this", seg_this));
    table.insert(pair<string, VMsegments>("that", seg_that));
}

/**
 * VMTranslator destructor
 */
VMTranslator::~VMTranslator() {
    // Your code here
    table.clear();
}

void VMTranslator::Translate(string vmCode){
    if(vmCode.find("(")==string::npos){
        ss << "\t";
    }
    ss << vmCode << endl;
}

string VMTranslator::RegisterTranslate(string segment, int index) {
    if (segment == "constant") return to_string(index);
    if (segment == "static") return to_string(16 + index);
    if (segment == "local") return "LCL";
    if (segment == "argument") return "ARG";
    if (segment == "this") return "THIS";
    if (segment == "that") return "THAT";
    if (segment == "pointer") return "R" + to_string(3 + index);
    if (segment == "temp") return "R" + to_string(5 + index);

    return name + "." + to_string(index);  // else it is static
}

/** Generate Hack Assembly code for a VM push operation */
string VMTranslator::vm_push(string segment, int offset){
    ss.str(string());
    string indextostr = to_string(offset); 
    string RegisterASM = RegisterTranslate(segment, offset);
    switch (table[segment]) {
        case seg_constant:
            Translate("@" + indextostr);
            Translate("D=A");
            Translate("@SP");
            Translate("AM=M+1");
            Translate("A=A-1");
            Translate("M=D");
            break;
        case seg_argument:
        case seg_local:
        case seg_this:
        case seg_that:
            Translate("@" + RegisterASM);
            Translate("D=M");
            Translate("@" + indextostr);
            Translate("A=D+A");
            Translate("D=M");
            Translate("@SP");
            Translate("A=M");
            Translate("M=D");
            Translate("@SP");
            Translate("M=M+1");
            break;
        case seg_static:
        case seg_temp:
        case seg_pointer:
            Translate("@" + RegisterASM);
            Translate("D=M");
            Translate("@SP");
            Translate("A=M");
            Translate("M=D");
            Translate("@SP");
            Translate("M=M+1");
            break;
        default:
            return ss.str();
    }
    return ss.str();
}

/** Generate Hack Assembly code for a VM pop operation */
string VMTranslator::vm_pop(string segment, int offset){    
    ss.str(string());
    string indextostr = to_string(offset);
    string RegisterASM = RegisterTranslate(segment, offset);
    switch (table[segment]) {
        case seg_constant:
            break;
        case seg_argument:
        case seg_local:
        case seg_this:
        case seg_that:
            Translate("@SP");
            Translate("AM=M-1");
            Translate("D=M");
            Translate("@" + RegisterASM);
            Translate("A=M");
            for(int i = 0;i<offset;i++){
                Translate("A=A+1");
            }
            Translate("M=D");
            break;
        case seg_static:
        case seg_temp:
        case seg_pointer:
            Translate("@SP");
            Translate("AM=M-1");
            Translate("D=M");
            Translate("@" + RegisterASM);
            Translate("M=D");
            break;
        default:
            return ss.str();
    }
    return ss.str();
}

/** Generate Hack Assembly code for a VM add operation */
string VMTranslator::vm_add() {
    ss.str(string());
    Translate("@SP // add");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("A=A-1");
    Translate("M=D+M");
    return ss.str() + "\n";
}
/** Generate Hack Assembly code for a VM sub operation */
string VMTranslator::vm_sub(){
    ss.str(string());
    Translate("@SP // sub");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("A=A-1");
    Translate("M=M-D");
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM neg operation */
string VMTranslator::vm_neg(){
    ss.str(string());
    Translate("@SP // neg");
    Translate("A=M");
    Translate("A=A-1");
    Translate("M=-M");
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM eq operation */
string VMTranslator::vm_eq(){
    ss.str(string());
    string iftrue("JGT_TRUE_" + name + "_" + to_string(symbolindex));
    string iffalse("JGT_FALSE_" + name + "_" + to_string(symbolindex));

    Translate("@SP // eq");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("@SP");
    Translate("AM=M-1");
    Translate("D=M-D");
    Translate("@" + iftrue);
    Translate("D;JEQ");
    Translate("D=0");
    Translate("@" + iffalse);
    Translate("0;JMP");
    Translate("(" + iftrue + ")");
    Translate("D=-1");
    Translate("(" + iffalse + ")");
    Translate("@SP");
    Translate("A=M");
    Translate("M=D");
    Translate("@SP");
    Translate("M=M+1");

    symbolindex++;
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM gt operation */
string VMTranslator::vm_gt(){
    ss.str(string());
    string iftrue("JGT_TRUE_" + name + "_" + to_string(symbolindex));
    string iffalse("JGT_FALSE_" + name + "_" + to_string(symbolindex));

    Translate("@SP // gt");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("@SP");
    Translate("AM=M-1");
    Translate("D=M-D");
    Translate("@" + iftrue);
    Translate("D;JGT");
    Translate("D=0");
    Translate("@" + iffalse);
    Translate("0;JMP");
    Translate("(" + iftrue + ")");
    Translate("D=-1");
    Translate("(" + iffalse + ")");
    Translate("@SP");
    Translate("A=M");
    Translate("M=D");
    Translate("@SP");
    Translate("M=M+1");

    symbolindex++;
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM lt operation */
string VMTranslator::vm_lt(){
    ss.str(string());
    string iftrue("JLT_TRUE_" + name + "_" + to_string(symbolindex));
    string iffalse("JLT_FALSE_" + name + "_" + to_string(symbolindex));

    Translate("@SP // lt");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("@SP");
    Translate("AM=M-1");
    Translate("D=M-D");
    Translate("@" + iftrue);
    Translate("D;JLT");
    Translate("D=0");
    Translate("@" + iffalse);
    Translate("0;JMP");
    Translate("(" + iftrue + ")");
    Translate("D=-1");
    Translate("(" + iffalse + ")");
    Translate("@SP");
    Translate("A=M");
    Translate("M=D");
    Translate("@SP");
    Translate("M=M+1");

    symbolindex++;
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM and operation */
string VMTranslator::vm_and(){
    ss.str(string());
    Translate("@SP // and");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("A=A-1");
    Translate("M=D&M");
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM or operation */
string VMTranslator::vm_or(){
    ss.str(string());
    Translate("@SP // or");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("A=A-1");
    Translate("M=D|M");
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM not operation */
string VMTranslator::vm_not(){
    ss.str(string());
    Translate("@SP // not");
    Translate("A=M");
    Translate("A=A-1");
    Translate("M=!M");
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM label operation */
string VMTranslator::vm_label(string label){
    ss.str(string());
    Translate("("+label+")");
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM goto operation */
string VMTranslator::vm_goto(string label){
    ss.str(string());
    Translate("@"+label);
    Translate("0;JMP");
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM if-goto operation */
string VMTranslator::vm_if(string label){
    ss.str(string());
    Translate("@SP");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("@"+label);
    Translate("D;JNE");
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM function operation */
string VMTranslator::vm_function(string function_name, int n_vars){
    ss.str(string());
    Translate("("+function_name+")");
    for(int i = 0;i<n_vars;i++){
            Translate("@0");
            Translate("D=A");
            Translate("@SP");
            Translate("AM=M+1");
            Translate("A=A-1");
            Translate("M=D");
    }
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM call operation */
string VMTranslator::vm_call(string function_name, int n_args){
    ss.str(string());
    Translate("@return_address");
    Translate("D=A");
    Translate("@SP");
    Translate("AM=M+1");
    Translate("A=A-1");
    Translate("M=D");
    Translate("@LCL");
    Translate("D=M");
    Translate("@SP");
    Translate("AM=M+1");
    Translate("A=A-1");
    Translate("M=D");
    Translate("@ARG");
    Translate("D=M");
    Translate("@SP");
    Translate("AM=M+1");
    Translate("A=A-1");
    Translate("M=D");
    Translate("@THIS");
    Translate("D=M");
    Translate("@SP");
    Translate("AM=M+1");
    Translate("A=A-1");
    Translate("M=D");
    Translate("@THAT");
    Translate("D=M");
    Translate("@SP");
    Translate("AM=M+1");
    Translate("A=A-1");
    Translate("M=D");
    Translate("@SP");
    for(int i = 0;i<5+n_args;i++){
        Translate("M=M-1");
    }
    Translate("D=M");
    Translate("@ARG");
    Translate("M=D");
    Translate("@SP");
    for(int i = 0;i<5+n_args;i++){
        Translate("M=M+1");
    }
    Translate("D=M");
    Translate("@LCL");
    Translate("M=D");
    Translate("@"+function_name);
    Translate("0;JMP");
    Translate("(return_address)");
    return ss.str() + "\n";
}

/** Generate Hack Assembly code for a VM return operation */
string VMTranslator::vm_return(){
    ss.str(string());
    Translate("@LCL");
    Translate("D=M");
    Translate("@FRAME");
    Translate("M=D");
    Translate("@return_address");
    Translate("M=D");
    Translate("@5");
    Translate("D=A");
    Translate("@return_address");
    Translate("M=M-D");
    Translate("@SP");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("@ARG");
    Translate("A=M");
    Translate("M=D");
    Translate("@ARG");
    Translate("D=M");
    Translate("@SP");
    Translate("M=D+1");
    Translate("@FRAME");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("@THAT");
    Translate("M=D");
    Translate("@FRAME");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("@THIS");
    Translate("M=D");
    Translate("@FRAME");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("@ARG");
    Translate("M=D");
    Translate("@FRAME");
    Translate("AM=M-1");
    Translate("D=M");
    Translate("@LCL");
    Translate("M=D");
    Translate("@return_address");
    Translate("0;JMP");
    return ss.str() + "\n";
}