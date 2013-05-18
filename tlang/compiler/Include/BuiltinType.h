//
//  BultinType.h
//  A toyable language compiler (like a simple c++)
//

#ifndef TCC_BUILTINTYPE_H
#define TCC_BUILTINTYPE_H

#include "Type.h"


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
    Type* getSlot(int index);
    int getSlotCount() const; 
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
    map<string, Type*> m_slots;
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


class IntType : public Type {
public:
    IntType(){}
    virtual ~IntType(){}
};

class BoolType : public Type {
    
};


class StringType : public Type {
public:
    StringType();
    StringType(const string &name, Scope *scope);
    ~StringType();
    
};


class FloatType : public Type {
public:
    Type *getValType();
    
};

class MapType : public Type {
public:
    MapType(const string &keyType, const string &valType);
    MapType(const Type *keyType, const Type *valType);
    void setTypeInfo(const string &keyType, const string &valType);
    void setTypeInfo(const Type *keyType, const Type *valType);
    Type *getKeyType() { return m_keyType; }
    Type *getValType() { return m_valType; }
private:
    Type *m_keyType;
    Type *m_valType;
};

class SetType : public Type {
public:
    Type *getValType() { return m_valType; }

private:
    Type *m_valType;
    
};

#endif // TCC_BUILTINTYPE_H
