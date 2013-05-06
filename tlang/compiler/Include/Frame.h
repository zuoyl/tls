//
//  Frame.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_FRAME_H
#define TCC_FRAME_H

#include "Common.h"

class Value;
class Statement;

class Frame {
public:
    Frame(int size){}
    ~Frame(){}
    Value* allocValue(int size);
    Value* allocValue();
	Statement* getIterablePoint();
	void pushIterablePoint(Statement* stmt);
	void popIterablePoint();
	Value* getReturnValue();
	void setReturnValue(Value* local);
private:
	int m_size;
	int m_offset;
	vector<Value*> m_locals;
	vector<Statement*> m_iterableStmtStack;
};

class FrameStack {
public:
    static FrameStack* getInstance();
    static Frame* getCurrentFrame();
    static Frame* getLastFrame();
    static Frame* allocNewFrame(int size);
    static void push(Frame *frame);
    static Frame* pop();
    static void clearAllFrames();
    
private:
    FrameStack(){}
    ~FrameStack(){}
    
    static vector<Frame *> m_frames;
    static Frame *m_currentFrame;
    static Frame *m_lastFrame;
    
};



#endif // TCC_FRAME_H
