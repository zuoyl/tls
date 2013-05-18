//
//  Type.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_TYPE_H
#define TCC_TYPE_H

#include "Common.h"
class Scope;
class ObjectVirtualTable;
class MethodType;

// 'class Type - the root type for all types
class Type 
{
public:
    //! constructor
    Type(){} 
    Type(const string name, bool isPublic)
        :m_name(name), m_isPublic(isPublic){}

    //! destructor
    virtual ~Type();
    
    //! setter/getter for the type publicity
    virtual void setPublic(bool isPublic) { m_isPublic = isPublic;  }
    virtual bool isPublic() const { return m_isPublic;  }
    
    //! setter/getter for type name
    virtual void setName(const string &name) { m_name = name; }
    virtual const string& getName() { return m_name; }
    
    //! get the type's size
    virtual int getSize() { return m_size; }
    
    //! getter/setter for slot type member in current type
    virtual void addSlot(const string &name, Type *slot);
    virtual Type* getSlot(const string &name);
        
    //! get slot by index
    virtual int getSlotCount();
    virtual Type* getSlot(int index);
    
    //! wether the type is compatible with other type 
    virtual bool isCompatibleWithType(Type &type) { return false; }
   
    //! wether the type is equal with specifier type
    virtual bool operator ==(Type &type) { return false; }
    
    //! type assign
    virtual Type& operator =(Type &type){ return *this; }
   
    //! all type should support virtual table
    virtual bool hasVirtualTable() { return false; }
    
    //! object virtual talbe for type
    virtual ObjectVirtualTable* getVirtualTable()  { return NULL; }

    virtual bool isEnumerable() { return false; }
    
protected:
    bool m_isPublic;
    string m_name;
    int m_size;
    map<string, Type *> m_slots;
    vector<Type *> m_slotseqs;
    
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

// type helper methods
bool isTypeCompatible(Type* type1, Type *type2);
bool isType(Type *type, const string &name);

#endif // TCC_TYPE_H
