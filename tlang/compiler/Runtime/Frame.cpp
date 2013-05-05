//
//  Frame.cpp
//  A toyable language compiler (like a simple c++)


#include "Common.h"
#include "Frame.h"

Frame* FrameStack::m_currentFrame = NULL;
Frame* FrameStack::m_lastFrame = NULL;
vector<Frame*> FrameStack::m_frames;

/// @brief Alloc local in frame
Value* Frame::allocValue(int size, bool inreg) {
    Value *value = new Value(m_offset, size);
	m_offset += size;
	m_locals.push_back(value); 
	return value;
}

/// @brief Alloc local in frame 4byte
Value* Frame::allocValue(bool inreg) {
    Value *value = new Value(m_offset, 4);
	m_offset += 4;
	m_locals.push_back(value);
	return value;
}

FrameStack* FrameStack::getInstance() {
    static FrameStack frameStack;
    return &frameStack;
}

/// @brief Get currernt frame
Frame* FrameStack::getCurrentFrame() {
    return m_currentFrame;
}

Frame* FrameStack::allocNewFrame(int size) {
    Frame *frame = new Frame();
    return frame;
}

void FrameStack::push(Frame *frame)
{
    m_frames.push_back(frame);
    
	// push ebp
    // mov ebp, esp
    // sub esp, localSize
    IREmiter::emit(IR_PUSH, "epb");
    IREmiter::emit(IR_MOV, "epb", "esp");
	const string localString(frame->getValuesSize());
	IREmiter::emit(IR_SUB, "esp", localString);
	
}

Frame* FrameStack::pop()
{
    Frame *frame = NULL;
    
    if (m_frames.size() >= 1) {
        frame = m_frames.at(m_frames.size() - 1);
        m_frames.pop_back();
    }
    // method's postlog
	// add esp, localSize
	// pop epb
	const string localString(frame->getValuesSize());
	IREmiter::emit(IR_ADD, "esp", localString);
	IREmiter::emit(IR_POP, "ebp");	
	
	// delete all locals
	vector<Value *>::iterator ite = m_locals.begin();
	while (ite != m_locals.end()) {
		delete *ite;
		ite++;
	}
	
    return frame;
}

void FrameStack::clearAllFrames()
{
    vector<Frame*>::iterator ite = m_frames.begin();
    for (; ite != m_frames.end(); ite++) {
        Frame *frame = *ite;
        if (frame) {
            delete frame;
            frame = NULL;
        }
    }
}


