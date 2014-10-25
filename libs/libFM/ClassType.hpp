#ifndef __Class_hpp__
#define __Class_hpp__

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

  // Start with a  class.  Assume value type.
  class ClassMetaData {
  public:
    HashMap<int> m_properties; // Maps property names to index in the list for each cell
    HashMap<Object> m_methods; // Methods for the class - maps method names to code objects
    Object m_defaults;         // Default values for properties
    FMString m_name;           // Name of the  class.
    ClassMetaData(ThreadContext *_ctxt);
  };
  
  class ClassMetaType : public HandleType<ClassMetaData> {
  public:
    ClassMetaType(ThreadContext *ctxt) {_ctxt = ctxt;}
    void addProperty(Object &meta, const Object &name, const Object &default_value);
    void addMethod(Object &meta, const Object &name, const Object &definition);
    void setName(Object &a, const FMString &name) {this->readWriteData(a)->m_name = name;}
    virtual DataCode code() const {return TypeMeta;}
    virtual const FMString& name() const {static FMString _name = "meta"; return _name;}
    //    Object empty();
    virtual FMString describe(const Object &a);
    virtual bool equals(const Object &a, const Object &b)
    {
      return false;
    }
    ClassMetaData* makeEmptyDataType() {return new ClassMetaData(_ctxt);}
    Object construct(const Object &meta); // Construct an object with this class
  };

  class ClassData {
  public:
    Object metaClass; // The meta class for this class.
    Object m_data; // The cell array of data for this class.
    ClassData(ThreadContext *ctxt) : metaClass(ctxt), m_data(ctxt) {}
  };

  class ClassType : public AggregateType<ClassData> {
  public:
    ClassType(ThreadContext *ctxt) {_ctxt = ctxt;}
    virtual DataCode code() const {return TypeClass;}
    virtual const FMString& name() const {static FMString _name = "class"; return _name;}
    virtual FMString describe(const Object &a);
    virtual Object getField(const Object &a, const Object &b);
    virtual void setField(Object &a, const Object &args, const Object &b);
    //    virtual Object getParens(const Object &a, const Object &args);
    //    virtual void setParens(Object &a, const Object &args, const Object &b);
    virtual bool hasMethod(const Object &a, const Object &name, Object &ret);
    virtual bool equals(const Object &a, const Object &b)
    {
      // FIXME - allow equality tests
      return false;
    }
    Object empty() {throw Exception("Cannot create empty classes");}
    ClassData* makeEmptyDataType() {throw Exception("Cannot create empty classes");}
  };
};


#endif
