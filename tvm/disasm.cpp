/// tvm
/// virtual machine for toyable language

#include "disasm.h"

#define FSPACE  "\t"
#define SSPACE  " "
#define SEPRAT_STR ","
#define SPACE_STR  "\t"

typedef void (InstructDisasm::*InstructParserPtr)(Instruct & inst, std::string &str);

struct OpcodeInfo {
    InstructType type;
    InstructParserPtr parser;

}OpcodeInfoTable[ ] = {
    {INST_B,    &InstructDisasm::disasm_B},   //1    
    {INST_BL,   &InstructDisasm::disasm_BL},
    {INST_BLX1, &InstructDisasm::disasm_BLX1},
    {INST_BLX2, &InstructDisasm::disasm_BLX2},
    {INST_BX,   &InstructDisasm::disasm_BX},
    {INST_AND,  &InstructDisasm::disasm_AND},
    {INST_EOR,  &InstructDisasm::disasm_EOR},
    {INST_SUB,  &InstructDisasm::disasm_SUB},
    {INST_RSB,  &InstructDisasm::disasm_RSB},
    {INST_ADD,  &InstructDisasm::disasm_ADD},
    {INST_ADC,  &InstructDisasm::disasm_ADC}, //10
    {INST_SBC,  &InstructDisasm::disasm_SBC},
    {INST_RSC,  &InstructDisasm::disasm_RSC},
    {INST_TST,  &InstructDisasm::disasm_TST},
    {INST_TEQ,  &InstructDisasm::disasm_TEQ},
    {INST_CMP,  &InstructDisasm::disasm_CMP},
    {INST_CMN,  &InstructDisasm::disasm_CMN},
    {INST_ORR,  &InstructDisasm::disasm_ORR},
    {INST_MOV,  &InstructDisasm::disasm_MOV},
    {INST_BIC,  &InstructDisasm::disasm_BIC},
    {INST_MNV,  &InstructDisasm::disasm_MVN}, //20
    {INST_MLA,  &InstructDisasm::disasm_MLA},
    {INST_MUL,  &InstructDisasm::disasm_MUL},
    {INST_SMLAL,&InstructDisasm::disasm_SMLAL},
    {INST_SMULL,&InstructDisasm::disasm_SMULL},
    {INST_UMLAL,&InstructDisasm::disasm_UMLAL},
    {INST_UMULL,&InstructDisasm::disasm_UMULL},
    {INST_CLZ,  &InstructDisasm::disasm_CLZ},
    {INST_MRS,  &InstructDisasm::disasm_MRS},
    {INST_MSR,  &InstructDisasm::disasm_MSR},
    {INST_LDR,  &InstructDisasm::disasm_LDR},
    {INST_LDRB, &InstructDisasm::disasm_LDRB},
    {INST_LDRBT,&InstructDisasm::disasm_LDRBT},
    {INST_LDRH, &InstructDisasm::disasm_LDRH},
    {INST_LDRSB,&InstructDisasm::disasm_LDRSB},
    {INST_LDRSH,&InstructDisasm::disasm_LDRSH},
    {INST_LDRT, &InstructDisasm::disasm_LDRT},
    {INST_STR,  &InstructDisasm::disasm_STR},  //40
    {INST_STRB, &InstructDisasm::disasm_STRB},
    {INST_STRBT,&InstructDisasm::disasm_STRBT},
    {INST_STRH, &InstructDisasm::disasm_STRH},
    {INST_STRT, &InstructDisasm::disasm_STRT},
    //load and   and
    {INST_LDM,  &InstructDisasm::disasm_LDM},
    {INST_STM,  &InstructDisasm::disasm_STM},
    //semphore  hore
    {INST_SWP,  &InstructDisasm::disasm_SWP},
    {INST_SWPB, &InstructDisasm::disasm_SWPB},
    //exceptio  ptio
    {INST_BKPT, &InstructDisasm::disasm_BKPT},
    {INST_SWI,  &InstructDisasm::disasm_SWI}, //50
    //coproces  oces
    {INST_CDP,  &InstructDisasm::disasm_CDP},
    {INST_LDC,  &InstructDisasm::disasm_LDC}, 
    {INST_MCR,  &InstructDisasm::disasm_MCR}, 
    {INST_MRC,  &InstructDisasm::disasm_MRC}, 
    {INST_STC,  &InstructDisasm::disasm_STC},
    {INST_NOP,  &InstructDisasm::disasm_NOP},
    {INST_Undefined, &InstructDisasm::disasm_Undefined }
};



void InstructDisasm::disasm(Instruct &inst, std::string &instStr)
{
    Instruct * arminst = (Instruct *)(inst);
    if (!arminst)
        return EInvalidInstruct;

    u32 tableSize = sizeof (OpcodeInfoTable)/sizeof(OpcodeInfoTable[0]);

    if ((u32)arminst->_type < tableSize) {
        InstructParserPtr ptr =  OpcodeInfoTable[arminst->_type].parser;
        return (this->*ptr)(*arminst, instStr);
    }   
    
    return ENoError;
}

void InstructDisasm::makeConditString(u8 condit, std::string & str)
{
    switch (condit) {
        case COND_EQ: str = "eq"; break;
        case COND_NE: str = "ne"; break;
        case COND_CS: str = "cs"; break;
        case COND_CC: str = "cc"; break;
        case COND_MI: str = "ml"; break;
        case COND_PL: str = "pl"; break;
        case COND_VS: str = "vs"; break;
        case COND_VC: str = "vc"; break;
        case COND_HI: str = "hi"; break;
        case COND_LS: str = "ls"; break;
        case COND_GE: str = "ge"; break;
        case COND_LT: str = "lt"; break;
        case COND_GT: str = "gt"; break;
        case COND_LE: str = "le"; break;
        default: break;
    };
    return ENoError;
}

void InstructDisasm::disasm_B(Instruct & inst, std::string & str)
{
    //makeConditString
    std::ostringstream strbuf;

    std::string conditStr;
    makeConditString(inst._condit, conditStr);
    strbuf << "b" << conditStr;
    //cacaluate the target address

    strbuf << FSPACE << showbase << hex << inst.info.branch.targetAddress;
    str = strbuf.str();

    return ENoError;
}
void InstructDisasm::disasm_BL(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;

    strbuf << "bl" << FSPACE;
    strbuf << showbase  << hex << inst.info.branch.targetAddress;;

    str = strbuf.str();

    return ENoError;
}

void InstructDisasm::disasm_BLX1(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "blx";
    
    s32 targetAddress = (s32)inst.info.branch.targetAddress;
    strbuf << FSPACE << showbase  << hex << targetAddress;
    str = strbuf.str();

    return ENoError;
}

void InstructDisasm::disasm_BLX2(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "blx";
    strbuf << FSPACE << "r" << dec << (u16)inst.info.branch.rm;

    return ENoError;
}

void InstructDisasm::disasm_BX(Instruct & inst, std::string & str)
{
    return ENoError;
}

void InstructDisasm::disasm_AND(Instruct & inst, std::string & str)
{    
    std::ostringstream strbuf;
    strbuf << "and";
    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    //make the RD string
    strbuf << FSPACE << "r" << (u16)inst.info.dpi.rd << ",";
    
    if (inst.info.dpi.rn == 15) {
        strbuf << SSPACE << "pc";
    }
    else {
        strbuf << SSPACE << "r" << (u16)inst.info.dpi.rn;
    }

    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();
    
    return ENoError;
}

void InstructDisasm::disasm_EOR(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "eor";
    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    //make the RD string
    strbuf << FSPACE << "r" << (u16)inst.info.dpi.rd << ",";
    
    if (inst.info.dpi.rn == 15) {
        strbuf << SSPACE << "pc";
    }
    else {
        strbuf << SSPACE << "r" << (u16)inst.info.dpi.rn;
    }

    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_SUB(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "sub";
    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    //make the RD string
    strbuf << FSPACE << "r" << (u16)inst.info.dpi.rd << ",";
    
    if (inst.info.dpi.rn == 15) {
        strbuf << SSPACE << "pc";
    }
    else {
        strbuf << SSPACE << "r" << (u16)inst.info.dpi.rn;
    }

    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();    
    return ENoError;
}
void InstructDisasm::disasm_RSB(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "rsb";
    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    //make the RD string
    strbuf << FSPACE << "r" << (u16)inst.info.dpi.rd << ",";
    
    if (inst.info.dpi.rn == 15) {
        strbuf << SSPACE << "pc";
    }
    else {
        strbuf << SSPACE << "r" << (u16)inst.info.dpi.rn;
    }

    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();    
    return ENoError;
}

void InstructDisasm::disasm_ADD(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "add";
    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;
 
    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    //make the RD string
    strbuf << FSPACE << "r" << (u16)inst.info.dpi.rd << ",";
    
    if (inst.info.dpi.rn == 15) {
        strbuf << SSPACE << "pc";
    }
    else {
        strbuf << SSPACE << "r" << (u16)inst.info.dpi.rn;
    }

    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();    
    return ENoError;
}

void InstructDisasm::disasm_ADC(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "adc";
    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    //make the RD string
    strbuf << FSPACE << "r" << (u16)inst.info.dpi.rd << ",";
    
    if (inst.info.dpi.rn == 15) {
        strbuf << SSPACE << "pc";
    }
    else {
        strbuf << SSPACE << "r" << (u16)inst.info.dpi.rn;
    }

    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();    
    return ENoError;
}

void InstructDisasm::disasm_SBC(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "";
    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    //make the RD string
    strbuf << FSPACE << "r" << (u16)inst.info.dpi.rd << ",";
    
    if (inst.info.dpi.rn == 15) {
        strbuf << SSPACE << "pc";
    }
    else {
        strbuf << SSPACE << "r" << (u16)inst.info.dpi.rn;
    }

    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();    
    return ENoError;
}

void InstructDisasm::disasm_RSC(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "rsc";

    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    //make the RD string
    strbuf << FSPACE << "r" << (u16)inst.info.dpi.rd << ",";
    
    if (inst.info.dpi.rn == 15) {
        strbuf << SSPACE << "pc";
    }
    else {
        strbuf << SSPACE << "r" << (u16)inst.info.dpi.rn;
    }

    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();    
    return ENoError;
}

void InstructDisasm::disasm_TST(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "tst";
    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    //make the RD string
    if (inst.info.dpi.rn == 15) {
        strbuf << FSPACE << "pc";
    }
    else {
        strbuf << FSPACE << "r" << dec << (u16)inst.info.dpi.rn;
    }
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_TEQ(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "teq";
    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    //make the RD string
    if (inst.info.dpi.rn == 15) {
        strbuf << FSPACE << "pc";
    }
    else {
        strbuf << FSPACE << "r" << dec << (u16)inst.info.dpi.rn;
    }
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_CMP(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "cmp";

    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    //make the RD string
    if (inst.info.dpi.rn == 15) {
        strbuf << FSPACE << "pc";
    }
    else {
        strbuf << FSPACE << "r" << dec << (u16)inst.info.dpi.rn;
    }
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_CMN(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "cmn";
    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    //make the RD string
    if (inst.info.dpi.rn == 15) {
        strbuf << FSPACE << "pc";
    }
    else {
        strbuf << FSPACE << "r" << dec << (u16)inst.info.dpi.rn;
    }
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_ORR(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "orr";
    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    //make the RD string
    strbuf << FSPACE << "r" << (u16)inst.info.dpi.rd << ",";
    
    if (inst.info.dpi.rn == 15) {
        strbuf << SSPACE << "pc";
    }
    else {
        strbuf << SSPACE << "r" << (u16)inst.info.dpi.rn;
    }

    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();    
    return ENoError;
}

void InstructDisasm::disasm_MOV(Instruct & inst, std::string & str)
{    
    std::ostringstream strbuf;
    strbuf << "mov";

    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }
    //make the RD string
    if (inst.info.dpi.rd == 15) {
        strbuf << FSPACE << "pc";
    }
    else {
        strbuf << FSPACE << "r" << (u16)inst.info.dpi.rd;
    }

    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_BIC(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "bic";
    
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }
    //make the RD string
    if (inst.info.dpi.rd == 15) {
        strbuf << FSPACE << "pc";
    }
    else {
        strbuf << FSPACE << "r" << (u16)inst.info.dpi.rd;
    }

    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();
    return ENoError;

}

void InstructDisasm::disasm_MVN(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "mvn";

    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }
    //make the RD string
    if (inst.info.dpi.rd == 15) {
        strbuf << FSPACE << "pc";
    }
    else {
        strbuf << FSPACE << "r" << (u16)inst.info.dpi.rd;
    }

    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << "," << SSPACE << operandStr;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_MLA(Instruct & inst, std::string & str)
{
    //mla{s} <rd>, <rm>, <rs>, <rn>
    std::ostringstream strbuf;
    strbuf << "mla";
     
    std::string condit;
    makeConditString(inst._condit, condit);
    strbuf << condit;

    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    // rd string
    strbuf << FSPACE << "r" << dec << (u16)inst.info.mpl.rd << "," ;
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rs << "," ;
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rn ;
    str = strbuf.str();   
    return ENoError;
}

void InstructDisasm::disasm_MUL(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "mul";
    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    // rd string
    strbuf << FSPACE << "r" << dec << (u16)inst.info.mpl.rd << "," ;
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rm << "," ;
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rn ;
    str = strbuf.str();   
    return ENoError;
}

void InstructDisasm::disasm_SMLAL(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "smlal";
    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    strbuf << FSPACE << "r" << dec << (u16)inst.info.mpl.rdlo << "," ;
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rdhi << "," ;
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rn ;
    str = strbuf.str();   
    return ENoError;

}

void InstructDisasm::disasm_SMULL(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "smull";
    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    strbuf << FSPACE << "r" << dec << (u16)inst.info.mpl.rdlo << "," ;
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rdhi << "," ;
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rm << ",";
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rn ;
    str = strbuf.str();   

    return ENoError;
}

void InstructDisasm::disasm_UMLAL(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "umlal";
    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    strbuf << FSPACE << "r" << dec << (u16)inst.info.mpl.rdlo << "," ;
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rdhi << "," ;
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rm << ",";
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rn ;
    str = strbuf.str();   

    return ENoError;
}

void InstructDisasm::disasm_UMULL(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "umull";
    if (inst.info.dpi.flagS == 0x01) {
        strbuf << "s";
    }

    strbuf << FSPACE << "r" << dec << (u16)inst.info.mpl.rdlo << "," ;
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rdhi << "," ;
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rm << ",";
    strbuf << SSPACE << "r" << dec << (u16)inst.info.mpl.rn ;
    str = strbuf.str();   

    return ENoError;
}

void InstructDisasm::disasm_CLZ(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "clz";
    strbuf << FSPACE << "r" << dec << (u16)inst.info.msic.rd << ",";
    strbuf << SSPACE << "r" << dec << (u16)inst.info.msic.rm;
    return ENoError;
}

void InstructDisasm::disasm_MRS(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "mrs";
    strbuf << FSPACE << "r" << dec << (u16)inst.info.msr.rd << ",";

    if (inst.info.msr.flagR == 0x01) {
        strbuf << SSPACE << "spsr";
    }
    else {
        strbuf << SSPACE << "cpsr";
    }
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_MSR(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "msr";

    if (inst.info.msr.flagR == 1) 
    {
        strbuf << FSPACE << "spsr";
    }
    else
    {
        strbuf << FSPACE << "cpsr";
    }
    if (inst.info.msr.filedMask & MSR_FIELD_C)
        strbuf << "_c";
    else if (inst.info.msr.filedMask & MSR_FIELD_X)
        strbuf << "_x";
    else if (inst.info.msr.filedMask & MSR_FIELD_S)
        strbuf << "_s";
    else if (inst.info.msr.filedMask & MSR_FIELD_F)
        strbuf << "_f";

    if (inst.info.msr.optype == 1)
        strbuf << "," << SSPACE << "#" << showbase << hex << ((u16)inst.info.msr.immedia & 0x00ff);
    else
        strbuf << "," << SSPACE << "r" << dec <<  (u16)inst.info.msr.rm;

    str = strbuf.str();
    return ENoError;

}


void InstructDisasm::disasm_LDR(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "ldr";
    
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << FSPACE << operandStr ;
    str = strbuf.str();

    return ENoError;
}

void InstructDisasm::disasm_LDRB(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "ldrb";
    
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << FSPACE << operandStr ;
    str = strbuf.str();

    return ENoError;
}

void InstructDisasm::disasm_LDRBT(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "ldrbt";
    
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << FSPACE << operandStr ;
    str = strbuf.str();

    return ENoError;
}

void InstructDisasm::disasm_LDRH(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "ldrh";
    
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << FSPACE << operandStr ;
    str = strbuf.str();

    return ENoError;
}

void InstructDisasm::disasm_LDRSB(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "ldrsb";
    
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << FSPACE << operandStr ;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_LDRSH(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "ldrsh";
    
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << FSPACE << operandStr ;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_LDRT(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "ldrt";
    
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << FSPACE << operandStr ;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_STR(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "str";
    
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << FSPACE << operandStr ;
    str = strbuf.str();
    return ENoError;
}
 
void InstructDisasm::disasm_STRB(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "str ";
    
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << FSPACE << operandStr ;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_STRBT(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "strbt";
    
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << FSPACE << operandStr ;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_STRH(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "strh";
    
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << FSPACE << operandStr ;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_STRT(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "strt";
    
    std::string operandStr;
    getoperandStr(inst, operandStr);
    strbuf << FSPACE << operandStr ;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_LDM(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "ldm";

    std::string modeStr;
    
    //get address mode string
    switch (inst.info.lsm.mode) {
    case LASM_IA_MODE: strbuf << "ia"; break;
    case LASM_DA_MODE: strbuf << "da"; break;
    case LASM_IB_MODE: strbuf << "ib"; break;
    case LASM_DB_MODE: strbuf << "db"; break;
    default:
        return EInvalidInstruct;
    };

    strbuf << FSPACE;
    if (inst.info.lsm.rn == 15){
        strbuf << "pc" ;
    }
    else {
        strbuf << "r" << dec << (u16)inst.info.lsm.rn ;
    }


    //get register list
    std::bitset<sizeof(inst.info.lsm.reglist) * 8> reglist = inst.info.lsm.reglist;
    char regStr[8];

    std::string reglistStr = "{";
    
    bool isContinue = false;
    bool hasRegBefore = false;
    s8   firstReg = -1;
    s8   endReg = -1;

    for (u8 i = 0; i < reglist.size(); i++) {
        if (reglist.test(i)) {            
            if (firstReg < 0){
                firstReg = i;
                endReg = i;
                isContinue = true;
            }            
            else if (i == (endReg + 1)) {
                endReg = i;
                isContinue = true;
            }
            else 
                isContinue = false;
        } //end if (reglist)
        else if (isContinue)
        {
            if (firstReg == endReg) {
                sprintf(regStr, "r%d", endReg); 
                if (strcmp(regStr, "r15") == 0)
                    sprintf(regStr, "%s", "pc");               
            }
            else {
                sprintf(regStr, "r%d-r%d", firstReg, endReg);
            }

            if (hasRegBefore == false) {
                reglistStr += regStr; 
            }
            else {
                reglistStr += SSPACE;
                reglistStr += regStr;
            }
            hasRegBefore = true;
            isContinue = false;
            firstReg = -1;
            endReg = -1;
        }
    } // end for

    //check the last bit is 1.
    if (firstReg > 0){
       
        if (firstReg == endReg) {
            sprintf(regStr, "r%d", endReg);   
            if (strcmp(regStr, "r15") == 0)
                sprintf(regStr, "%s", "pc");
        }
        else {
            sprintf(regStr, "r%d-r%d", firstReg, endReg);
        }

        if (hasRegBefore == false) {
            reglistStr += regStr; 
        }
        else {
            reglistStr += SSPACE;
            reglistStr += regStr;
        }

    }
    reglistStr += "}";

    //get instruct string
    if (inst.info.lsm.flagS == 0 ) { //LDM
        if (inst.info.lsm.flagW == 0x00){
            strbuf << "," << SSPACE;
        }
        else {
            strbuf << "!" << "," << SSPACE;
        }
        strbuf << reglistStr;
    }
    else {
        strbuf << "," << SSPACE << reglistStr << "^";
    }

    str = strbuf.str();
    return ENoError;
}


void InstructDisasm::disasm_STM(Instruct & inst, std::string & str)
{
    std::ostringstream strbuf;
    strbuf << "stm";
    
    //get address mode string
    switch (inst.info.lsm.mode) {
    case LASM_IA_MODE: strbuf << "ia"; break;
    case LASM_DA_MODE: strbuf << "da"; break;
    case LASM_IB_MODE: strbuf << "ib"; break;
    case LASM_DB_MODE: strbuf << "db"; break;
    default:
        return EInvalidInstruct;
        break;
    };

    strbuf << FSPACE;
    if (inst.info.lsm.rn == 15){
        strbuf << "pc";
    }
    else {
        strbuf << "r" << dec << (u16)inst.info.lsm.rn;
    }

    //get register list
    std::bitset<sizeof(inst.info.lsm.reglist) *8> reglist = 
        inst.info.lsm.reglist;
    char regStr[8];

    std::string reglistStr = "{";
    
    bool isContinue = false;
    bool hasRegBefore = false;
    s8   firstReg = -1;
    s8   endReg = -1;

    for (u8 i = 0; i < reglist.size(); i++) {
        if (reglist.test(i)) {            
            if (firstReg < 0){
                firstReg = i;
                endReg = i;
                isContinue = true;
            }            
            else if (i == (endReg + 1)) {
                endReg = i;
                isContinue = true;
            }
            else
                isContinue = false;

        } //end if (reglist)
        else if (isContinue)
        {
                if (firstReg == endReg) {
                   sprintf(regStr, "r%d", endReg);                    
                }
                else {
                   sprintf(regStr, "r%d-r%d", firstReg, endReg);
                }

                if (hasRegBefore == false) {
                    reglistStr += regStr; 
                }
                else {
                    reglistStr += SEPRAT_STR;
                    reglistStr += regStr;
                }
                hasRegBefore = true;
                isContinue = false;
                firstReg = -1;
                endReg = -1;
        }
    } // end for

    //check the last bit is 1.
    if (firstReg > 0){
       
        if (firstReg == endReg) {
            if (endReg != 15)
                sprintf(regStr, "r%d", endReg);
            else 
                sprintf(regStr, "%s", "pc");
            
        }
        else {
            sprintf(regStr, "r%d-r%d", firstReg, endReg);
        }

        if (hasRegBefore == false) {
            reglistStr += regStr; 
        }
        else {
            reglistStr += SEPRAT_STR;
            reglistStr += regStr;
        }

    }
    reglistStr += "}";

    //get instruct string

    if (inst.info.lsm.flagS == 0 ) { //STM1
        
        if (inst.info.lsm.flagW == 0x00){
            strbuf << SSPACE;
        }
        else {
            strbuf << "!" <<"," << SSPACE;
        }
        strbuf << reglistStr;
    }
    else {  //STM2

        strbuf << reglistStr << "^";
    }

    str = strbuf.str();

    return ENoError;
}



void InstructDisasm::disasm_SWP(Instruct & inst, std::string & str)
{
    ostringstream strbuf;
    strbuf << "swp";
    strbuf << FSPACE << "r" << dec << (u16)inst.info.swi.rd << ",";
    strbuf << SSPACE << "r" << dec << (u16)inst.info.swi.rm << ",";
    strbuf << SSPACE << "r" << dec << (u16)inst.info.swi.rn;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_SWPB(Instruct & inst, std::string & str)
{
    ostringstream strbuf;
    strbuf << "swpb";
    strbuf << FSPACE << "r" << dec << (u16)inst.info.swi.rd << ",";
    strbuf << SSPACE << "r" << dec << (u16)inst.info.swi.rm << ",";
    strbuf << SSPACE << "r" << dec << (u16)inst.info.swi.rn;
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_BKPT(Instruct & inst, std::string & str)
{
    return ENoError;
}

void InstructDisasm::disasm_SWI(Instruct & inst, std::string & str)
{
    ostringstream strbuf;
    strbuf << "swi";
    if (inst.info.swi.imed24 < 10) {
        strbuf << FSPACE << dec << inst.info.swi.imed24;
    }
    else {
        strbuf << showbase;
        strbuf << FSPACE << hex << inst.info.swi.imed24;
    }
    str = strbuf.str();
    return ENoError;
}

void InstructDisasm::disasm_CDP(Instruct & inst, std::string & str)
{
    return ENoError;
}

void InstructDisasm::disasm_LDC(Instruct & inst, std::string & str)
{
    return ENoError;
}
 
void InstructDisasm::disasm_MCR(Instruct & inst, std::string & str)
{
    return ENoError;
}
 
void InstructDisasm::disasm_MRC(Instruct & inst, std::string & str)
{
    return ENoError;
}
 
void InstructDisasm::disasm_STC(Instruct & inst, std::string & str)
{
    return ENoError;
}
void InstructDisasm::disasm_NOP(Instruct & inst, std::string & str)
{
    str = "nop";
    return ENoError;
}
void InstructDisasm::disasm_Undefined(Instruct & inst, std::string & str)
{
    return ENoError;
}


//void InstructDisasm::getoperandStr(Instruct & inst std::string &operandStr)
void InstructDisasm::getoperandStr(Instruct & inst, std::string &operandStr)
{
    using namespace utility;
  

    if ((inst._type <= INST_MNV) && (inst._type >= INST_AND)) {
        
        std::ostringstream strbuf;

        switch (inst.info.dpi.mode) {
        
        case DPI_IMMEDIATE_MODE:
            if (inst.info.dpi.shifter_operand < 10) {
                strbuf << "#" << dec << inst.info.dpi.shifter_operand;
            }
            else {
                strbuf << showbase;
                strbuf << "#" << hex << inst.info.dpi.shifter_operand; 
            }
            operandStr = strbuf.str();
            break;

        case DPI_RM_MODE:
            strbuf << "r"  << dec << (u16)inst.info.dpi.rm; 
            operandStr = strbuf.str();
            break;

        case DPI_RM_LSL_SHIFT_MODE:
            strbuf << "r" << dec << (u16)inst.info.dpi.rm << "," << SSPACE;
            if ( inst.info.dpi.shift_imm < 10) {
                strbuf << "lsl #" << dec << (u32)inst.info.dpi.shift_imm;
            }
            else {
                strbuf << showbase;
                strbuf << "lsl #" << hex << (u32)inst.info.dpi.shift_imm;
            }
            operandStr = strbuf.str();
            break;

        case DPI_RM_LSL_RS_MODE:
            strbuf << "r" << dec << (u16)inst.info.dpi.rm << "," << SSPACE;
            strbuf << "lsr r" << dec << (u16)inst.info.dpi.rs ;
            operandStr = strbuf.str();
            break;

        case DPI_RM_LSR_SHIFT_MODE:
            strbuf << "r" << (u16)inst.info.dpi.rm << "," << SSPACE;
            if (inst.info.dpi.shift_imm < 10) {
                strbuf << "lsr #" << dec << (u32)inst.info.dpi.shift_imm;
            }
            else {
                strbuf << showbase;
                strbuf << "lsr #" << hex << (u32)inst.info.dpi.shift_imm;
            }
            operandStr = strbuf.str();
            break;
        
        case DPI_RM_LSR_RS_MODE:
            strbuf << "r" << dec << (u16)inst.info.dpi.rm << "," << SSPACE;
            strbuf << "lsr " << "r" << dec << (u16)inst.info.dpi.rs;
            operandStr = strbuf.str();
            break;

        case DPI_RM_ASR_SHIFT_MODE:
            strbuf << "r" << dec << (u16)inst.info.dpi.rm << "," << SSPACE;
            if (inst.info.dpi.shift_imm < 10) {
                strbuf << "asr #" << dec << inst.info.dpi.shift_imm;
            }
            else {
                strbuf << showbase;
                strbuf << "asr #" << hex << inst.info.dpi.shift_imm;
            }
            operandStr = strbuf.str();
            break;

        case DPI_RM_ASR_RS_MODE:
            strbuf << "r" << dec << (u16)inst.info.dpi.rm << "," << SSPACE;
            strbuf << "asr " << "r" << dec << (u16)inst.info.dpi.rs;
            operandStr = strbuf.str();
            break;

        case DPI_RM_ROR_SHIFT_MODE:
            strbuf << "r" << dec << (u16)inst.info.dpi.rm << "," << SSPACE;
            if (inst.info.dpi.shift_imm < 10) {
                strbuf << "ror #" << dec << inst.info.dpi.shift_imm;
            }
            else {
                strbuf << showbase;
                strbuf << "ror #" << hex <<  inst.info.dpi.shift_imm;
            }
            operandStr = strbuf.str();
            break;

        case DPI_RM_ROR_RS_MODE:
            strbuf << "r" << dec << (u16)inst.info.dpi.rm << "," << SSPACE;
            strbuf << "ror r" << dec << (u16)inst.info.dpi.rs;
            operandStr = strbuf.str();
            break;

        case DPI_RM_RRX_MODE:
            strbuf << "r" << dec << (u16)inst.info.dpi.rm << "," << SSPACE;
            strbuf << "rrx";
            operandStr = strbuf.str();
            break;
        default:
            break;
        };
    }// end for data process instruction

    else if ((inst._type <= INST_STRT) && (inst._type >= INST_LDR)) {
        
        std::ostringstream strbuf;
        std::string sigStr;
        if (inst.info.mls.flagU == 0){
            sigStr = "-";
        }

        switch (inst.info.mls.mode) {

        case LDSR_RN_OFFSET12_WB_MODE:
            if ((u16)inst.info.mls.rd == 15){
                strbuf << "pc" << "," << SSPACE;
            }
            else {
                strbuf << "r" << dec << (u16)inst.info.mls.rd << "," << SSPACE;
            }

            if (inst.info.mls.offset < 10) {
                strbuf << "#" << sigStr << dec << inst.info.mls.offset;
            }
            else {
                strbuf << showbase;
                strbuf << "#" << sigStr << hex << inst.info.mls.offset;
            }
            operandStr = strbuf.str();
            break;

        case LDSR_RN_RM_WB_MODE:
            strbuf << "r" << dec << (u16)inst.info.mls.rn << "," << SSPACE;
            strbuf << "r" << dec << (u16)inst.info.mls.rm;
            operandStr = strbuf.str();
            break;

        case LDSR_RN_RM_SHIFT_SHIFTM_WB_MODE:
            //<Rn>, +/-<Rm>, Shift #<shift_imm>
            strbuf << "r" << dec << (u16)inst.info.mls.rn << "," << SSPACE;
            strbuf << sigStr << "r" << dec << (u16)inst.info.mls.rm << "," << SSPACE;

            if (inst.info.mls.shift == 0x00)
                strbuf << "LSL";
            else if (inst.info.mls.shift == 0x01)
                strbuf << "LSR";
            else if (inst.info.mls.shift == 0x02)
                strbuf << "ASR";
            else if (inst.info.mls.shift == 0x03) {
                if (inst.info.mls.shift_imm == 0)             
                    strbuf << "ROR";
                else 
                    strbuf << "RRX";
            }

            if (inst.info.mls.shift_imm < 10) {
                strbuf << SSPACE << "#" << dec << (u32)inst.info.mls.shift_imm;
            }
            else {
                strbuf << showbase;
                strbuf << SSPACE << "#" << hex << (u32)inst.info.mls.shift_imm;
            }
            operandStr = strbuf.str();
            break;

        case LDSR_RN_OFFSET12_H_WB_MODE:
            strbuf << "r" << dec << (u16)inst.info.mls.rn << "," << SSPACE;
            if (inst.info.mls.offset < 10) {
                strbuf << "#" << sigStr << inst.info.mls.offset;
            }
            else {
                strbuf << showbase;
                strbuf << "#" << sigStr << inst.info.mls.offset;
            }
            if (inst.info.mls.flagW == 1)
                strbuf << "!";
            operandStr = strbuf.str();
            break;

        case LDSR_RN_RM_H_WB_MODE:
            strbuf << "r" << dec << (u16)inst.info.mls.rn << "," << SSPACE;
            strbuf << sigStr << "r" << dec << (u16)inst.info.mls.rm;
            if (inst.info.mls.flagW == 1)
                strbuf << "!";
            operandStr = strbuf.str();
            break;

        case LDSR_RN_RM_SHIFT_SHIFTM_H_WB_MODE:
            //<Rn>, +/-<Rm>, Shift #<shift_imm>
            strbuf << "r" << dec << (u16)inst.info.mls.rn << "," << SSPACE;
            strbuf << sigStr << "r" << dec << (u16)inst.info.mls.rm << "," << SSPACE;

            if (inst.info.mls.shift == 0x00)
                strbuf << "LSL";
            else if (inst.info.mls.shift == 0x01)
                strbuf << "LSR";
            else if (inst.info.mls.shift == 0x02)
                strbuf << "ASR";
            else if (inst.info.mls.shift == 0x03) {
                if (inst.info.mls.shift_imm == 0)             
                    strbuf << "ROR";
                else 
                    strbuf << "RRX";
            }
            if (inst.info.mls.shift_imm < 10) {
                strbuf << SSPACE << "#" << sigStr << dec << inst.info.mls.shift_imm;
            }
            else {
                strbuf << showbase;
                strbuf << SSPACE << "#" << hex << inst.info.mls.shift_imm;
            }
            if (inst.info.mls.flagW == 1)
                strbuf << "!";
            operandStr = strbuf.str();
            break;

        case LDSR_RN_OFFSET12_V_WB_MODE:
            strbuf << "r" << dec << (u16)inst.info.mls.rn << "," << SSPACE;
            if (inst.info.mls.offset < 10) {
                strbuf << sigStr << dec << inst.info.mls.offset;
            }
            else {
                strbuf << showbase;
                strbuf << sigStr << hex << inst.info.mls.offset;
            }
            operandStr = strbuf.str();
            break;

        case LDSR_RN_RM_V_WB_MODE:
            strbuf << "r" << dec << (u16)inst.info.mls.rn << "," << SSPACE;
            strbuf << sigStr << "r" << dec << inst.info.mls.rm;
            operandStr = strbuf.str();
            break;

        case LDSR_RN_RM_SHIFT_V_WB_MODE:
            strbuf << "r" << dec << (u16)inst.info.mls.rn << "," << SSPACE;
            strbuf << sigStr << "r" << dec << (u16)inst.info.mls.rm << "," << SSPACE;

            if (inst.info.mls.shift == 0x00)
                strbuf << "LSL";
            else if (inst.info.mls.shift == 0x01)
                strbuf << "LSR";
            else if (inst.info.mls.shift == 0x02)
                strbuf << "ASR";
            else if (inst.info.mls.shift == 0x03) {
                if (inst.info.mls.shift_imm == 0)             
                    strbuf << "ROR";
                else 
                    strbuf << "RRX";
            }

            if (inst.info.mls.shift_imm < 10) {
                strbuf << SSPACE << "#" << dec << inst.info.mls.shift_imm;
            }
            else {
                strbuf << showbase;
                strbuf << SSPACE << "#" << hex << inst.info.mls.shift_imm;
            }
            operandStr = strbuf.str();
            break;
        default:
             break;

        };

    }// end for ldr/str instruction


    return ENoError;
}
