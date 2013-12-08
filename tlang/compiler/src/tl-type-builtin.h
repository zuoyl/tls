//
//  tl-type-builtin.h
//  A toyable language compiler (like a simple c++)
//

#ifndef __TL_TYPE_BUILTIN_H__
#define __TL_TYPE_BUILTIN_H__

#include "tl-type.h"

namespace tlang {
    class MethodType;
    class ClassType : public Type {
    public:
            ClassType();
            ClassType(const string &name, bool isPublic, bool isFinal = false);
            ~ClassType();

            bool operator !=(Type &type);
            bool operator ==(Type &type);
            Type & operator =(Type &type);
            bool isFinal() { return m_isFinal;}
            void setFinal(bool w) { m_isFinal = w; } 
        private:
            vector<pair<const string, MethodType*> > m_methods;
            vector<pair<const string, Type *> > m_vars;
            vector<pair<const string, ClassType*> > m_baseClass;
            string m_name;
            bool m_isPublic;
            bool m_isFinal;
        };

    class MethodType : public Type {
        public:
            MethodType():Type(){}
            MethodType(const string &name, bool isPublic):
                Type(name, isPublic){}
            ~MethodType(){}

            bool hasVirtualTable() { return false; }
            VirtualTable* getVirtualTable()  { return NULL; }
            bool isCompatibleWithType(Type& type) { return false;} 
            bool isEnumerable() { return false; }
            
            bool operator !=(Type &type) { return false; }
            bool operator ==(Type &type) { return false;}
            Type& operator =(Type &type) { return *this; }
           
            void setLinkAddress(int addr) { m_linkAddress = addr; }
            int getLinkAddress() const { return m_linkAddress; }
            
            Type *getReturnType() { return m_retType; }
            void setReturnType(Type *type) { m_retType = type; }
            bool isOfClassMember() { return m_isOfClass; }
            void setOfClassMember(bool isOfClass) { m_isOfClass = isOfClass; }
        private:
            string m_name;
            bool m_isOfClass;
            Type *m_retType;
            int m_linkAddress;
    };

    class BuiltinType: public Type {
        public:
            BuiltinType(){} 
            BuiltinType(const string &name):Type(name, true){}
            ~BuiltinType(){}
    };

    class IntType : public BuiltinType {
        public:
            IntType():BuiltinType("int"){}
            ~IntType(){}
        };

    class BoolType : public BuiltinType {
        public: 
            BoolType():BuiltinType("bool"){}
            ~BoolType(){}
    };


    class StringType : public BuiltinType {
        public:
            StringType():BuiltinType("string"){}
            StringType(const string &name):BuiltinType("string"){}
            ~StringType(){}
            
    };


    class FloatType : public BuiltinType {
        public:
            FloatType():BuiltinType("float"){}
            ~FloatType(){}
            Type* getValType();
            
        };

    class VoidType : public BuiltinType {
        public:
            VoidType():BuiltinType("void"){}
            ~VoidType(){}
        };
    class MapType : public BuiltinType {
        public:
            MapType(); 
            MapType(const string &keyType, const string &valType);
            MapType(Type* keyType, Type* valType);
            void setItemType(const string &keyType, const string &valType);
            void setItemType(Type* keyType, Type* valType);
            Type* getKeyType() { return m_keyType; }
            Type* getValType() { return m_valType; }
        private:
            Type* m_keyType;
            Type* m_valType;
            string m_keyTypeName;
            string m_valTypeName;
        };

        class SetType : public BuiltinType 
        {
        public:
            SetType():BuiltinType("set"),m_valType(NULL){}
            ~SetType(){}
            Type* getValType() { return m_valType; }
            void setValType(Type* type){ m_valType = type; }
            void setValType(const string &type){ m_valTypeName = type; }
        private:
            Type* m_valType;
            string m_valTypeName; 
        };
} // namespace tlang 
#endif // __TL_TYPE_BUILTIN_H_


