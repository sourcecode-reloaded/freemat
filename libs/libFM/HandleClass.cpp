#include "HandleClass.hpp"
#include "ThreadContext.hpp"
#include "AllTypes.hpp"
#include "Globals.hpp"

using namespace FM;

// The add listener method



// Create the built in listener class
void FM::makeListenerClass(ThreadContext *ctxt) {
  Object listener = ctxt->_meta->makeScalar();
  ClassMetaData *cmd = ctxt->_meta->rw(listener);
  ctxt->_meta->addProperty(listener,
			   ctxt->_string->makeString("Source"),
			   false, // isconstant
			   false, // isdependent
			   ctxt->_cell->empty(), // cell array of source objects
			   ctxt->_double->empty(),
			   ctxt->_double->empty());
  ctxt->_meta->addProperty(listener,
			   ctxt->_string->makeString("EventName"),
			   false, // isconstant
			   false, // isdependent
			   ctxt->_string->empty(), // name of the event
			   ctxt->_double->empty(),
			   ctxt->_double->empty());
  ctxt->_meta->addProperty(listener,
			   ctxt->_string->makeString("Callback"),
			   false, // isconstant
			   false, // isdependent
			   ctxt->_double->empty(), // name of the callback
			   ctxt->_double->empty(),
			   ctxt->_double->empty());
  ctxt->_meta->addProperty(listener,
			   ctxt->_string->makeString("Enabled"),
			   false, // isconstant
			   false, // isdependent
			   ctxt->_bool->makeScalar(true), // value
			   ctxt->_double->empty(), // getter
			   ctxt->_double->empty());
  ctxt->_meta->addProperty(listener,
			   ctxt->_string->makeString("Recursive"),
			   false, // isconstant
			   false, // isdependent
			   ctxt->_bool->makeScalar(false), // value
			   ctxt->_double->empty(), // getter
			   ctxt->_double->empty());
  Object handle = ctxt->_globals->get("handle",ctxt);
  cmd->m_name = ctxt->_string->makeString("event.listener");
  cmd->m_constructor = ctxt->_module->pass();
  ctxt->_meta->addSuperClass(listener,handle);
  // This is one way to handle namespaces...
  Object event_space = ctxt->_struct->makeScalar();
  ctxt->_struct->setField(event_space,ctxt->_string->makeString("listener"),listener);
  ctxt->_globals->set("event",event_space);
}

/*
Object addlistener(const Object &args, int nargout, ThreadContext *ctxt) {
  if (args.count() != 3)
    throw Exception("addlistener requires three arguments: obj, eventname, callback");
  const Object &callback = ctxt->_list->ro(args)[2];
  std::cout << "Check that eventname is valid\n";
  const ClassData *cd = ctxt->_class->ro(self);
  const ClassMetaData *cmd = ctxt->_meta->ro(cd->metaClass);
  ndx_t ndx = ctxt->_list->indexOf(cmd->m_events,eventname);
  if (ndx == -1) throw Exception("Eventname " + eventname.description() + " is not valid");
  return ctxt->_list->makeScalar(eventname);
}
*/

Object is_valid_event(const Object &args, int nargout, ThreadContext *ctxt) {
  if (args.count() != 2)
    throw Exception("is_valid_event requires two arguments: obj, eventname");
  const Object &self = ctxt->_list->ro(args)[0];
  const Object &eventname = ctxt->_list->ro(args)[1];
  const ClassData *cd = ctxt->_class->ro(self);
  const ClassMetaData *cmd = ctxt->_meta->ro(cd->metaClass);
  ndx_t ndx = ctxt->_list->indexOf(cmd->m_events,eventname);
  return ctxt->_list->makeScalar(ctxt->_bool->makeScalar(ndx != -1));
}

// Create the built in Handle class
void FM::makeHandleClass(ThreadContext *ctxt) {
  Object handle = ctxt->_meta->makeScalar();
  ClassMetaData *cmd = ctxt->_meta->rw(handle);
  ctxt->_meta->addProperty(handle,
			   ctxt->_string->makeString("_ishandle"),
			   true, // isconstant
			   false, // isdependent
			   ctxt->_bool->makeScalar(true), // value
			   ctxt->_double->empty(), // getter
			   ctxt->_double->empty());
  ctxt->_meta->addMethod(handle,
			 ctxt->_string->makeString("is_valid_event"),
			 ctxt->_module->builtin("is_valid_event",is_valid_event),
			 false); // TODO remove boolean arguments - they are unclear
  // ctxt->_meta->addProperty(handle,
  // 			   ctxt->_string->makeString("_listeners"),
  // 			   false, // isconstant
  // 			   false, // isdependent
  // 			   ctxt->_struct->makeScalar(), // Initial value
  // 			   ctxt->_double->empty(), // getter
  // 			   ctxt->_double->empty()); // seter
  // ctxt->_meta->addMethod(handle,
  // 			 ctxt->_string->makeString("addlistener"),
  // 			 ctxt->_builtin->makeBuiltin("addlistener",addlistener),
  // 			 false);
  cmd->m_name = ctxt->_string->makeString("base_handle");
  cmd->m_constructor = ctxt->_module->pass();
  ctxt->_globals->set("base_handle",handle);
}
