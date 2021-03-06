//
//  tl-lable.h
//  A toyable language compiler (like a simple c++)
//


#ifndef __TL_LABEL_H__
#define __TL_LABEL_H__

#include "tl-common.h"


namespace tlang {
    class Label {
        public:
            Label(const char *name = NULL);
            Label(const string &name):m_name(name){}
            ~Label(){}
            static Label newLabel();
            const string& getName();
            
        private:
            string m_name;
            static int m_index;
            
        };

} // namespace tlang 
#endif // __TL_LABLE_H__
