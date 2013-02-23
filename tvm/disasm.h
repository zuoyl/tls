/// tvm
/// virtual machine for toyable language

#ifndef TVMDISASM_INC
#define TVMDISASM_INC

#include "tvm.h"

class   InstructDisasm  {
public:
    static InstructDisasm* instance();
    void disasm(u8 *inbuf, u32 size, u8 *outbuf, u32 outsize);
    void disasm(u8 *inbuf, u32 size, const char *file);
    void disasm(const char *infile, const char *outfile);
    void disasm(Instruct &inst, string &str);
private:
    void makeConditString(u8 condit, string &str);

    void disasm_B(Instruct &inst, string &str);
    void disasm_BL(Instruct &inst, string &str);
    void disasm_BLX1(Instruct &inst, string &str);
    void disasm_BLX2(Instruct &inst, string &str);
    void disasm_BX(Instruct &inst, string &str);
    void disasm_AND(Instruct &inst, string &str);
    void disasm_EOR(Instruct &inst, string &str);
    void disasm_SUB(Instruct &inst, string &str);
    void disasm_RSB(Instruct &inst, string &str);
    void disasm_ADD(Instruct &inst, string &str);
    void disasm_ADC(Instruct &inst, string &str);
    void disasm_SBC(Instruct &inst, string &str);
    void disasm_RSC(Instruct &inst, string &str);
    void disasm_TST(Instruct &inst, string &str);
    void disasm_TEQ(Instruct &inst, string &str);
    void disasm_CMP(Instruct &inst, string &str);
    void disasm_CMN(Instruct &inst, string &str);
    void disasm_ORR(Instruct &inst, string &str);
    void disasm_MOV(Instruct &inst, string &str);
    void disasm_BIC(Instruct &inst, string &str);
    void disasm_MVN(Instruct &inst, string &str);
    void disasm_MLA(Instruct &inst, string &str);
    void disasm_MUL(Instruct &inst, string &str);
    void disasm_SMLAL(Instruct &inst, string &str);
    void disasm_SMULL(Instruct &inst, string &str);
    void disasm_UMLAL(Instruct &inst, string &str);
    void disasm_UMULL(Instruct &inst, string &str);
    void disasm_CLZ(Instruct &inst, string &str);
    void disasm_MRS(Instruct &inst, string &str);
    void disasm_MSR(Instruct &inst, string &str);
    void disasm_LDR(Instruct &inst, string &str);
    void disasm_LDRB(Instruct &inst, string &str);
    void disasm_LDRBT(Instruct &inst, string &str);
    void disasm_LDRH(Instruct &inst, string &str);
    void disasm_LDRSB(Instruct &inst, string &str);
    void disasm_LDRSH(Instruct &inst, string &str);
    void disasm_LDRT(Instruct &inst, string &str);
    void disasm_STR(Instruct &inst, string &str); 
    void disasm_STRB(Instruct &inst, string &str);
    void disasm_STRBT(Instruct &inst, string &str);
    void disasm_STRH(Instruct &inst, string &str);
    void disasm_STRT(Instruct &inst, string &str);
    void disasm_LDM(Instruct &inst, string &str);
    void disasm_STM(Instruct &inst, string &str);
    void disasm_SWP(Instruct &inst, string &str);
    void disasm_SWPB(Instruct &inst, string &str);
    void disasm_BKPT(Instruct &inst, string &str);
    void disasm_SWI(Instruct &inst, string &str);
    void disasm_CDP(Instruct &inst, string &str);
    void disasm_LDC(Instruct &inst, string &str); 
    void disasm_MCR(Instruct &inst, string &str); 
    void disasm_MRC(Instruct &inst, string &str); 
    void disasm_STC(Instruct &inst, string &str);
    void disasm_NOP(Instruct &inst, string &str);
    void disasm_Undefined(Instruct &inst, string &str);
    void getoperandStr(Instruct &inst, string &str);
    
private:
    InstructDisasm(){}
     ~InstructDisasm(){}
    

};

#endif //TVMDISASM_INC
