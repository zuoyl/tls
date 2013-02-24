//
//  BultinType.h
//  A toyable language compiler (like a simple c++)
//

#ifndef TCC_BUILTINTYPE_H
#define TCC_BUILTINTYPE_H

#include "compiler/Type.h"

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

class IntType : public Type {
    
};

class BoolType : public Type {
    
};

class FloatType : public Type {
    
};

class MapType : public Type {
    
};

class ListType : public Type {
    
};

#endif // TCC_BUILTINTYPE_H
