#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include <string>
#include <unordered_map>
#include "llvm/IR/CFG.h"

using namespace llvm;
using namespace std;

#define DEBUG_TYPE "ValueNumbering"

using namespace llvm;

namespace {
struct ValueNumbering : public FunctionPass {
  string func_name = "test";
  static char ID;
  ValueNumbering() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    errs() << "ValueNumbering: ";
    errs() << F.getName() << "\n";
    if (F.getName() != func_name) return false;

    unordered_map<string, std::vector<string>> Succ;
    unordered_map<string, std::vector<string>> Uevar;
    unordered_map<string, std::vector<string>> Varkill;
    unordered_map<string, std::vector<string>> Liveout;

    //get the successor of each basic block to store the relationships of each bb;
    //initial every liveout to be {} (null);
    for (auto& basic_block : F)
    {
        string name = basic_block.getName();
        std::vector<string> succ;
        std::vector<string> liveout;
        for (BasicBlock *Suc : successors(&basic_block)) {
            string succname = Suc->getName();
            succ.push_back(succname);
        }
        sort(succ.begin(), succ.end());
        pair<string, std::vector<string>> p(name, succ);
        pair<string, std::vector<string>> p1(name, liveout);
        Succ.insert(p);
        Liveout.insert(p1);
    }

    //get uevar and varkill of each basic block;
    std::vector<string> n;
    for (auto& basic_block : F)
    {
        string name = basic_block.getName();
        std::vector<string> varkill;
        std::vector<string> uevar;
        for (auto& inst : basic_block)
        {
            if(inst.getOpcode() == Instruction::Load){
                string str1 = inst.getOperand(0)->getName();
                if(std::find(varkill.begin(), varkill.end(), str1) == varkill.end() &&
                        std::find(uevar.begin(), uevar.end(), str1) == uevar.end()) {
                    uevar.push_back(str1);
                }
            }
            if(inst.getOpcode() == Instruction::Store){
                if(inst.getOperand(1)->hasName() &&
                        std::find(varkill.begin(), varkill.end(), inst.getOperand(1)->getName()) == varkill.end()) {
                    string str2 = inst.getOperand(1)->getName();
                    varkill.push_back(str2);
                }
            }
        }
        sort(uevar.begin(), uevar.end());
        sort(varkill.begin(), varkill.end());
        pair<string, std::vector<string>> p1(name, uevar);
        pair<string, std::vector<string>> p2(name, varkill);
        n.push_back(name);
        Uevar.insert(p1);
        Varkill.insert(p2);
    }

    //get Liveout of every basic block;
    bool flag = true;
    int ite = 1;
    while(flag){
        //errs() << "iter number:" << ite << "\n";
        ite++;
        flag = false;
        for(auto iter = Succ.begin(); iter != Succ.end(); iter++) {
            //choose block n;
            string bb_name = iter->first;
            std::vector<string> pre = Liveout.find(bb_name)->second;
            //errs() << "bb_name:" << bb_name << "\n";
            //找出block n 的每一个secc block x(presecc)
            for(string presecc : Succ.find(bb_name)->second) {
                if ((Liveout.find(presecc)->second).size() == 0) {
                    std::vector <string> liveout = Liveout.find(presecc)->second;
                    std::vector <string> uevar = Uevar.find(presecc)->second;
                    liveout = uevar;
                    std::vector <string> bb = Liveout.find(bb_name)->second;
                    std::vector<string> newres;
                    std::set_union(bb.begin(), bb.end(), liveout.begin(), liveout.end(),
                                   std::back_inserter(newres));
                    Liveout[bb_name] = newres;
                } else {
                    std::vector <string> te = Liveout.find(bb_name)->second;
                    std::vector <string> liveout = Liveout.find(presecc)->second;
                    std::vector <string> uevar = Uevar.find(presecc)->second;
                    std::vector <string> varkill = Varkill.find(presecc)->second;
                    std::vector <string> temp1;
                    std::vector <string> temp2;
                    std::vector <string> temp3;
                    std::set_difference(liveout.begin(), liveout.end(), varkill.begin(), varkill.end(),
                                        std::back_inserter(temp1));
                    std::set_union(temp1.begin(), temp1.end(), uevar.begin(), uevar.end(),
                                   std::back_inserter(temp2));
                    std::set_union(temp2.begin(), temp2.end(), te.begin(), te.end(),
                                   std::back_inserter(temp3));
                    Liveout[bb_name] = temp3;
                }
            }
            if((Liveout.find(bb_name)->second).size() != pre.size()){
                flag = true;
            }
        }
    }

    //Print the result;
    for(string name : n){
        errs() << "---- " << name << " ----" << "\n";
        errs() << "UEVAR: ";
        for(string s : Uevar.find(name)->second){
            errs() << s << " ";
        }
        errs() << "\n";
        errs() << "VARKILL: ";
        for(string s : Varkill.find(name)->second){
            errs() << s << " ";
        }
        errs() << "\n";
        errs() << "LIVEOUT: ";
        for(string s : Liveout.find(name)->second){
            errs() << s << " ";
        }
        errs() << "\n";
    }

    return false;
  }
}; // end of struct ValueNumbering
}  // end of anonymous namespace

char ValueNumbering::ID = 0;
static RegisterPass<ValueNumbering> X("ValueNumbering", "ValueNumbering Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
