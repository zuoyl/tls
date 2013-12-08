//
//  Frame.cpp
//  A toyable language compiler (like a simple c++)


#include "tl-frame.h"
#include "tl-value.h"


using namespace tlang;

Frame* FrameStack::m_currentFrame = NULL;
Frame* FrameStack::m_lastFrame = NULL;
vector<Frame*> FrameStack::m_frames;

/// @brief Alloc local in frame
Value* Frame::allocValue(int size) {
#if 0
    Value* value = new Value(m_offset, size);
	m_offset += size;
	m_locals.push_back(value); 
	return value;
#else
    return NULL;
#endif
}

/// @brief Alloc local in frame 4byte
Value* Frame::allocValue(bool inreg) {
#if 0
    Value* value = new Value(m_offset, 4);
	m_offset += 4;
	m_locals.push_back(value);
	return value;
#else
    return NULL;
#endif
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
#if 0
    Frame* frame = new Frame();
    return frame;
#else
    return NULL;
#endif
}

void FrameStack::push(Frame* frame)
{
    m_frames.push_back(frame);
    
}

Frame* FrameStack::pop()
{
    Frame* frame = NULL;
    
    if (m_frames.size() >= 1) {
        frame = m_frames.at(m_frames.size() - 1);
        m_frames.pop_back();
    }
#if 0	
	// delete all locals
	vector<Value* >::iterator ite = m_locals.begin();
	while (ite != m_locals.end()) {
		delete* ite;
		ite++;
	}
#endif
    return frame;
}

void FrameStack::clearAllFrames()
{
    vector<Frame*>::iterator ite = m_frames.begin();
    for (; ite != m_frames.end(); ite++) {
        Frame *frame =* ite;
        if (frame) {
            delete frame;
            frame = NULL;
        }
    }
}


