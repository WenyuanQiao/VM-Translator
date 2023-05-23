#ifndef VMTRANSLATOR_H
#define VMTRANSLATOR_H
#include <sstream>
#include <string>
#include <map>
using namespace std;

class VMTranslator {
    private:
        void Translate(string vmCode);
        string RegisterTranslate(string segment,int index);
        int symbolindex;
        enum VMsegments{
            seg_constant,
            seg_static,
            seg_temp,
            seg_pointer,
            seg_local,
            seg_argument,
            seg_this,
            seg_that};
        map<string,VMsegments> table;
        stringstream ss;
        string name;

    public:

        VMTranslator();
        ~VMTranslator();

        string vm_push(string segment, int offset);
        string vm_pop(string segment, int offset);

        string vm_add();
        string vm_sub();
        string vm_neg();
        string vm_eq();
        string vm_gt();
        string vm_lt();
        string vm_and();
        string vm_or();
        string vm_not();

    /** Project 08 functions */
        string vm_label(string label);
        string vm_goto(string label);
        string vm_if(string label);
        string vm_function(string function_name, int n_vars);
        string vm_call(string function_name, int n_args);
        string vm_return();

        
};

#endif /* VMTRANSLATOR_H */