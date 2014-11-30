#ifndef __ClassType_hpp__
#define __ClassType_hpp__

#include "Object.hpp"
#include "Type.hpp"
#include "HashMap.hpp"
#include "HandleType.hpp"

namespace FM
{
  struct ThreadContext;

  /*
  // What we want is something like HandleClass - a class with properties and events
  struct ClassMethod {
    bool Abstract;
    AccessType Access;
    ClassMetaData* DefiningClass;
    bool Hidden;
    FMString Name;
    bool Sealed;
    bool Static;
    Object Value;
  };
  
  struct ClassProperty {
    FMString Name;
    bool AbortSet;
    bool Abstract;
    AccessType Access;
    bool Constant;
    Object DefaultValue;
    ClassMetaData* DefiningClass;
    AccessType GetAccess;
    bool Dependent;
    bool Transient;
    Object GetMethod;
    bool GetObservable;
    bool HasDefault;
    bool Hidden;
    AccessType SetAccess;
    Object SetMethod;
    bool SetObservable;
  };

  struct ClassEvent {
    FMString Name;
    bool Hidden;
    AccessType ListenAccess;
    AccessType NotifyAccess;
    ClassMetaData* DefiningClass;
  };

  struct ClassMetaData {
    bool Abstract; // True if this is an abstract class
    std::vector<ClassMetaData*> AllowedSubclasses;
    bool ConstructOnLoad;
    ClassPackage* myPackage;
    bool Enumeration;
    std::vector<ClassEvent*> EventList;
    std::vector<ClassEnumerationMember*> EnumerationMemberList;
    bool Hidden;
    std::vector<ClassMetaData*> InferiorClasses;
    std::vector<ClassMethod*> MethodList;
    FMString Name;
    std::vector<ClassProperty*> PropertyList;
    bool Sealed;
    std::vector<ClassMetaData*> SuperiorClasses;
  };

  */
  //  This is a lot of C++ code... Is it really necessary?

  class ClassMethodMetaData {
  public:
    bool m_static;
    Object m_definition;
    ClassMethodMetaData(ThreadContext *_ctxt) : m_static(false), m_definition(_ctxt) {}
  };

  class ClassPropertyMetaData {
  public:
    bool m_constant;   // Set to true if the property is constant
    bool m_dependent;  // Set to true if the property is dependent
    Object m_default;  // Default value
    Object m_getter;   // Getter (if it has one)
    Object m_setter;   // Setter (if it has one)
    int m_index;       // Index into the object instance's list of properties
    ClassPropertyMetaData(ThreadContext *_ctxt) : m_constant(false),
      m_default(_ctxt), m_dependent(false), m_getter(_ctxt),
      m_setter(_ctxt), m_index(0) {}
  };

  // Start with a  class.  Assume value type.
  class ClassMetaData {
  public:
    Object m_name;             // Name of the  class.
    HashMap<ClassPropertyMetaData*> m_properties; // Properties for the class 
    HashMap<ClassMethodMetaData*> m_methods; // Methods for the class 
    Object m_defaults;
    ClassMetaData(ThreadContext *_ctxt);
  };
  
  class ClassMetaType : public AggregateType<ClassMetaData,HandleSemantics> {
  public:
    ClassMetaType(ThreadContext *ctxt) {_ctxt = ctxt;}
    void addProperty(Object &meta, const Object &name, bool constant, 
		     bool dependent, const Object &default_value,
		     const Object &getter, const Object &setter);
    void addMethod(Object &meta, const Object &name, const Object &definition, bool is_static);
    void setName(Object &a, const Object &name) {this->rw(a)->m_name = name;}
    void addSuperClass(Object &meta, const Object &super);
    virtual DataCode code() const {return TypeMeta;}
    virtual const FMString& name() const {static FMString _name = "meta"; return _name;}
    //    Object empty();
    virtual FMString describe(const Object &a);
    virtual FMString brief(const Object &a);
    virtual bool equals(const Object &a, const Object &b)
    {
      return false;
    }
    Object construct(const Object &a);
    virtual Object deref(const Object &a); // Construct an object without parameters
    virtual Object getParens(const Object &a, const Object &b); // Construct with parameters
    virtual Object getField(const Object &meta, const Object &fieldname);
  };

  class ClassData {
  public:
    Object metaClass; // The meta class for this class.
    Object m_data; // The cell array of data for this class.
    ClassData(ThreadContext *ctxt) : metaClass(ctxt), m_data(ctxt) {}
  };

  class ClassType : public AggregateType<ClassData,ValueSemantics> {
  public:
    ClassType(ThreadContext *ctxt) {_ctxt = ctxt;}
    virtual DataCode code() const {return TypeClass;}
    virtual const FMString& name() const {static FMString _name = "class"; return _name;}
    virtual FMString describe(const Object &a);
    virtual FMString brief(const Object &a);
    virtual Object getField(const Object &a, const Object &b);
    virtual Object getFieldNoGetters(const Object &a, const Object &b);
    virtual void setField(Object &a, const Object &args, const Object &b);
    virtual void setFieldNoSetters(Object &a, const Object &args, const Object &b);
    //    virtual Object getParens(const Object &a, const Object &args);
    //    virtual void setParens(Object &a, const Object &args, const Object &b);
    virtual bool hasMethod(const Object &a, const Object &name, Object &ret);
    const Object & className(const Object &a) const {return _ctxt->_meta->ro(this->ro(a)->metaClass)->m_name;}
    virtual bool equals(const Object &a, const Object &b)
    {
      // FIXME - allow equality tests
      return false;
    }
    Object empty() {throw Exception("Cannot create empty classes");}
  };
};


#endif
