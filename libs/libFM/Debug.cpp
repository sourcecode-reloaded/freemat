#include "Debug.hpp"
#include "ThreadContext.hpp"
#include "VM.hpp"
#include "FileSystem.hpp"
#include "ListType.hpp"
#include "StringType.hpp"
#include "BreakpointType.hpp"
#include "Globals.hpp"
#include "Compiler.hpp"
#include "Assembler.hpp"

using namespace FM;

// TODO - Breakpoints should be across all threads...
Object FM::dbstop(const Object &args, int nargout, ThreadContext *ctxt) {
  std::cout << "DBSTOP called: " << args.description() << "\n";
  const Object *rp = ctxt->_list->ro(args);
  // There is a mini-grammar for dbstop
  // dbstop in file
  FMString module;
  FMString loc;
  FMString exp;
  bool inflag = false;
  bool atflag = false;
  bool expflag = false;
  bool whenflag = false;
  int ip = 0;
  while (ip < args.count()) {
    if ((ctxt->_string->str(rp[ip]).toLower() == "in") && (args.count() >= (ip+1))) {
      module = ctxt->_string->str(rp[ip+1]);
      inflag = true;
      ip += 2;
    } else if ((ctxt->_string->str(rp[ip]).toLower() == "at") && (args.count() >= (ip+1))) {
      loc = ctxt->_string->str(rp[ip+1]);
      atflag = true;
      ip += 2;
    } else if ((ctxt->_string->str(rp[ip]).toLower() == "if") && (args.count() >= (ip+1))) {
      for (int i=ip+1;i<args.count();i++)
	exp += ctxt->_string->str(rp[i]);
      expflag = true;
      ip = args.count();
    }
  }
  if (inflag)
    std::cout << "in: " << module << "\n";
  if (atflag)
    std::cout << "at: " << loc << "\n";
  if (expflag)
    std::cout << "if: " << exp << "\n";
  if (expflag && !inflag) whenflag = true; // if without in --> when
  if (atflag && !inflag) throw Exception("Cannot specify dbstop location with 'at' without a location using 'in'");
  
  
  // Lock the global lock
  Object bp = ctxt->_breakpoint->makeScalar();
  BreakpointData *bpd = ctxt->_breakpoint->rw(bp);
  bpd->frame_name = resolve_full_path(module); 
  bpd->line_number = loc.toInt();  //FIXME if it has num@num
  bpd->bp_type = BreakpointTypeCode::Unconditional;
  if (expflag && !whenflag)
    {
      bpd->bp_type = BreakpointTypeCode::Conditional;
      ctxt->_compiler->compile("dbstop__=("+exp+");\n");
      bpd->m_condition = ctxt->_asm->run(ctxt->_compiler->module());
    }
  bpd->bp_when = BreakpointWhen::Always;
  if (whenflag) {
    bpd->bp_type = BreakpointTypeCode::When;
    if (exp.toLower() == "error")
      bpd->bp_when = BreakpointWhen::Error;
    else if (exp.toLower() == "caughterror")
      bpd->bp_when = BreakpointWhen::CaughtError;
    else if (exp.toLower() == "warning")
      bpd->bp_when = BreakpointWhen::Warning;
    else if (exp.toLower() == "naninf")
      bpd->bp_when = BreakpointWhen::NanInf;
    else
      throw Exception("Unrecognized form of dbstop condition " + exp);
  }
  bpd->id = 123;
  Object dblist(ctxt->_list->empty());
  if (ctxt->_globals->isDefined("_dblist"))
    dblist = ctxt->_globals->get("_dblist",ctxt);
  std::cout << "DBList was: " << dblist.description() << "\n";
  ctxt->_list->push(dblist,bp);
  std::cout << "DBList is now: " << dblist.description() << "\n";
  ctxt->_globals->set("_dblist",dblist);
  Object dlist2 = ctxt->_globals->get("_dblist",ctxt);
  std::cout << "Fetch of DBList yields:" << dlist2.description() << "\n";
  std::cout << "ctxt->_globals = " << ctxt->_globals << "\n";
  return ctxt->_list->makeScalar(bp);
}

