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
    void disasm(Instruct &inst, std::string &str);
private:
    void makeConditString(u8 condit, std::string &str);

    void disasm_B(Instruct &inst, std::string &str);
    void disasm_BL(Instruct &inst, std::string &str);
    void disasm_BLX1(Instruct &inst, std::string &str);
    void disasm_BLX2(Instruct &inst, std::string &str);
    void disasm_BX(Instruct &inst, std::string &str);
    void disasm_AND(Instruct &inst, std::string &str);
    void disasm_EOR(Instruct &inst, std::string &str);
    void disasm_SUB(Instruct &inst, std::string &str);
    void disasm_RSB(Instruct &inst, std::string &str);
    void disasm_ADD(Instruct &inst, std::string &str);
    void disasm_ADC(Instruct &inst, std::string &str);
    void disasm_SBC(Instruct &inst, std::string &str);
    void disasm_RSC(Instruct &inst, std::string &str);
    void disasm_TST(Instruct &inst, std::string &str);
    void disasm_TEQ(Instruct &inst, std::string &str);
    void disasm_CMP(Instruct &inst, std::string &str);
    void disasm_CMN(Instruct &inst, std::string &str);
    void disasm_ORR(Instruct &inst, std::string &str);
    void disasm_MOV(Instruct &inst, std::string &str);
    void disasm_BIC(Instruct &inst, std::string &str);
    void disasm_MVN(Instruct &inst, std::string &str);
    void disasm_MLA(Instruct &inst, std::string &str);
    void disasm_MUL(Instruct &inst, std::string &str);
    void disasm_SMLAL(Instruct &inst, std::string &str);
    void disasm_SMULL(Instruct &inst, std::string &str);
    void disasm_UMLAL(Instruct &inst, std::string &str);
    void disasm_UMULL(Instruct &inst, std::string &str);
    void disasm_CLZ(Instruct &inst, std::string &str);
    void disasm_MRS(Instruct &inst, std::string &str);
    void disasm_MSR(Instruct &inst, std::string &str);
    void disasm_LDR(Instruct &inst, std::string &str);
    void disasm_LDRB(Instruct &inst, std::string &str);
    void disasm_LDRBT(Instruct &inst, std::string &str);
    void disasm_LDRH(Instruct &inst, std::string &str);
    void disasm_LDRSB(Instruct &inst, std::string &str);
    void disasm_LDRSH(Instruct &inst, std::string &str);
    void disasm_LDRT(Instruct &inst, std::string &str);
    void disasm_STR(Instruct &inst, std::string &str); 
    void disasm_STRB(Instruct &inst, std::string &str);
    void disasm_STRBT(Instruct &inst, std::string &str);
    void disasm_STRH(Instruct &inst, std::string &str);
    void disasm_STRT(Instruct &inst, std::string &str);
    void disasm_LDM(Instruct &inst, std::string &str);
    void disasm_STM(Instruct &inst, std::string &str);
    void disasm_SWP(Instruct &inst, std::string &str);
    void disasm_SWPB(Instruct &inst, std::string &str);
    void disasm_BKPT(Instruct &inst, std::string &str);
    void disasm_SWI(Instruct &inst, std::string &str);
    void disasm_CDP(Instruct &inst, std::string &str);
    void disasm_LDC(Instruct &inst, std::string &str); 
    void disasm_MCR(Instruct &inst, std::string &str); 
    void disasm_MRC(Instruct &inst, std::string &str); 
    void disasm_STC(Instruct &inst, std::string &str);
    void disasm_NOP(Instruct &inst, std::string &str);
    void disasm_Undefined(Instruct &inst, std::string &str);
    void getoperandStr(Instruct &inst, std::string &str);
    
private:
    InstructDisasm(){}
     ~InstructDisasm(){}
    

};

#endif //TVMDISASM_INC
