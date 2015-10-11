#include "GarbageCollector.hpp"
#include "AllTypes.hpp"

using namespace FM;

using children_list = std::multiset<ObjectBase*>;
using connections = std::map<ObjectBase*,children_list>;

namespace FM {
  class VisitAncestors : public ObjectVisitor {
    children_list &my_kids;
    ThreadContext *my_ctxt;
  public:
    VisitAncestors(ObjectBase *root, children_list &descendents, ThreadContext *ctxt) : my_kids(descendents), my_ctxt(ctxt) 
    {
      std::cout << "Initializing visitor with base " << root << "\n";
    }
    void operator()(const Object &p) {
      std::cout << "  Visit " << p.raw() << " of type code " << p.typeCode() << " with brief " << p.brief() << "\n";
      if (p.isEmpty()) return;
      if (p.isClass() && (my_ctxt->_class->isHandle(p))) {
	std::cout << "Found descendant with base " << p.raw() << "\n";
	my_kids.insert(p.raw());
	return;
      }
      p.type()->visitContainedObjects(p.raw(),*this);
    }
  };
}

Object FM::builtin_gc(const Object &, ndx_t, ThreadContext *ctxt) {
  // First, copy the refcnts to the gc_refcnts
  for (auto p: ctxt->_handles) {
    p->gc_count() = p->count();
    std::cout << "Object " << p << " has ref count " << p->gc_count() << "\n";
  }
  // Next, build the edge list
  connections edges;
  for (auto p: ctxt->_handles) {
    children_list descendents;
    VisitAncestors visitor(p,descendents,ctxt);
    p->get_type()->visitContainedObjects(p,visitor);
    edges[p] = descendents;
  }
  // Next, we loop through the connections, and for each child, we decrement it's reference count
  for (auto root: edges) 
    for (auto child: root.second) {
      std::cout << "Decrementing count for child " << child << ", now at " << child->gc_count() << "\n";
      child->gc_count()--;
    }
  // Now make a pass through it -- any node that has a non-zero
  // count has external references, and it's descendants need
  // to be bumped up
  for (auto p: ctxt->_handles)
    if (p->gc_count() > 0) {
      std::cout << "Object " << p << " has non-zero count - incrementing referenced objects\n";
      for (auto child: edges[p]) {
	child->gc_count()++;
	std::cout << "  -> Marking " << child << " with reference " << child->gc_count() << "\n";
      }
    }
  // Last pass, any object with a reference of 0 should be freed - 
  // To do so --
  //   1. sort objects so that last created is first destroyed
  //   2. artificially bump reference count for each object so there
  //      is no cascade of deletes
  //   3. "manually" call delete on each object
  children_list to_delete;
  for (auto p: ctxt->_handles)
    if (p->gc_count() == 0) {
      p->count()++;
      to_delete.insert(p);
    }
  for (auto p: to_delete)
    if (p->gc_count() == 0) 
      (p->get_type())->destroyObject(p);
  return ctxt->_double->empty();
}
