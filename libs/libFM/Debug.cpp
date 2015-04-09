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

struct dbargs {
  FMString module;
  FMString loc;
  FMString expr;
  bool allflag;
  bool inflag;
  bool atflag;
  bool expflag;
  bool whenflag;
  BreakpointWhen bp_when;
};

static void ParseDBCommandArgs(ThreadContext *ctxt, const Object &args, dbargs& result) {
  result.allflag = false;
  result.inflag = false;
  result.expflag = false;
  result.whenflag = false;
  result.atflag = false;
  const Object *rp = ctxt->_list->ro(args);
  int ip = 0;
  while (ip < args.count()) {
    if ((ctxt->_string->str(rp[ip]).toLower() == "in") && (args.count() >= (ip+1))) {
      result.module = ctxt->_string->str(rp[ip+1]);
      result.inflag = true;
      ip += 2;
    } else if ((ctxt->_string->str(rp[ip]).toLower() == "at") && (args.count() >= (ip+1))) {
      result.loc = ctxt->_string->str(rp[ip+1]);
      result.atflag = true;
      ip += 2;
    } else if ((ctxt->_string->str(rp[ip]).toLower() == "if") && (args.count() >= (ip+1))) {
      FMString exp;
      for (int i=ip+1;i<args.count();i++)
	exp += ctxt->_string->str(rp[i]);
      result.expr = exp;
      result.expflag = true;
      ip = args.count();
    } else if ((ctxt->_string->str(rp[ip]).toLower() == "all")) {
      result.allflag = true;
      ip++;
    }
  }
  if (result.expflag && !result.inflag) result.whenflag = true;
  if (result.atflag && !result.inflag) throw Exception("Cannot specify breakpoint location with 'at' without a location using 'in'");
  result.bp_when = BreakpointWhen::Always;
  if (result.whenflag){
    if (result.expr.toLower() == "error")
      result.bp_when = BreakpointWhen::Error;
    else if (result.expr.toLower() == "caughterror")
      result.bp_when = BreakpointWhen::CaughtError;
    else if (result.expr.toLower() == "warning")
      result.bp_when = BreakpointWhen::Warning;
    else if (result.expr.toLower() == "naninf")
      result.bp_when = BreakpointWhen::NanInf;
    else
      throw Exception("Unrecognized form of dbstop condition " + result.expr);
  }
}

Object FM::dbquit(const Object &args, int nargout, ThreadContext *ctxt) {
  if (args.count() == 0)
    throw VMDBQuitException();
  throw VMDBQuitException(-1);
}

Object FM::dbcont(const Object &args, int nargout, ThreadContext *ctxt) {
  ctxt->_vm->signalReturn();
  return ctxt->_list->empty();
}

Object FM::dbclear(const Object &args, int nargout, ThreadContext *ctxt) {
  const Object *rp = ctxt->_list->ro(args);
  if (args.count() == 0) return ctxt->_list->empty();
  dbargs cmd_args;
  ParseDBCommandArgs(ctxt,args,cmd_args);
  if (!ctxt->_globals->isDefined("_dblist")) 
    return ctxt->_list->empty(); // Nothing to do.
  if (cmd_args.allflag)
    {
      ctxt->_globals->set("_dblist",ctxt->_list->empty());
      return ctxt->_list->empty();
    }
  Object dblist = ctxt->_globals->get("_dblist",ctxt);
  const Object *old = ctxt->_list->ro(dblist);
  Object dblist_filtered = ctxt->_list->empty();
  for (int i=0;i<dblist.count();i++) {
    const BreakpointData *cbpd = ctxt->_breakpoint->ro(old[i]);
    bool keepit = true;
    if (cmd_args.inflag && !cmd_args.atflag && (cbpd->frame_name == resolve_full_path(cmd_args.module)))
      keepit = false;
    if (cmd_args.inflag && cmd_args.atflag && (cbpd->frame_name == resolve_full_path(cmd_args.module))
	&& (cbpd->line_number == cmd_args.loc.toInt()))
      keepit = false;
    if (cmd_args.whenflag && (cbpd->bp_when == cmd_args.bp_when))
      keepit = false;
    if (keepit)
      ctxt->_list->push(dblist_filtered,old[i]);
  }
  ctxt->_globals->set("_dblist",dblist_filtered);
  return ctxt->_list->empty(); // Nothing to do.
}

Object FM::dbstop(const Object &args, int nargout, ThreadContext *ctxt) {
  std::cout << "DBSTOP called: " << args.description() << "\n";
  const Object *rp = ctxt->_list->ro(args);
  // There is a mini-grammar for dbstop
  dbargs cmd_args;
  ParseDBCommandArgs(ctxt,args,cmd_args);
  Object bp = ctxt->_breakpoint->makeScalar();
  BreakpointData *bpd = ctxt->_breakpoint->rw(bp);
  bpd->frame_name = resolve_full_path(cmd_args.module); 
  bpd->line_number = cmd_args.loc.toInt();  //FIXME if it has num@num
  bpd->bp_type = BreakpointTypeCode::Unconditional;
  if (cmd_args.expflag && !cmd_args.whenflag)
    {
      bpd->bp_type = BreakpointTypeCode::Conditional;
      ctxt->_compiler->compile("dbstop__=("+cmd_args.expr+");\n");
      bpd->m_condition = ctxt->_assembler->run(ctxt->_compiler->module());
    }
  bpd->bp_when = BreakpointWhen::Always;
  if (cmd_args.whenflag) {
    bpd->bp_type = BreakpointTypeCode::When;
    bpd->bp_when = cmd_args.bp_when;
  }
  bpd->id = 123;
  Object dblist(ctxt->_list->empty());
  if (ctxt->_globals->isDefined("_dblist"))
    dblist = ctxt->_globals->get("_dblist",ctxt);
  std::cout << "DBList was: " << dblist.description() << "\n";
  ctxt->_list->push(dblist,bp);
  std::cout << "DBList is now: " << dblist.description() << "\n";
  ctxt->_globals->set("_dblist",dblist);  // Race condition - if two threads set breakpoints at the same time...
  Object dlist2 = ctxt->_globals->get("_dblist",ctxt);
  std::cout << "Fetch of DBList yields:" << dlist2.description() << "\n";
  std::cout << "ctxt->_globals = " << ctxt->_globals << "\n";
  return ctxt->_list->makeScalar(bp);
}

Object FM::dbup(const Object &args, int nargout, ThreadContext *ctxt) {
  Frame *t = ctxt->_vm->activeFrame();
  Frame *debug = t;
  while (debug && (debug->_type != FrameTypeCode::Debug))
    debug = debug->_prevFrame;
  if (!debug) return ctxt->_list->empty();

  Object dbshift(ctxt);
  bool dboffset = debug->getLocalVariableSlow("dbshift__",dbshift);
  if (!dboffset) dbshift = ctxt->_double->makeScalar(0);
  // TODO - Detect at top of stack
  dbshift = ctxt->_double->makeScalar(dbshift.asDouble() + 1);
  debug->setVariableSlow("dbshift__",dbshift);
  return ctxt->_list->empty();
  
  Frame *frame = debug->_closedFrame->_prevFrame;
  while (frame && (!frame->_closed))
    frame = frame->_prevFrame;
  if (!frame)
    throw Exception("Cannot go up");
  debug->_closedFrame = frame;
  //  ctxt->_vm->signalReturn();
  return ctxt->_list->empty();
}

Object FM::dbdown(const Object &args, int nargout, ThreadContext *ctxt) {
  Frame *t = ctxt->_vm->activeFrame();
  Frame *debug = t;
  while (debug && (debug->_type != FrameTypeCode::Debug))
    debug = debug->_prevFrame;
  if (!debug) return ctxt->_list->empty();
  
  Object dbshift(ctxt);
  bool dboffset = debug->getLocalVariableSlow("dbshift__",dbshift);
  if (!dboffset) throw Exception("Cannot go down");
  if (dbshift.asDouble() <= 0) throw Exception("Cannot go down");
  dbshift = ctxt->_double->makeScalar(dbshift.asDouble() - 1);
  debug->setVariableSlow("dbshift__",dbshift);
  return ctxt->_list->empty();
    
  Frame *frame = debug->_closedFrame->_nextFrame;
  while (frame && (!frame->_closed))
    frame = frame->_nextFrame;
  if (!frame)
    throw Exception("Cannot go up");
  debug->_closedFrame = frame;
  //  ctxt->_vm->signalReturn();
  return ctxt->_list->empty();  
}

Object FM::dbstep(const Object &args, int nargout, ThreadContext *ctxt) {
  std::cout << "DBSTEP called: " << args.description() << "\n";
  // Skip parsing the arguments for now...
  Frame *t = ctxt->_vm->activeFrame();
  // t is our frame.  Search backwards for something we can meaningfully step in
  Frame *debug = t;
  while (debug && debug->_type != FrameTypeCode::Debug) debug = debug->_prevFrame;
  debug = debug->_prevFrame;
  if (!debug) 
    return ctxt->_list->empty(); // Couldn't find anything to step
  assert(debug->_name != ">>debug"); // Shouldn't happen
  int lineno = debug->mapIPToLineNumber(debug->_ip);
  if (lineno == -1)
    {
      // Should be equivalent to dbstep out, I would think.
      ctxt->_vm->signalReturn();
      return ctxt->_list->empty();
    }
  if (args.count() > 0) {
    const Object *ap = ctxt->_list->ro(args);
    FMString arg = ctxt->_string->str(ap[0]).toLower();
    if (arg == "in")
      debug->_state = FrameRunStateCode::StepIn;
    else if (arg == "out") {
      Frame *debug_parent = debug->_prevFrame;
      while (debug_parent && (!debug_parent->isDebuggable())) debug_parent = debug_parent->_prevFrame;
      if (!debug_parent) return ctxt->_list->empty();
      assert(debug_parent->_name != ">>debug"); // Shouldn't happen
      std::cout << "Setting state for frame " << debug_parent->_name << " to StepOut\n";
      debug_parent->_state = FrameRunStateCode::StepOut;
    } else {
      int skip_lines = arg.toInt();
      if (skip_lines < 1)
	throw Exception("dbstep requires an argument >= 1 (or in/out)");
      debug->_transient_bp = lineno+skip_lines;
    }
  } else {
    debug->_transient_bp = lineno+1;
  }
  ctxt->_vm->signalReturn();
  return ctxt->_list->empty();
}
