#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include <string>
#include <map>
#include <typeinfo>
#include <iostream>
#include <fstream>

using namespace llvm;
using namespace std;

#define DEBUG_TYPE "ValueNumbering"

using namespace llvm;

namespace {
struct ValueNumbering : public FunctionPass {
  string func_name = "test";
  static char ID;
  ValueNumbering() : FunctionPass(ID) {}
  int VN = 1;
  std::map<Value*, int>HashTable;
  std::map<std::string, int>HashTable_instr;
  
  int addOrFindHashTable(Value*op,bool *found){
    auto search = HashTable.find(op);
    auto retVN = VN;
    if(search!=HashTable.end()){
      *found = true;
      retVN = search->second;
    }
    else{
      *found = false;
      HashTable.insert(std::pair<Value*,int>(op,VN));
      VN++;
    }
    //print out the hash table
    // for (auto it = HashTable.begin(); it != HashTable.end(); it++) {
    //     errs()<< "{" << it->first << ": " << it->second << "}\n";
    // }
    
    return retVN;

  }

  int addOrFindInstrHashTable(Value*op3,std::string *op,std::string *op1,bool *found)
  {
    auto search = HashTable_instr.find(*op);
    auto retVN = VN;
    if(search!=HashTable_instr.end()){
      *found = true;
      retVN = search->second;
      HashTable.insert(std::pair<Value*,int>(op3,retVN));
    }
    else{
      *found = false;
      VN++;
      HashTable.insert(std::pair<Value*,int>(op3,retVN));
      HashTable_instr.insert(std::pair<std::string,int>(*op,retVN));
      HashTable_instr.insert(std::pair<std::string,int>(*op1,retVN));
    }
    
    return retVN;

  }


  bool runOnFunction(Function &F) override {
    Value *op1;
    int op_1_vn;
    Value *op2;
    int op_2_vn;
    Value *op3;
    int op_3_vn;
    std::string *operation = new std::string;
    std::string *expression = new std::string;
    std::string *expression1 = new std::string;

    std::string *instr = new std::string;
    HashTable.clear();
    errs() << "ValueNumbering: ";
    errs() << F.getName() << "\n";
        if (F.getName() != func_name) return false;

        for (auto& basic_block : F)
        {

            for (auto& inst : basic_block)
            {
            	errs() << inst << "\n";
              if(inst.getOpcode() == Instruction::Load){
                      errs() << "This is Load"<<"\n";
              }
              if(inst.getOpcode() == Instruction::Store){
                      errs() << "This is Store"<<"\n";
              }
                if (inst.isBinaryOp())
                {
                    errs() << "Op Code:" << inst.getOpcodeName()<<"\n"; // print opcode name
                    if(inst.getOpcode() == Instruction::Add){
                      // errs() << "This is Addition"<<"\n";
                      *operation = "+";
                    }
                    if(inst.getOpcode() == Instruction::Sub){
                      // errs() << "This is Subtraction"<<"\n";
                      *operation = "-";
                    }
                    if(inst.getOpcode() == Instruction::Mul){
                      // errs() << "This is Multiplication"<<"\n";
                      *operation = "*";
                    }
                    if(inst.getOpcode() == Instruction::UDiv){
                      // errs() << "This is Division"<<"\n";
                      *operation = "/";
                    }
                    
                    // See Other classes, Instruction::Sub, Instruction::UDiv, Instruction::SDiv
                //	errs() << "Operand(0)" << (*inst.getOperand(0))<<"\n";
                    auto* ptr = dyn_cast<User>(&inst);
                    auto* val = dyn_cast<Value>(&inst);

                    for(int i =0;i<=1;i++)
                    {
                      if(i==0){
                        op1=ptr->getOperand(0);

                      }
                      if(i==1){
                        op2=ptr->getOperand(1);

                      }

                    }
                    auto* op3 = dyn_cast<Value>(&inst);

                    bool *found = new bool(false);
                    op_1_vn = addOrFindHashTable(op1,found);
                    op_2_vn = addOrFindHashTable(op2,found);
                    
                    *expression = std::to_string(op_1_vn) + *operation + std::to_string(op_2_vn);
                    *expression1 = std::to_string(op_2_vn) + *operation + std::to_string(op_1_vn);

                    op_3_vn = addOrFindInstrHashTable(op3,expression,expression1,found);
                    

                    std::string instr_ = std::to_string(op_3_vn) + "=" + std::to_string(op_1_vn)+ *operation+std::to_string(op_2_vn);
                    *instr = *instr + instr_ + "\n";
                    
                    

                }
            }
             errs() << "instructions: \n" << *instr;
            //  ofstream myfile;
            //  myfile.open ("output.out");
            //  myfile << *instr;
            //  myfile.close();
            std::string str1 = F.getParent()->getSourceFileName();            
            std::string str(str1,0,1);
            str = str +".out";

            ofstream myfile;
            myfile.open (str);
            myfile << *instr;
            myfile.close();



        }
    return false;
  }
}; // end of struct ValueNumbering
}  // end of anonymous namespace

char ValueNumbering::ID = 0;
static RegisterPass<ValueNumbering> X("ValueNumbering", "ValueNumbering Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);