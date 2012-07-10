#include "llvm/Pass.h"
#include "llvm/Support/DebugLoc.h"
#include "llvm/Analysis/DebugInfo.h"
#include "llvm/Function.h"
#include "llvm/InstrTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>
#include <iostream>

using namespace llvm;

namespace  {

    std::string
    instr_label(Instruction const & instr, Instruction const & next)
    {
        CallInst const * call = dyn_cast<CallInst const>(&instr);
        if (call != NULL) {
            return "call: " + call->getCalledFunction()->getName().str();
        }
        else {
            MDNode const * md = instr.getDebugLoc().getAsMDNode(instr.getContext());
            if (md != NULL) {
                DILocation loc(md);
                std::stringstream ss;
                ss << loc.getFilename().str() << ":" << loc.getLineNumber() << ":" << loc.getColumnNumber();
                return instr.getOpcodeName() + std::string(" [") + ss.str() + "]";
            }
            else {
                return instr.getOpcodeName();
            }
        }
    }

    template<typename Iterator>
    Iterator next_iter(Iterator iter)
    {
	++iter;
	return iter;
    }

    std::string
    block_label(BasicBlock const & block)
    {
        std::string out;
        for(BasicBlock::const_iterator
		instr = block.begin(),
		next = next_iter(block.begin()),
		end = block.end();
            next != end;
	    ++instr, ++next)
        {
	    
            out += instr_label(*instr, *next);
            out += "\n";
        }
        return out;
    }
    

    struct Hello : public FunctionPass {

        static char ID;
        Hello() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &f) {
            std::cout << "*********\n";
            std::cout << f.getName().str() << "\n";

            for(Function::const_iterator block = f.begin(), end = f.end();
                block != end; ++block)
            {
                std::cout << "------\n";
                std::string const & label = block_label(*block);
                std::cout << label << "\n";

                TerminatorInst const * term = block->getTerminator();

                for(unsigned succ = 0, end = term->getNumSuccessors();
                    succ != end; ++succ)
                {
                    std::cout << &*block << " --> " << term->getSuccessor(succ) << "\n";
                }
            }

            return false;
        }
    };  // end of struct Hello
}  // end of anonymous namespace

char Hello::ID = 0;

static RegisterPass<Hello> X("hello", "Hello World Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);


