//
//  Type.h
//  A toyable language compiler (like a simple c++)


#ifndef __TL_TYPE_H__
#define __TL_TYPE_H__

#include "tl-common.h"

namespace  tlang {
    class VirtualTable;
    class MethodType;
    //
    // Each object has a type and the type includes data and virtual table 
    // 
    // 'class Type - the root type for all types
    class Type {
        public:
            //! constructor
            Type(); 
            Type(const string& name, bool isPublic);

            //! destructor
            virtual ~Type();
            
            //! setter/getter for the type publicity
            void setPublic(bool isPublic) { m_isPublic = isPublic;  }
            bool isPublic() const { return m_isPublic;  }
            
            //! setter/getter for type name
            void setName(const string& name) { m_name = name; }
            const string& getName() { return m_name; }
            
            //! get the type's size
            int getSize(); 
            
            //! add slot member
            void addSlot(const string& name, Type* slot) {
                m_slots.push_back(make_pair(name, slot));
            }
            //! insert slot member in specified position
            void insertSlot(int index, const string& name, Type* slot);
            
            //! check wethere has the specified member
            bool hasSlot(const string& name);

            //! get slot by name 
            Type* getSlot(const string& name); 
            
            //! get slot count
            int getSlotCount() { return (int)m_slots.size(); }
            //! get slot by index 
            void getSlot(int index, string& name, Type** type);

            //! all type should support virtual table
            virtual bool hasVirtualTable() { return true; }
            
            //! object virtual talbe for type
            virtual VirtualTable* getVirtualTable()  { return m_vtbl; }
            
            //! wether the type is compatible with other type 
            virtual bool isCompatibleWithType(Type& type); 
            
            //! wether the type is equal with specifier type
            virtual bool operator ==(Type& type) { return false; }
            
            //! type assign
            virtual Type& operator =(Type& type){ return *this; }
           
            //! check wether the tpe is enumerable
            virtual bool isEnumerable() { return false; }
            
        protected:
            bool m_isPublic;
            string m_name;
            VirtualTable* m_vtbl; 
            vector<pair<string, Type*> > m_slots;
    };
    // class' TypeDomain - contalls all type informations
    // TypeDomain is a global domain which manage all class and it's subtype
    class TypeDomain {
        public:
            typedef map<string, Type*>::iterator iterator;
        public:
            TypeDomain();
            ~TypeDomain();
            // add and get class from the domain 
            void addDomain(const string& name, Type* type);
            Type* getDomain(const string& name);
            /// class types iterator 
            size_t size() { return m_domains.size(); }
            
            /// get a type for sa specified class
            void addType(const string& domain, const string& name, Type* type);
            void getType(const string& domain, const string& name, Type** type);
        private:
            void initializeBuiltinTypes();
        private:
            map<string, map<string, Type*>* > m_domains; 
    };

    // class' VirtualTable - each object has a virtual table
    class VirtualTable {
        public:
            ~VirtualTable(){}
            VirtualTable(){}
        
            bool isPublic() const { return m_isPublic; }
            void setPublic(bool w) { m_isPublic = w; } 
            void setName(const string& name) { m_name = name; }
            const string& getName() const { return m_name; }
            int getSize() { return m_size; }
        
            void addSlot(const string& name, Type* slot); 
            Type* getSlot(const string& name);
        int  getSlotCount() const { return (int)m_slots.size(); }
        bool getSlot(int index, string& name, Type**slot); 
    
        bool operator !=(Type& type);
        bool operator ==(Type& type);
        Type& operator =(Type& type);
    private:
        vector<pair<string, Type* > > m_slots;
        string m_name;
        bool m_isPublic; 
        int m_size;
    };

    // type helper methods
    bool isTypeCompatible(Type* type1, Type* type2);
    bool isType(Type* type, const string& name);

} // namespace tlang 
#endif // __TL_TYPE_H__

