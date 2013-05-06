//
//  Type.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_TYPE_H
#define TCC_TYPE_H

#include "Common.h"

class Scope;
class Type;
class ClassType;
class InterfaceType;
class StructType;
class MethodType;
class ObjectVirtualTable;
class TypeSpec;
class ASTVisitor;
class AST;
// type helper methods
bool isTypeCompatible(Type* type1, Type *type2);
bool isType(Type *type, const string &name);
Type* getTypeBySpec(TypeSpec *spec);

// class Type - the root type for all types
class Type 
{
public:
    //! constructor
    Type(); 
    Type(const string name, bool isPublic);

    //! destructor
    virtual ~Type();
    
    //! setter/getter for the type publicity
    virtual void setPublic(bool isPublic)
			{ m_isPublic = isPublic;  }
    virtual bool isPublic() const
			{	return m_isPublic;  }
    
    //! set type name
    virtual void setName(const string &name)
        { m_name = name; }
    
    // !get type name
    virtual const string& getName() const 
        { return m_name; }
    
    //! get the type's size
    virtual int getSize() { 
        return m_size; 
    }
    
    //! getter/setter for slot type member in current type
    virtual void addSlot(const string &name, Type *slot) 
    { }
    virtual Type* getSlot(const string &name) const
    { return NULL; }
        
    //! get slot by index
    virtual int getSlotCount() const
    { return 0; }
    virtual Type* getSlot(int index) 
    { return NULL;} 
    
    //! wether the type is compatible with other type 
    virtual bool isCompatibleWithType(Type *type) 
    { return false; }
   
    //! wether the type is equal with specifier type
    virtual bool operator ==(Type *type)
    { return false; }
    
    //! type assign
    virtual Type& operator =(Type *type){};
   
    //! all type should support virtual table
    virtual bool hasVirtualTable()
    { return false; }
    
    //! object virtual talbe for type
    virtual ObjectVirtualTable* getVirtualTable() 
    { return NULL; }

    virtual bool isEnumerable() 
    { return false; }
    
protected:
    bool m_isPublic;
    string m_name;
    int m_size;
    
};
// class TypeDomain - which contalls all type
class TypeDomain {
public:
    TypeDomain();
    ~TypeDomain();
    void addType(const string &name, Type *type);
    void getType(const string &name, Type **type);
private:
    map<string, Type *> m_types;
};


class ObjectVirtualTable {
public:
    ~ObjectVirtualTable();
    ObjectVirtualTable();
    
    bool isPublic() const { return true; }
    void setScope(Scope *scope) {}
    Scope* getScope() const { return NULL; }
    void setName(const string &name) { m_name = name; }
    const string& getName() const { return m_name; }
    int getSize() { return m_size; }
    
    void addSlot(const string &name, Type *slot) {}
    Type* getSlot(const string &name) const { return NULL; }
    int  getSlotCount() const { return (int)m_slots.size(); }
    Type* getSlot(int index) { return NULL; }
    
    bool operator !=(Type &type);
    bool operator ==(Type &type);
    Type& operator =(Type &type);
private:
    vector<std::pair<const string, MethodType *> > m_slots;
    string m_name;
    int m_size;
    
};

class ClassType : public Type {
public:
    ClassType();
    ClassType(const string &name, Scope *scope, bool isPublic, bool isFrozen = false);
    ~ClassType();

    bool isPublic() const { return m_isPublic; }
    void setScope(Scope *scope) { m_scope = scope; }
    Scope* getScope() const { return m_scope; }
    void setName(const string &name) { m_name = name; }
    const string& getName() const { return m_name; }
    int getSize() { return m_size; }
    
    void addSlot(const string &name, Type *slot);
    Type* getSlot(const string &name) const;
    int  getSlotCount() const { return (int)m_slots.size(); }
    Type* getSlot(int index);
    
    bool operator !=(Type &type);
    bool operator ==(Type &type);
    Type& operator =(Type &type);
    bool hasVirtualTable() const { return false; }
    ObjectVirtualTable* getVirtualTable() const { return NULL; } 
    bool isFrozen() { return m_isFrozen;}
  
private:
    vector<std::pair<const string, Type*> > m_slots;
    vector<std::pair<const string, MethodType*> > m_methods;
    vector<std::pair<const string, Type *> > m_vars;
    vector<std::pair<const string, ClassType*> > m_baseClass;
    vector<std::pair<const string, InterfaceType*> > m_baseInterface;
    ObjectVirtualTable *m_vtbl;
    string m_name;
    Scope *m_scope;
    int m_size;
    bool m_isPublic;
    bool m_isFrozen;
};

class ProtocolType : public Type {
public:
    ProtocolType();
    ProtocolType(const string &name, Scope *scope, bool isPublic);
    ~ProtocolType();
    
    
    bool isPublic() const { return m_isPublic; }
    void setScope(Scope *scope) { m_scope = scope; }
    Scope* getScope() const { return m_scope; }
    void setName(const string &name) { m_name = name; }
    const string& getName() const { return m_name; }
    
    int getSize() { return m_size; }
   
    void addSlot(const string &name, Type *slot);
    Type* getSlot(const string &name) const;
    int  getSlotCount() const;
    Type* getSlot(int index);
    
    bool operator !=(Type &type);
    bool operator ==(Type &type);
    Type& operator =(Type &type);
    bool hasVirtualTable() const { return false; }
    ObjectVirtualTable* getVirtualTable() const { return NULL; }    
   
private:
    string m_name;
    Scope *m_scope;
    int m_size;
    ObjectVirtualTable *m_vtbl;
    bool m_isPublic;
};

class StructType : public Type {
public:
    StructType();
    StructType(const string &name, Scope *scope, bool isPublic);
    ~StructType();
    
    bool isPublic() const { return m_isPublic; }
    void setScope(Scope *scope) { m_scope = scope; }
    Scope* getScope() const { return m_scope; }
    void setName(const string &name) { m_name = name; }
    const string& getName() const { return m_name; }
    int getSize() { return m_size; }
    
    void addSlot(const string &name, Type *slot);
    Type* getSlot(const string &name) const;
    int  getSlotCount() const { return (int)m_slots.size(); }
    Type* getSlot(int index);
    
    bool operator !=(Type &type);
    bool operator ==(Type &type);
    Type& operator =(Type &type);    
    bool hasVirtualTable() const { return false; }
    ObjectVirtualTable* getVirtualTable() const { return NULL; }
    
private:
    string m_name;
    Scope *m_scope;
    int m_size; 
    bool m_isPublic;
    vector<std::pair<string, Type *> > m_slots;
};


class MethodType : public Type {
public:
    MethodType();
    MethodType(const string &name, Scope *scope, bool isPublic);
    ~MethodType();

    
    bool isPublic() const { return m_isPublic; }
    void setScope(Scope *scope) { m_scope = scope; }
    Scope* getScope() const { return m_scope; }
    void setName(const string &name) { m_name = name; }
    const string& getName() const { return m_name; }
    int getSize() { return m_size; }
    
    void addSlot(const string &name, Type *slot);
    Type* getSlot(const string &name) const;
    int  getSlotCount() const { return 0; }
    Type* getSlot(int index) ;
    
    bool operator !=(Type &type);
    bool operator ==(Type &type);
    Type& operator =(Type &type);
    bool hasVirtualTable() const { return false; }
    ObjectVirtualTable* getVirtualTable() const { return NULL; }    
   
    void setLinkAddress(int addr) { m_linkAddress = addr; }
    int getLinkAddress() const { return m_linkAddress; }
    
    Type *getReturnType() { return m_retType; }
    void setReturnType(Type *type) { m_retType = type; }
    
private:
    string m_name;
    Scope *m_scope;
    int m_size;  
    bool m_isPublic;
    bool m_isOfProtocol;
    bool m_isOfClass;
    Type *m_retType;
    int m_linkAddress;
};
#endif // TCC_TYPE_H
