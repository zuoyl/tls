//
//  tl-frame.h
//  A toyable language compiler (like a simple c++)


#ifndef __TL_FRAME_H__
#define __TL_FRAME_H__

#include "tl-common.h"
#include "tl-ast-stmt.h"

namespace tlang {
    class Value;
    class Statement;

    class Frame {
        public:
            Frame(int size){}
            ~Frame(){}
            Value* allocValue(int size);
            Value* allocValue(bool inreg);
            ASTStatement* getIterablePoint() {
                if (!m_iterableStmts.empty())
                    return m_iterableStmts.back();
                else
                    return NULL;
            }
            void pushIterablePoint(ASTStatement* stmt){}
            void popIterablePoint(){}
            Value* getReturnValue() { return NULL; }
            void setReturnValue(Value* local) { }
        private:
            vector<Value*> m_locals;
            vector<ASTStatement*> m_iterableStmts;
        };

    class FrameStack {
        public:
            static FrameStack* getInstance();
            static Frame* getCurrentFrame();
            static Frame* getLastFrame();
            static Frame* allocNewFrame(int size);
            static void push(Frame* frame);
            static Frame* pop();
            static void clearAllFrames();
            
        private:
            FrameStack(){}
            ~FrameStack(){}
            
            static vector<Frame*> m_frames;
            static Frame *m_currentFrame;
            static Frame *m_lastFrame;
            
    };

} // namespace tlang 
#endif // __TL_FRAME_H__
