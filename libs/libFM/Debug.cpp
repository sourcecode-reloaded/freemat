#include "Debug.hpp"
#include "ThreadContext.hpp"
#include "VM.hpp"
#include "FileSystem.hpp"
#include "ListType.hpp"
#include "StringType.hpp"
#include "BreakpointType.hpp"
#include "Globals.hpp"

using namespace FM;

// TODO - Breakpoints should be across all threads...
void FM::DBStop(ThreadContext *ctxt, const Object &args) {
  std::cout << "DBSTOP called: " << args.description() << "\n";
  const Object *rp = ctxt->_list->ro(args);
  const Object &module = rp[0];
  const Object &location = rp[1];
  const Object &condition = rp[2];
  std::cout << "  Module:" << module.description() << "\n";
  std::cout << "  Location: " << location.description() << "\n";
  std::cout << "  Condition: " << condition.description() << "\n";
  // TODO - replace this with a path search
  std::cout << "  Full path: " << resolve_full_path(ctxt->_string->str(module)) << "\n";
  // Check for case of an integer
  FMString loc = ctxt->_string->str(location);
  bool isint = true;
  for (auto p : loc) {
    if (!isdigit(p)) {
      isint = false;
      break;
    }
  }
  std::cout << "Pure integer check: " << isint << "\n";
  // Lock the global lock
  Object bp = ctxt->_breakpoint->makeScalar();
  BreakpointData *bpd = ctxt->_breakpoint->rw(bp);
  bpd->frame_name = resolve_full_path(ctxt->_string->str(module));
  bpd->line_number = loc.toInt();
  bpd->bp_type = BreakpointTypeCode::Unconditional;
  bpd->m_condition = condition;
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
}

