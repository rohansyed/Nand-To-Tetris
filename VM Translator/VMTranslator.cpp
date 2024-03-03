#include "VMTranslator.h"

#include <string>

using namespace std;

/**
 * VMTranslator constructor
 */
VMTranslator::VMTranslator() {
  // Your code here
}

/**
 * VMTranslator destructor
 */
VMTranslator::~VMTranslator() {
  // Your code here
}

/** Generate Hack Assembly code for a VM push operation */
string VMTranslator::vm_push(string segment, int offset) {
  // initialising variables
  string vmCode;
  string label;
  int baseAddress;

  // if constant, loads value into A-register
  if (segment == "constant") {
    vmCode = "@" + to_string(offset) + "\n" + "D=A\n";

    // if this,that,local,argument segments, sets label to respective symbol
  } else if (segment == "this" || segment == "that" || segment == "argument" ||
             segment == "local") {
    if (segment == "local") {
      label = "LCL";
    } else if (segment == "this") {
      label = "THIS";
    } else if (segment == "argument") {
      label = "ARG";
    } else if (segment == "that") {
      label = "THAT";
    }

    // setting vmCode to calculation of base address & offset address
    vmCode = "@" + to_string(offset) + "\n" + "D=A\n" + "@" + label + "\n" +
             "A=M+D\n" + "D=M\n";

    // if pointer,temp segments, sets base address to location in stack
  } else if (segment == "temp" || segment == "pointer") {
    if (segment == "temp") {
      baseAddress = 5;  // temp segment location in stack
    } else {
      baseAddress = 3;  // pointer segment location in stack
    }

    // correcting vmCode
    vmCode = "@" + to_string(baseAddress + offset) + "\n" + "D=M\n";
  }

  // if static segment, sets vmCode to offset
  else if (segment == "static") {
    vmCode = "@static." + to_string(offset) + "\n" + "D=M\n";
  }

  // pushes data (D-reg) to stack
  vmCode += "@SP\n";
  vmCode += "A=M\n";
  vmCode += "M=D\n";
  vmCode += "@SP\n";
  vmCode += "M=M+1\n";

  return vmCode;
}

/** Generate Hack Assembly code for a VM pop operation */
string VMTranslator::vm_pop(string segment, int offset) {
  // initialising variables
  string vmCode;
  string label;
  int baseAddress;

  // if this,that,local,argument segments, sets label to respective symbol
  if (segment == "this" || segment == "that" || segment == "argument" ||
      segment == "local") {
    if (segment == "local") {
      label = "LCL";
    } else if (segment == "this") {
      label = "THIS";
    } else if (segment == "argument") {
      label = "ARG";
    } else if (segment == "that") {
      label = "THAT";
    }

    // finding the intended memory address & offset value
    vmCode = "@" + to_string(offset) + "\n" + "D=A\n" + "@" + label + "\n" +
             "D=M+D\n" + "@R13\n" + "M=D\n" + "@SP\n" + "AM=M-1\n" + "D=M\n" +
             "@R13\n" + "A=M\n" + "M=D\n";
  }

  // if static segment, sets vmCode
  else if (segment == "static") {
    vmCode = "@SP\n";
    vmCode += "AM=M-1\n";
    vmCode += "D=M\n";
    vmCode += "@static." + to_string(offset) + "\n";
    vmCode += "M=D\n";

    // if pointer,temp segments, sets base address to location in stack
  } else if (segment == "temp" || segment == "pointer") {
    if (segment == "temp") {
      baseAddress = 5;  // temp segment location in stack
    } else {
      baseAddress = 3;  // pointer segment location in stack
    }

    vmCode = "@SP\n";
    vmCode += "AM=M-1\n";
    vmCode += "D=M\n";
    vmCode += "@" + to_string(baseAddress + offset) + "\n";
    vmCode += "M=D\n";
  }

  return vmCode;
}

/** Generate Hack Assembly code for a VM add operation */
string VMTranslator::vm_add() {
  // initialsing variable
  string addCode;

  // loads stack pointer, decrements it and sets address to top of stack, then
  // stores the second top-most value in D-reg, then gets address of top-most
  // value in stack and adds the second top-most value to this first value.
  // stores result at top-most value space
  addCode += "@SP\n";
  addCode += "AM=M-1\n";
  addCode += "D=M\n";
  addCode += "A=A-1\n";
  addCode += "M=M+D\n";

  return addCode;
}

/** Generate Hack Assembly code for a VM sub operation */
string VMTranslator::vm_sub() { return ""; }

/** Generate Hack Assembly code for a VM neg operation */
string VMTranslator::vm_neg() { return ""; }

/** Generate Hack Assembly code for a VM eq operation */
string VMTranslator::vm_eq() { return ""; }

/** Generate Hack Assembly code for a VM gt operation */
string VMTranslator::vm_gt() { return ""; }

/** Generate Hack Assembly code for a VM lt operation */
string VMTranslator::vm_lt() { return ""; }

/** Generate Hack Assembly code for a VM and operation */
string VMTranslator::vm_and() {
  // initialsing variable
  string andCode;

  // loads stack pointer, decrements it and sets address to top of stack, then
  // stores the second top-most value in D-reg, then gets address of top-most
  // value in stack and then uses AND operation on the values (Hack Assembly)
  andCode += "@SP\n";
  andCode += "AM=M-1\n";
  andCode += "D=M\n";
  andCode += "A=A-1\n";
  andCode += "M=D&M\n";

  return andCode;
}

/** Generate Hack Assembly code for a VM or operation */
string VMTranslator::vm_or() { return ""; }

/** Generate Hack Assembly code for a VM not operation */
string VMTranslator::vm_not() {
  // initialsing variable
  string notCode;

  // loads stack pointer, sets address to top of memory, then applys NOT/INVERT
  // operation on the value currently at top of stack
  notCode += "@SP\n";
  notCode += "A=M-1\n";
  notCode += "M=!M\n";

  return notCode;
}

/** Generate Hack Assembly code for a VM label operation */
string VMTranslator::vm_label(string label) { return "(" + label + ")\n"; }

/** Generate Hack Assembly code for a VM goto operation */
string VMTranslator::vm_goto(string label) { return "@" + label + "\n0;JMP"; }

/** Generate Hack Assembly code for a VM if-goto operation */
string VMTranslator::vm_if(string label) {
  // hack assembly code for go-to operation
  string a = "@SP\n";
  string b = "AM=M-1\n";
  string c = "D=M\n";
  string d = "@";
  string e = "\n";
  string f = "D;JNE";
  return a + b + c + d + label + e + f;
}

/** Generate Hack Assembly code for a VM function operation */
string VMTranslator::vm_function(string function_name, int n_vars) {}

/** Generate Hack Assembly code for a VM call operation */
string VMTranslator::vm_call(string function_name, int n_args) {}

/** Generate Hack Assembly code for a VM return operation */
string VMTranslator::vm_return() {}