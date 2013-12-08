//
//  IREmiter.h - IR instruction emiter
//  A toyable language compiler (like a simple c++)
//


#ifndef __TL_IR_EMITER_H__
#define __TL_IR_EMITER_H__

#include "tl-common.h"
#include "tl-ir.h"
#include "tl-ir-nodeblock.h"
#include <cstdarg>


namespace tlang {
    class Label;
    class Value;

    /// Generate intermediate presentation instruction
    class IREmiter {
    public:
        IREmiter();
        ~IREmiter();
        /// set assemble file output path
        void setOutputPath(const string &path) { m_path = path; } 
        /// set assemble file name 
        void setAssembleFile(const string& file){ m_file = file; } 
        /// set code block list in which the generated code will be saved 
        void setIRBlockList(IRBlockList *blockList){ m_blocks = blockList; }
        /// prepare for code generation 
        void prepare(); 
        // create tlang object format and set it as the currernt tof 
        void createTof(const string &name); 
        // close current tof 
        void closeTof();
        // generate a block 
        void emitBlock(); 
        // generate a label 
        void emitLabel(Label &label);
        // single instruction without operand 
        void emit(int inst);
        // instruction with one operands
        void emit(int inst, Value &val);
        // instruction with two operands
        void emit(int inst, Value &val1, Value &val2);
        // instruction with three operands 
        void emit(int inst, Value &left, Value &right, Value &result);
        void emitException();
        void emitCMP(Value& val1, Value &val2, Label &trueLabel, Label &falseLabel);
        void emitCMP(Value& val1, int val2, Label &trueLabel, Label &falseLabel);
        void emitJump(Label &lable);
        void emitMethodCall(Value &val);
    private:
        void putbyte(unsigned long w);
        void putasm(const char* fmt, ... );
    private:
        bool m_isOutputAssembleFile;
        string m_path; 
        string m_file; 
        IRBlockList *m_blocks;
        ofstream m_tofFile;
        ofstream m_asmFile;  
        IRBlock* m_curBlock;
    };


}// namespace tlang 
#endif // endif __TL_IR_EMITER_H__

