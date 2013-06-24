#include <boost/algorithm/string/predicate.hpp>

#include "llvm/Pass.h"
#include "llvm/Support/DebugLoc.h"
#include "llvm/DebugInfo.h"
#include "llvm/Function.h"
#include "llvm/InstrTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>
#include <iostream>
#include <string>
#include <set>

using namespace llvm;

namespace  {

  std::set<std::string> called_functions, visited_functions;

    std::string
    instr_label(Instruction const & instr, Instruction const & next)
    {
        CallInst const * call = dyn_cast<CallInst const>(&instr);
        if (call != NULL) {
	    std::stringstream ss;
	    Function * func = call->getCalledFunction();
	    if (func == NULL) {
		ss << "Delta_i: {(" << &instr << ", [XXX INDIR CALL], " << &next << ")}\n";
		return ss.str();
	    }
	    std::string callee =func->getName().str();
	    if (!boost::starts_with(callee, "llvm.dbg")) {
		std::stringstream csss; csss << &instr; std::string cs = csss.str();
		std::stringstream rsss; rsss << "r" << &instr; std::string rs = rsss.str();
		called_functions.insert(callee);
		std::string es = "e." + callee;
		std::string xs = "x." + callee;
		ss << "Delta_c: {(" << cs << ", call-" << callee << ", " << es << ")}\n";
		ss << "Delta_r: {(" << xs << ", " << cs << ", ret-" << callee << ", " << rs << ")}\n";
		ss << "Delta_i: {(" << rs << ", *, " << &next << ")}\n";
		return ss.str();
	    }
	    ss << "Delta_i: (" << &instr << ", *, " << &next << ")\n";
	    return ss.str();
        }
        else {
	    std::stringstream ss;
	    ss << "Delta_i: {(" << &instr << ", (" << instr.getOpcodeName();
            MDNode const * md = instr.getDebugLoc().getAsMDNode(instr.getContext());
            if (md != NULL) {
                DILocation loc(md);
                ss << " [" << loc.getFilename().str() << ":" << loc.getLineNumber() << ":" << loc.getColumnNumber() << "]";
            }
	    ss << "), " << &next << ")}\n";
	    return ss.str();
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
	BasicBlock::const_iterator
	    instr = block.begin(),
	    next = next_iter(block.begin()),
	    end = block.end();
        for(;
            next != end;
	    ++instr, ++next)
        {
	  //std::cout << &*instr << "->" << &*next << ":" << instr->getOpcodeName() << "\n";
            out += instr_label(*instr, *next);
            out += "\n";
        }
	assert(block.getTerminator() == &*instr);
        return out;
    }


    Instruction const & firstInstr(Function const & f)
    {
	BasicBlock const & first = f.getEntryBlock();
	BasicBlock::const_iterator instr = first.begin();
	return *instr;
    }
    
    Instruction const & successorInst(TerminatorInst const & inst, unsigned succ)
    {
	BasicBlock * blk = inst.getSuccessor(succ);
	BasicBlock::const_iterator instr = blk->begin();
	return *instr;
    }
    

    struct Hello : public FunctionPass {

        static char ID;
        Hello() : FunctionPass(ID) {}

      ~Hello() {
	for (std::set<std::string>::const_iterator iter = called_functions.begin();
	     iter != called_functions.end(); ++iter)
	{
	  if (visited_functions.count(*iter) == 0) {
	    std::cerr << "Delta_i: {(e." << *iter << ", *,"
		      << " x." << *iter << ")}\n";
	  }
	}
      }

        virtual bool runOnFunction(Function &f) {
	    static bool first = true;
	    if (first) {
		first = false;
		std::cerr << "Q0: e.main\nQf: x.main\n";
	    }
            //std::cerr << "*********\n";
            //std::cerr << f.getName().str() << "\n";

	    std::cerr << "Delta_i: {(e." << f.getName().str() << ", *, " << &firstInstr(f) << ")}\n";

	    visited_functions.insert(f.getName().str());

            for(Function::const_iterator block = f.begin(), end = f.end();
                block != end; ++block)
            {
                //std::cerr << "------\n";
                std::string const & label = block_label(*block);
                std::cerr << label << "\n";

                TerminatorInst const * term = block->getTerminator();

                for(unsigned succ = 0, end = term->getNumSuccessors();
                    succ != end; ++succ)
                {
		    std::cerr << "Delta_i: {(" << term << ", *, " << &successorInst(*term, succ) << ")}\n";
                }

		if (term->getNumSuccessors() == 0) {
		    assert(term->getOpcodeName() == std::string("ret")
			   || term->getOpcodeName() == std::string("unreachable"));
		    std::cerr << "Delta_i: {(" << term << ", *, " << "x." << f.getName().str() << ")}\n";
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


