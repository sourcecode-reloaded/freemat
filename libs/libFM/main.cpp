#include <boost/timer/timer.hpp>
#include <boost/unordered_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <unordered_map>
#include "Object.hpp"
#include "DoubleType.hpp"
#include "Scanner.hpp"
#include "Parser.hpp"
#include "ThreadContext.hpp"
#include "AllTypes.hpp"
#include "Compiler.hpp"
#include "Assembler.hpp"
#include "VM.hpp"
//#include "fnv.hh"
#include "HashMap.hpp"
#include "Symbol.hpp"
#include "GarbageCollector.hpp"
#include "HandleClass.hpp"
#include "FileSystem.hpp"
#include "Debug.hpp"
#include "TypeUtils.hpp"
#include "Globals.hpp"
#include "SparseType.hpp"
#include <readline/readline.h>
#include <readline/history.h>
#include "Config.hpp"

//#include <valarray>

// What is the strategy for get?
//
// get(ObjectVector)
//
// Option 1. Convert all arguments to index arrays.
//   Each Type object has a "asIndex" function.
//   Something like:
//      Object asIndex(const Object &a, const Tuple &dims)
//  The index type object can then be used to index the output.
//  Need to make a decision about slicing.
//

using namespace FM;

typedef std::pair<int, int> Edge;

struct cycle_detector : public boost::dfs_visitor<>
{
  cycle_detector(std::list<Edge > &bad_list) : _bad_list(bad_list) {}
  template <class Edge, class Graph>
  void back_edge(Edge e, Graph& g) {
    std::cout << "Found back edge " << e << "\n";
    std::cout << "Source: " << source(e,g) << "\n";
    std::cout << "Target: " << target(e,g) << "\n";
    _bad_list.push_back(std::make_pair(source(e,g),target(e,g)));
  }
protected:
  std::list<Edge > &_bad_list;
};


void compileModule(ThreadContext *ctxt, const FMString &name, HashMap<Object> &classes)
{
  std::cout << "Compiling module: " << name << "\n";
  try {
    bool failed;
    FMString body = ReadFileIntoString(name+".m",failed);
    body += "\n\n";
    ctxt->_compiler->compile(body,resolve_full_path(name));
    Module *mod = ctxt->_compiler->module();
    if (mod)
      {
	Object p = ctxt->_assembler->run(mod);
	DBOUT(Disassemble(ctxt,p));
	DBOUT(std::cout << "Compile: \n");
	DBOUT(std::cout << p << "\n");
	if (ctxt->_module->ro(p)->m_modtype == ClassdefModuleType)
	  classes.insert(std::make_pair(ctxt->_string->makeString(name),p));
	  // {
	  //   // Check for dependencies...
	  //   ctxt->_module->deref(p);
	  // }
	else
	  ctxt->_globals->set(name,p);
      }
    else
      {
	std::cout << "   Module empty\n";
      }
  } catch (const FM::Exception &e) {
    std::cout << "Exception: " << e.msg() << "\n";
  }
}

// Here is an example of a reduction operation
//
//  The number of reductions to perform is prod(size(a)) with size(a)[reddim] = 1
//  The output stride is 1
//  The reduction length is size(a)[reddim]
//  The input stride is trickier.  The amount to skip is prod(size(a)) for dims=1...reddim-1 - this is the stride
//  The number of reductions to do is prod(size(a))/size(a)[reddim]
//
//  Consider a 3 dim array - rows x cols x slices
//    where the reduction is along the cols
//    reductionlen = cols
//    howmany = rows*slices
//    pagesize = rows*cols
//    pagecount = slices
//    stride = rows
//
//  Now consider a 4 dim arrat rows x cols x slices x cubes
//  If we want to reduce along the slices direction
//
//    reductionlen = slices
//    stride = rows*cols
//    pagesize = rows*cols*slices
//    pagecount = cubes
//    rowcount = rows*cols
//  For the input, we will loop through rows (pagesize).
//  start = row + page*pagesize + ndx*stride

template <class Input, class Output, class Op>
inline static void dispatch_reduce(Output *op, const Input *ip, ndx_t reduxlen, ndx_t pagecount, ndx_t stride)
{
  ndx_t ptr = 0;
  for (ndx_t page=0;page<pagecount;page++)
    for (ndx_t row=0;row<stride;row++)
      Op::func(op+(ptr++),ip+page*stride*reduxlen+row,reduxlen,stride);
}

Object reduce(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  ndx_t dim = 0;
  if (args.count() > 1)
    dim = (ndx_t)(ctxt->_list->second(args).asDouble()) - 1;
  const Object & x = ctxt->_list->first(args);
  const Tuple &inputdims = x.rodims();
  // Next calculate the output size
  Tuple outputSize = inputdims;
  outputSize.set(dim,1);
  // Calculate the number of outputs
  ndx_t howmany = outputSize.count();
  ndx_t reduxlen = inputdims.dimension(dim);
  ndx_t stride = 1;
  for (ndx_t k=0;k<dim;k++) stride *= inputdims.dimension(k);
  ndx_t pagecount = howmany/stride;
  Object ret(ctxt->_double->zeroArrayOfSize(outputSize));
  dispatch_reduce<double,double,OpSum>(ctxt->_double->rw(ret),ctxt->_double->ro(x),reduxlen,pagecount,stride);
  return ctxt->_list->makeScalar(ret);
}

// All - is a reduction operation
Object all(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  
}


Object strcmp(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  if (args.count() != 2) throw Exception("strcmp requires two arguments");
  const Object &x = ctxt->_list->first(args);
  const Object &y = ctxt->_list->second(args);
  if (x.isString() && y.isString())
    return ctxt->_list->makeScalar(ctxt->_bool->makeScalar(ctxt->_string->str(x) == ctxt->_string->str(y)));
  return ctxt->_list->makeScalar(ctxt->_bool->makeScalar(false));
}

Object size(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  Tuple dims(Tuple::canonicalForm(ctxt->_list->ro(args)[0].dims()));
  Object ret(ctxt->_double->makeMatrix(1,dims.dimensions()));
  for (auto i=0;i<dims.dimensions();i++)
    ctxt->_double->rw(ret)[i] = double(dims.dimension(i));
  return ctxt->_list->makeScalar(ret);
}

Object print(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  ctxt->_io->output("\n\nPrint:" + args.description() + "\n\n");
  return ctxt->_list->empty();
}

Object handir(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  for (auto p : ctxt->_handles) {
    ctxt->_io->output(" Handle class: " + Stringify(p) + "\n");
  }
  return ctxt->_list->empty();
}

Object numel(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
	return ctxt->_list->makeScalar(ctxt->_double->makeScalar(double(ctxt->_list->ro(args)[0].count())));
}


Object tot_int8(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  const Object &arg(ctxt->_list->ro(args)[0]);
  if (arg.type()->isComplexType())
    return ctxt->_list->makeScalar(ctxt->t_zint8->convert(arg));
  else
    return ctxt->_list->makeScalar(ctxt->t_int8->convert(arg));
}

Object tot_int16(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  return ctxt->_list->makeScalar(ctxt->t_int16->convert(ctxt->_list->ro(args)[0]));
}

Object tot_int32(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  return ctxt->_list->makeScalar(ctxt->t_int32->convert(ctxt->_list->ro(args)[0]));
}

Object tot_int64(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  return ctxt->_list->makeScalar(ctxt->t_int64->convert(ctxt->_list->ro(args)[0]));
}

Object to_uint8(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  return ctxt->_list->makeScalar(ctxt->_uint8->convert(ctxt->_list->ro(args)[0]));
}

Object to_uint16(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  return ctxt->_list->makeScalar(ctxt->_uint16->convert(ctxt->_list->ro(args)[0]));
}

Object to_uint32(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  return ctxt->_list->makeScalar(ctxt->_uint32->convert(ctxt->_list->ro(args)[0]));
}

Object to_uint64(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  return ctxt->_list->makeScalar(ctxt->_uint64->convert(ctxt->_list->ro(args)[0]));
}

Object to_double(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  return ctxt->_list->makeScalar(ctxt->_double->convert(ctxt->_list->ro(args)[0]));
}

Object to_single(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  return ctxt->_list->makeScalar(ctxt->_single->convert(ctxt->_list->ro(args)[0]));
}

Object backtrace(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  return ctxt->_list->makeScalar(ctxt->_vm->backtrace());
}

Object mksparse(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  const Object &x = ctxt->_list->ro(args)[0];
  if (x.isComplex())
    return ctxt->_list->makeScalar(ctxt->_spcomplex->convert(x));
  else
    return ctxt->_list->makeScalar(ctxt->_spdouble->convert(x));
}

Object full(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  const Object &x = ctxt->_list->ro(args)[0];
  switch (x.typeCode()) {
  case TypeSparseLogical:
    return ctxt->_list->makeScalar(ctxt->_splogical->asFullMatrix(x));
  case TypeSparseDouble:
    return ctxt->_list->makeScalar(ctxt->_spdouble->asFullMatrix(x));
  case TypeSparseComplex:
    return ctxt->_list->makeScalar(ctxt->_spcomplex->asFullMatrix(x));
  default:
    return args;
  }
}

Object dblist(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  if (ctxt->_globals->isDefined("_dblist"))
    {
      Object dblist = ctxt->_globals->get("_dblist",ctxt);
      return ctxt->_list->makeScalar(makeCellFromList(ctxt,dblist));
    }
  else
    return ctxt->_list->makeScalar(ctxt->_cell->empty());
}

// Create an addlistener method for the handle class

Object classfunc(const Object &args, ndx_t nargout, ThreadContext *ctxt) {
  const Object &x = ctxt->_list->ro(args)[0];
  if (!x.isClass())
    return ctxt->_list->makeScalar(ctxt->_string->makeString(x.type()->name()));
  else
    return ctxt->_list->makeScalar(ctxt->_class->className(x));
}

int main(int argc, char *argv[])
{

  assert(sizeof(Complex<char>) == 2*sizeof(char));

  StdIOTermIF io;
  std::mutex *lock = new std::mutex;
  std::map<FMString,Object> *globals = new std::map<FMString,Object>();
  
  ThreadContext *ctxt = ThreadContext::BuildNewThreadContext(&io); // Global context
  //  globals->insert(std::make_pair("_dblist",ctxt->_cell->empty()));
  ctxt->_globals = new Globals(ctxt); // shared by all

  makeHandleClass(ctxt);
  //  makeListenerClass(ctxt);

  boost::timer::cpu_timer timer;

  boost::filesystem::path cwd(boost::filesystem::current_path());
  boost::filesystem::directory_iterator p(cwd);

  HashMap<Object> classes;

  while (p != boost::filesystem::directory_iterator()) {
    std::cout << "File: " << *p << " Extension " << p->path().extension() << " Stem " << p->path().stem() << "\n";
    if (p->path().extension() == ".m" || p->path().extension() == ".M")
      {
	FMString func = p->path().stem().string();
	std::cout << "  Parsing function " << func << "\n";
	compileModule(ctxt,func,classes);
      }
    ++p;
  }

  std::cout << "********************************************************************************\n";
  std::cout << "*  Classes\n";
  for (auto i=classes.begin();i!=classes.end();++i)
    std::cout << "Class: " << i->first << " " << i->second << "\n";
  std::cout << "********************************************************************************\n";

  // Use Boost to analyze the dependencies
  FMMap<FMString,int> name_to_node_map;
  FMMap<int,FMString> node_to_name_map;
  auto nodectr = 0;
  for (auto i=classes.begin();i!=classes.end();++i)
    {
      std::cout << "Assigning class " << i->first << " to node " << nodectr << "\n";
      name_to_node_map[ctxt->_string->str(i->first)] = nodectr;
      node_to_name_map[nodectr] = ctxt->_string->str(i->first);
      nodectr++;
    }

  // Next, build a list of edges
  std::vector<Edge > edge_list;

  for (auto i=classes.begin();i!=classes.end();++i)
    {
      auto src = name_to_node_map[ctxt->_string->str(i->first)];
      Object dependencies = ctxt->_module->ro(i->second)->m_dependencies;
      for (auto j=0;j<dependencies.count();j++) {
	Object dependency = ctxt->_list->ro(dependencies)[j];
	FMString dependency_name = ctxt->_string->str(dependency);
	if (name_to_node_map.contains(dependency_name))
	  {
	    auto dst = name_to_node_map[dependency_name];
	    std::cout << "Class " << src << " depends on class " << dst << "\n";
	    edge_list.push_back(std::make_pair(dst,src));
	  }
     }
    }

  // Check for cycles
  
  
  // Use BOOST to find the visitation order


  {
    using namespace boost;
    typedef adjacency_list<vecS, vecS, bidirectionalS, property<vertex_color_t, default_color_type> > Graph;
    Graph g(edge_list.begin(), edge_list.end(), nodectr);
    typedef graph_traits<Graph>::vertex_descriptor Vertex;
    typedef std::list<Vertex> VisitOrder;
    VisitOrder visit_order;

    std::list<Edge > bad_list;
    cycle_detector vis(bad_list);
    boost::depth_first_search(g, visitor(vis));
    std::cout << "The graph has a cycle? " << (bad_list.size() > 0) << "\n";
    for (auto i=bad_list.begin();i!=bad_list.end();++i)
      {
	std::cout << "Classes " + node_to_name_map[i->first] + " and " + 
	  node_to_name_map[i->second] + " have a cyclic dependency\n";
      }
    if (bad_list.size() == 0) {
      boost::topological_sort(g, std::front_inserter(visit_order));
      for (auto i=visit_order.begin(); i!= visit_order.end(); ++i) {
	std::cout << " Visit " << node_to_name_map[*i] << "\n";
	Object mod = classes.at(ctxt->_string->makeString(node_to_name_map[*i]));
	ctxt->_module->deref(mod);
      }
    }
  }


  // For now - hard code a single function to preload
  /*
  ctxt->_globals->insert(std::make_pair("three",compileFunc(ctxt,"three")));
  ctxt->_globals->insert(std::make_pair("add",compileFunc(ctxt,"add")));
  ctxt->_globals->insert(std::make_pair("fixa",compileFunc(ctxt,"fixa")));
  ctxt->_globals->insert(std::make_pair("dima",compileFunc(ctxt,"dima")));
  ctxt->_globals->insert(std::make_pair("t4",compileFunc(ctxt,"t4")));
  ctxt->_globals->insert(std::make_pair("swit",compileFunc(ctxt,"swit")));
  ctxt->_globals->insert(std::make_pair("exc",compileFunc(ctxt,"exc")));
  ctxt->_globals->insert(std::make_pair("excb",compileFunc(ctxt,"excb")));
  ctxt->_globals->insert(std::make_pair("bind",compileFunc(ctxt,"bind")));
  */


  // TODO - Fix this.
  // This isn't completely correct -- we should delay metaclass construction until after
  // the functions have been parsed.  Otherwise, e.g., pi.m may not be parsed before aclass.m.
  //  compileModule(ctxt,"sclass");

  // compileModule(ctxt,"mab");
  // compileModule(ctxt,"foo");
  // compileModule(ctxt,"bar");
  // compileModule(ctxt,"sclass");
  // compileModule(ctxt,"grok");

  // Object sclassMeta = ctxt->_globals->at("?sclass");
  // Object soo = ctxt->_meta->construct(sclassMeta);
  // ctxt->_globals->insert(std::make_pair("soo",soo));

  //  std::cout << "Symbol flags size: " << sizeof(symbol_flags_t) << "\n";

  //  compileFunc(ctxt,"sclass");
  
  //  compileModule(ctxt,"localfn");
  //  compileModule(ctxt,"three");

  //  compileModule(ctxt,"nest1");
  //  compileModule(ctxt,"counter");
  //  compileModule(ctxt,"makeParabola");

  {
    // Create a variable
    Object a = ctxt->_double->makeScalar(3.14);
    // Create a capture of it
    Object cap1 = ctxt->_captured->makeScalar();
    ctxt->_captured->set(cap1,a);
    // Insert cap1 as a variable in the global workspace
    ctxt->_globals->set("cap1",cap1);

    // Insert a copy of cap1 as another variable in the global workspace
    Object cap2 = ctxt->_captured->makeScalar();
    cap2 = cap1;
    ctxt->_globals->set("cap2",cap2);


    SparseDoubleType *sp = new SparseDoubleType(ctxt);
    Object cap3 = sp->makeScalar(43);
    
    ctxt->_globals->set("cap3",cap3);
  }

  ctxt->_globals->set("size",ctxt->_module->builtin("size",size));
  ctxt->_globals->set("print",ctxt->_module->builtin("print",print));
  ctxt->_globals->set("handir",ctxt->_module->builtin("handir",handir));
  ctxt->_globals->set("all",ctxt->_module->builtin("all",all));
  ctxt->_globals->set("strcmp",ctxt->_module->builtin("strcmp",strcmp));
  ctxt->_globals->set("numel",ctxt->_module->builtin("numel",numel));
  ctxt->_globals->set("int8",ctxt->_module->builtin("int8",tot_int8));
  ctxt->_globals->set("int16",ctxt->_module->builtin("int16",tot_int16));
  ctxt->_globals->set("int32",ctxt->_module->builtin("int32",tot_int32));
  ctxt->_globals->set("int64",ctxt->_module->builtin("int64",tot_int64));
  ctxt->_globals->set("uint8",ctxt->_module->builtin("uint8",to_uint8));
  ctxt->_globals->set("uint16",ctxt->_module->builtin("uint16",to_uint16));
  ctxt->_globals->set("uint32",ctxt->_module->builtin("uint32",to_uint32));
  ctxt->_globals->set("uint64",ctxt->_module->builtin("uint64",to_uint64));
  ctxt->_globals->set("double",ctxt->_module->builtin("double",to_double));
  ctxt->_globals->set("single",ctxt->_module->builtin("single",to_single));
  ctxt->_globals->set("backtrace",ctxt->_module->builtin("backtrace",backtrace));
  ctxt->_globals->set("mksparse",ctxt->_module->builtin("mksparse",mksparse));
  ctxt->_globals->set("reduce",ctxt->_module->builtin("reduce",reduce));
  ctxt->_globals->set("full",ctxt->_module->builtin("full",full));
  ctxt->_globals->set("dbstop",ctxt->_module->builtin("dbstop",dbstop));
  ctxt->_globals->set("dbquit",ctxt->_module->builtin("dbquit",dbquit));
  ctxt->_globals->set("dbclear",ctxt->_module->builtin("dbclear",dbclear));
  ctxt->_globals->set("dblist",ctxt->_module->builtin("dblist",dblist));
  ctxt->_globals->set("dbstep",ctxt->_module->builtin("dbstep",dbstep));
  ctxt->_globals->set("dbcont",ctxt->_module->builtin("dbcont",dbcont));
  ctxt->_globals->set("dbup",ctxt->_module->builtin("dbup",dbup));
  ctxt->_globals->set("dbdown",ctxt->_module->builtin("dbdown",dbdown));
  ctxt->_globals->set("class",ctxt->_module->builtin("class",classfunc));
  ctxt->_globals->set("gc",ctxt->_module->builtin("gc",builtin_gc));

  // // Global symbols
  // for (auto i=ctxt->_globals->begin(); i != ctxt->_globals->end(); ++i)
  //   {
  //     std::cout << "Global symbol: " << i->first << ": " << i->second.brief() << "\n";
  //   }

  // Look up the global symbol for cap1
  {
    Object cap1ref = ctxt->_globals->get("cap1",ctxt);
    ctxt->_captured->set(cap1ref,ctxt->_double->makeScalar(2.78));
  }

  while (1)
    {
      char *p = readline("--> ");
      if (p && *p)
	add_history(p);
      if (!p)
	return 0;
      FMString body(p);
      body += "\n\n";
      try {
	ctxt->_compiler->compile(body,"expr:"+body.trimmed().left(10)+"...");
	Module *mod = ctxt->_compiler->module();
	if (mod)
	  {
	    Object p = ctxt->_assembler->run(mod);
	    DBOUT(std::cout << "Code object: " << p.description() << "\n");
	    DBOUT(Disassemble(ctxt,p));
	    Disassemble(ctxt,p);
	    timer.start();
	    Object params = ctxt->_list->empty();
	    ctxt->_vm->executeModule(p,params);
	    timer.stop();
	    std::cout << " Execution time " << timer.elapsed().wall/1.0e9 << "\n";
	  }
      } catch (const FM::Exception &e) {
	std::cout << "Exception: " << e.msg() << "\n";
      } catch (const VMDBQuitException &) {
      }
    }

  return 0;


}
