//
//  BultinType.h
//  A toyable language compiler (like a simple c++)
//

#ifndef TCC_BUILTINTYPE_H
#define TCC_BUILTINTYPE_H

#include "Type.h"

class IntType : public Type {
public:
    IntType():Type("int", true) { m_size = 4; }
    /// add slot type member in current type
    void addSlot(const string &name, Type *slot){};
    /// get slot type member in current type
    Type* getSlot(const string &name) const;
    /// get specified slot type 
    Type* getSlot(int index);
    /// get slot count
    int getSlotCount() const;
    /// wether the type is compatible with other type 
    bool isCompatibleWithType(Type *type);
   
    /// wether the type is equal with specifier type
    bool operator ==(Type *type);
   
    Type& operator =(Type *type);
   
    /// all type should support virtual table
    bool hasVirtualTable() const;
    /// object virtual talbe for type
    ObjectVirtualTable* getVirtualTable() const;
};

class BoolType : public Type {
    
};


class StringType : public Type {
public:
    StringType();
    StringType(const string &name, Scope *scope);
    ~StringType();
    
    bool isPublic() const;
    void setScope(Scope *scope);
    Scope* getScope() const;
    void setName(const string &name);
    const string& getName() const;
    void setSize();
    int getSize();
    bool operator !=(Type &type);
    bool operator ==(Type &type);
    Type& operator =(Type &type);    
    void addSlot(const string &name, Type *slot);
    void getSlot(const string &name, Type **slot) const;
    int  getSlotCount() const;
    void getSlot(int index, Type **slot);   
    bool hasVirtualTable() const { return false; }
    ObjectVirtualTable* getVirtualTable() const { return NULL; }    

    
private:
    string m_name;
    Scope *m_scope;
    int m_size;      
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
    Type *getKeyType();
    Type *getValType();
private:
    
    
};

class SetType : public Type {
public:
    Type *getValType();
    
};

#endif // TCC_BUILTINTYPE_H
