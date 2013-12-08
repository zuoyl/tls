//
//  tl-scope.h - manage the Object scope 
//  A toyable language compiler (like a simple c++)


#ifndef __TL_SCOPE_H__
#define __TL_SCOPE_H__

#include "tl-common.h"
#include "tl-type.h"

namespace tlang {
    class Object;

    /// "class Scope
    /// Scope manage all Objects in current life scope, such as method block,class block,etc.
    class Scope {
        public:
            Scope();
            /// Scope constructor
            /// @param name the name of the scope
            /// @param parent the parent scope of the scope
            Scope(const string& name, Scope* parent);
            
            /// Scope destructor
            virtual ~Scope();
            
            /// Get scope's name
            /// @ret the name of scope
            const string& getScopeName() { return m_scopeName; }
            
            /// Get scope's parent scope
            /// @ret the parent scope
            Scope* getParentScope() { return m_parentScope; }
            
            /// Set scope's parent scope
            /// @param parent the parent scope
            void setParentScope(Scope *parent) { m_parentScope = parent; }
            
            /// Define a new Object in the scope, if the Object exist, excpetion will be throwed.
            /// @param Object the new Object 
            void defineObject(Object *object);
            
            /// Check to see wether has a specified Object
            /// @param name the Object's name
            /// @param nested indicate wether to resolve recursively
            /// @ret true indicate that has the Object, else none
            bool hasObject(const string &name, bool nested = false);
            
            /// Resolve a Object by the specified name if the Object exist
            /// @param name the Object's name
            /// @param nested indicate wether to resolve recursively, for example the parent's scope
            /// @ret the Object found by name
            Object* resolveObject(const string &name, bool nested = false);
            
            /// Define a new type in the scope, if the type exist, exception will be throwed
            /// @param type the new type
            void defineType(Type *type);
            
            /// Resolve a type by the specified name if the type exist
            /// @param name the type's name
            /// @param nested indicate wether to resolve recursively, for example the parent's scope
            Type* resolveType(const string &name, bool nested = true);

            /// Resolve a type by the specified name if the type exist
            /// @param name the type's name
            /// @param nested indicate wether to resolve recursively, for example the parent's scope
            /// ret true indicate that has the Object, else none
            bool hasType(const string &name, bool nested = true);

            int getObjectsCount() { return (int)m_objects.size(); }
        protected:
            /// Name of the scope
            string m_scopeName;
            /// Parent scope of the scope
            Scope *m_parentScope;
            
            /// Holder for all Objects in the scope
            map<const string, Object*> m_objects;
            
            /// Holder for all types in the scope
            TypeDomain m_types;
        };

        /// 'class Object 
    class Object {
        public:
            enum { LocalObject, HeapObject };
        public:
            explicit Object(const string &name, Type *type):m_name(name),m_type(type){}
            explicit Object(const char *name, Type *type):m_name(name), m_type(type){} 
            ~Object(){} 
            void setName(const string &name) { m_name = name; }
            void setType(Type *type) { m_type = type; }
            const string& getName() { return m_name; }
            Type* getType() { return m_type; }
            void setStorage(int storage) { m_storage = storage; }
            int  getStorage() { return m_storage; }
            int  getSize() { return m_size; }
            int  getOffset() { return m_offset; }
            void setOffset(int offset) { m_offset = offset; }
        protected: 
            string m_name;
            Type *m_type;
            int m_size;
            int m_offset;
            int m_storage;      // indicate wether in stack or in memory
    }; 
} // namespace tlang 
#endif // __TL_SCOPE_H__
