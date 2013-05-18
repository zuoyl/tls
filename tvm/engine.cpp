/// tvm
/// virtual machine for toyable language


#include "tvm.h"
#include "instruct.h"
#include "engine.h"
#include "dbgagent.h"
#include "util.h"


#define GET_TCMODE()        this->m_mode
#define SET_TCMODE(v)       this->m_mode = (v)

#define SET_TCREG(r, v)     this->setRegister(this->m_mode, r, v)
#define GET_TCREG(r)        this->getRegister(this->m_ode, r)

#define SET_TCREG2(mode, r, v)     this->setRegister(mode, r, v)
#define GET_TCREG2(mode, r)        this->getRegister(mode, r)

#define GET_TCCPSR()        this->m_cpsr
#define SET_TCCPSR(v)       this->m_cpsr = (v)
#define GET_TCCPSR2(v)       this->getCPSRValue((v))
#define SET_TCCPSR2(f,v)     this->setCPSRValue((f),(v))

#define GET_TCSPSR()        this->getSPSR(this->m_mode)
#define SET_TCSPSR(v)       this->setSPSR(this->m_mode, v)

#define CPSR_FLAG_MODE   0
#define CPSR_FLAG_T      5
#define CPSR_FLAG_F      6
#define CPSR_FLAG_I      7
#define CPSR_FLAG_Q      27
#define CPSR_FLAG_V      28
#define CPSR_FLAG_C      29
#define CPSR_FLAG_Z      30
#define CPSR_FLAG_N      31

typedef void (TVMEngine::*InstructExecutePtr)(Instruct & inst);
struct OpcodeExecuteInfo {
    InstructType type;
    InstructExecutePtr exec;

} OpcodeExecuteInfoTable[ ] = {
    {INST_B,		&TVMEngine::exec_B		},   //1    
    {INST_BL,		&TVMEngine::exec_BL		},
    {INST_BLX1,	    &TVMEngine::exec_BLX1	},
    {INST_BLX2,	    &TVMEngine::exec_BLX2	},
    {INST_BX,		&TVMEngine::exec_BX		},
    {INST_AND,	    &TVMEngine::exec_AND	},
    {INST_EOR,		&TVMEngine::exec_EOR	},
    {INST_SUB,		&TVMEngine::exec_SUB	},
    {INST_RSB,		&TVMEngine::exec_RSB	},
    {INST_ADD,	    &TVMEngine::exec_ADD	},
    {INST_ADC,	    &TVMEngine::exec_ADC	}, //10
    {INST_SBC,		&TVMEngine::exec_SBC	},
    {INST_RSC,		&TVMEngine::exec_RSC	},
    {INST_TST,	    &TVMEngine::exec_TST	},
    {INST_TEQ,	    &TVMEngine::exec_TEQ	},
    {INST_CMP,	    &TVMEngine::exec_CMP	},
    {INST_CMN,	    &TVMEngine::exec_CMN	},
    {INST_ORR,		&TVMEngine::exec_ORR	},
    {INST_MOV,	    &TVMEngine::exec_MOV	},
    {INST_BIC,		&TVMEngine::exec_BIC		},
    {INST_MNV,	    &TVMEngine::exec_MVN	}, //20
    {INST_MLA,	    &TVMEngine::exec_MLA	},
    {INST_MUL,	    &TVMEngine::exec_MUL	},
    {INST_SMLAL,	&TVMEngine::exec_SMLAL},
    {INST_SMULL,	&TVMEngine::exec_SMULL},
    {INST_UMLAL,	&TVMEngine::exec_UMLAL},
    {INST_UMULL,	&TVMEngine::exec_UMULL},
    {INST_CLZ,		&TVMEngine::exec_CLZ	},
    {INST_MRS,	    &TVMEngine::exec_MRS	},
    {INST_MSR,	    &TVMEngine::exec_MSR	},
    {INST_LDR,		&TVMEngine::exec_LDR	},
    {INST_LDRB,	    &TVMEngine::exec_LDRB	},
    {INST_LDRBT,	&TVMEngine::exec_LDRBT	},
    {INST_LDRH,	    &TVMEngine::exec_LDRH	},
    {INST_LDRSB,	&TVMEngine::exec_LDRSB	},
    {INST_LDRSH,	&TVMEngine::exec_LDRSH	},
    {INST_LDRT,	    &TVMEngine::exec_LDRT	},
    {INST_STR,		&TVMEngine::exec_STR	},  //40
    {INST_STRB,	    &TVMEngine::exec_STRB	},
    {INST_STRBT,	&TVMEngine::exec_STRBT},
    {INST_STRH,	    &TVMEngine::exec_STRH	},
    {INST_STRT,	    &TVMEngine::exec_STRT	},
    //load and   and
    {INST_LDM,	    &TVMEngine::exec_LDM	},
    {INST_STM,	    &TVMEngine::exec_STM	},
    //semphore  hore
    {INST_SWP,	    &TVMEngine::exec_SWP	},
    {INST_SWPB,	    &TVMEngine::exec_SWPB	},
    //exceptio  ptio
    {INST_BKPT,	    &TVMEngine::exec_BKPT	},
    {INST_SWI,	    &TVMEngine::exec_SWI	}, //50
    //coproces  oces
    {INST_CDP,		&TVMEngine::exec_CDP	},
    {INST_LDC,		&TVMEngine::exec_LDC	}, 
    {INST_MCR,	    &TVMEngine::exec_MCR	}, 
    {INST_MRC,	    &TVMEngine::exec_MRC	}, 
    {INST_STC,	    &TVMEngine::exec_STC	}
};

/// @brief engine run entry
void TVMEngine::run() 
{
    while (m_isHalted != true ) {
        try {
            u32 curpc = GET_TCRREG(R15);
            Instruction inst;
            fetch(inst);
            decode(inst);
            exec(inst);
            if (curpc == GET_TCREG(R15)) {
                SET_TCREG(curpc+4);
            }
         }
        catch (InvalidInstructException e) {
            if (e.value == 0xfffffffL)
                if (m_exceptionHandler)
                    m_exceptionHandler(param);
        }
    }
}

/// @brief stop the engine according to the reason
void TVMEngine::stop(u32 reason)
{
    m_isHalted = true;
    m_haltReason = reason;
}

/// @brief reset the engine according to the reason
void TVMEngine::reset(u32 reason) 
{
	m_curMode = NormalMode;
    m_isHalted = false;

	resetAllRegisters(); 
    SET_TCCPSR(0xD3);
    SET_TCSPSR(0);
}

/// @brief exectute ninst instrustion from current pc
void TVMEngine::executeInstrcutions(u32 ninst) 
{
    while (m_isHalted != true && ninst > 0 ) {
        try {
            u32 curpc = GET_TCRREG(R15);
            Instruction inst;
            fetch(inst);
            decode(inst);
            exec(inst);
            if (curpc == GET_TCREG(R15)) {
                SET_TCREG(curpc+4);
            }
            ninst--;
         }
        catch (InvalidInstructException e) {
            if (e.value == 0xfffffffL)
                if (m_exceptionHandler)
                    m_exceptionHandler(param);
        }
    }
}

/// @brief fetch an instruction
void TVMEngine::fetch(Instruction &inst) 
{
    u32 address = GET_TCREG(R15);
    inst.value = readMemory(address, 4);
}

/// @brief register exception handler
void TVMEngine::registerExceptionHandler(CpuExceptionHandler handler, u32 param)
{
    m_exceptHandler = handler;
    m_exceptParam = param;
}

/// @brie unregister exception handler
void TVMEngine::unregisetExceptionHandle()
{
    m_exceptHandler = 0;
}



/// @brief exectue a instruction
void TVMEngine::exec(Instruct &inst)
{
    u32 size = sizeof (OpcodeExecuteInfoTable)/sizeof(OpcodeExecuteInfoTable[0]);

    if ((u32)inst.type < size) {
		bakRegister();
		bakCPSR();
        InstructExecutePtr ptr =  OpcodeExecuteInfoTable[inst.type].exec;
        (this->*ptr)(inst);
    }   
}
/// @brief decode a instruction
void TVMEngine::decode(Instruct &inst)
{
    InstructDecoder::instance()->decode(inst);
}

/// @brief check wether the instruction's condition pass
bool TVMEngine::isConditionPass(Instruct & inst)
{
    // 1. check the kid condition
    if ( (inst.condit == 0x0f) && (inst.kind != 0x05) ) {
        return false;
    }
    
    // 2. check to see wether the cpu status flag satisfy the condition
    bool ret = false;
    switch (inst.condit) {
        case COND_EQ:
            if (GET_TCCPSR2(CPSR_FLAG_Z) == 1) ret = true;
            break;
        case COND_NE:
            if (GET_TCCPSR2(CPSR_FLAG_Z) == 0) ret = true;
            break;

        case COND_CS:
            if (GET_TCCPSR2(CPSR_FLAG_C) == 1) ret = true;
            break;
        case COND_CC:
            if (GET_TCCPSR2(CPSR_FLAG_C) == 0) ret = true;
            break;

        case COND_MI:
            if (GET_TCCPSR2(CPSR_FLAG_N) == 1) ret = true;
            break;
        case COND_PL:
            if (GET_TCCPSR2(CPSR_FLAG_N) == 0) ret = true;
            break;

        case COND_VS:
            if (GET_TCCPSR2(CPSR_FLAG_V) == 1) ret = true;
            break;
        case COND_VC:
            if (GET_TCCPSR2(CPSR_FLAG_V) == 0) ret = true;
            break;

        case COND_HI:
            if (GET_TCCPSR2(CPSR_FLAG_C) == 1 && GET_TCCPSR2(CPSR_FLAG_Z) == 0)
                ret = true;
            break;
        case COND_LS:
      		if (GET_TCCPSR2(CPSR_FLAG_C) == 0 && GET_TCCPSR2(CPSR_FLAG_Z) == 1)
                ret = true;
            break;

        case COND_GE:
            if (GET_TCCPSR2(CPSR_FLAG_N) == GET_TCCPSR2(CPSR_FLAG_V))
                ret = true;
            break;
        case COND_LT:
            if (GET_TCCPSR2(CPSR_FLAG_N) != GET_TCCPSR2(CPSR_FLAG_V))
                ret = true;
            break;

        case COND_GT:
            if (GET_TCCPSR2(CPSR_FLAG_Z) == 0 &&
                GET_TCCPSR2(CPSR_FLAG_N) == GET_TCCPSR2(CPSR_FLAG_V))
                ret = true;
            break;
        case COND_LE:
            if (GET_TCCPSR2(CPSR_FLAG_Z) == 1 &&
                GET_TCCPSR2(CPSR_FLAG_N) != GET_TCCPSR2(CPSR_FLAG_V))
                ret = true;
            break;
        case COND_Always:
            ret = true;
            break;
        case COND_NV:
            //nothing, just so.
            break;

        default:
            ret = true;
            break;
    };
    return ret;
}


/// @brief exectutor for B instruction
void TVMEngine::exec_B(Instruct & inst ) 
{ 
    if (isConditionPass(inst)) {
        s32 val = signExtend(inst.info.branch.signed_immed_24, 24);
        SET_TCREG(R15, (val << 2) + GET_TCREG(R15) + 8);
    }
}

/// @brief exectutor for BL instruction
void TVMEngine::exec_BL(Instruct & inst ) 
{ 
    if (isConditionPass(inst)) {
        SET_TCREG(R14, GET_TCREG(R15) + 4);      
        s32 val = signExtend(inst.info.branch.signed_immed_24, 24);
        SET_TCREG(R15, (val << 2) + GET_TCREG(R15) + 8);
    }
 }
 
/// @brief exectutor for BL instruction
void TVMEngine::exec_BLX1(Instruct & inst ) 
{ 
    if (isConditionPass(inst)) {
        SET_TCREG(R14, GET_TCREG(R15) + 4);
        SET_TCCPSR2(CPSR_FLAG_T, 1);

        //create the target address
        s32 address = signExtend(inst.info.branch.signed_immed_24, 24) << 2;
        address += inst.info.branch.flagL << 1;        
        SET_TCREG(R15, address);
        m_isThumb = true;
    }
}
/// @brief exectutor for BLX2 instruction
void TVMEngine::exec_BLX2(Instruct & inst )
{
    if (isConditionPass(inst)) {
        SET_TCREG(R14, GET_TCREG(R15) + 4);
        SET_TCCPSR2(CPSR_FLAG_T, (u8)GET_TCREG(inst.info.branch.rm) & 0x01);
        SET_TCREG(R15, inst.info.branch.rm & 0xffffffff);
    }
}
/// @brief exectutor for BX instruction
void TVMEngine::exec_BX(Instruct & inst )
{
    if (isConditionPass(inst)) {
        SET_TCCPSR2(CPSR_FLAG_T, (u8)(GET_TCREG(inst.info.branch.rm) & 0x01));
        SET_TCREG(R15, inst.info.branch.rm & 0xffffffff);
    }
}

/// @brief exectutor for ADD instruction
void TVMEngine::exec_ADD(Instruct & inst )
{
	u32 rn = 0;

    if (isConditionPass(inst)) {
		rn = GET_TCREG(inst.info.dpi.rn);
		if(inst.info.dpi.rn == R15)
			SET_TCREG(inst.info.dpi.rd, rn + 8 + inst.info.dpi.shifter_operand);
		else
			SET_TCREG(inst.info.dpi.rd, rn + inst.info.dpi.shifter_operand);
	
        if ((inst.info.dpi.flagS == 1) && (inst.info.dpi.rd == R15)){
            SET_TCCPSR(GET_TCSPSR());
        }
        else if (inst.info.dpi.flagS == 1){
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, (GET_TCREG(inst.info.dpi.rd) == 0?1:0));
            SET_TCCPSR2(CPSR_FLAG_C, isCarryFrom(BIT_SIZE(u32),
				rn, inst.info.dpi.shifter_operand));
            SET_TCCPSR2(CPSR_FLAG_V, isOverflowFrom(BIT_SIZE(u32),
				rn,inst.info.dpi.shifter_operand));
        }
    }
}
/// @brief exectutor for EOR instruction
void TVMEngine::exec_EOR(Instruct & inst ) 
{ 
    if (isConditionPass(inst)) {
        SET_TCREG(inst.info.dpi.rd,
            GET_TCREG(inst.info.dpi.rn) ^ inst.info.dpi.shifter_operand);

        if ((inst.info.dpi.flagS == 1) && (inst.info.dpi.rd == R15)) {
            SET_TCCPSR(GET_TCSPSR());
        }
        else if (inst.info.dpi.flagS == 1) {
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, (GET_TCREG(inst.info.dpi.rd) == 0? 1:0));
            SET_TCCPSR2(CPSR_FLAG_C, (u8)inst.info.dpi.shifterCarryOut);
        }
    }    
}
/// @brief exectutor for SUB instruction
void TVMEngine::exec_SUB(Instruct & inst ) 
{
    if (!isConditionPass(inst)) 
		return;

	ue2 rn = GET_TCREG(inst.info.dpi.rn);

    SET_TCREG(inst.info.dpi.rd, rn - inst.info.dpi.shifter_operand);
    if ((inst.info.dpi.flagS == 1) && (inst.info.dpi.rd == R15)) {
        SET_TCCPSR(GET_TCSPSR());
    }
    else if (inst.info.dpi.flagS == 1){
        SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
        SET_TCCPSR2(CPSR_FLAG_Z, (GET_TCREG(inst.info.dpi.rd) ==0?1:0));
        SET_TCCPSR2(CPSR_FLAG_C, !isBorrowFrom(BIT_SIZE(u32), rn,
                          inst.info.dpi.shifter_operand));
        SET_TCCPSR2(CPSR_FLAG_V, isOverflowFrom(BIT_SIZE(u32), rn,
                          inst.info.dpi.shifter_operand));
    }    
}
/// @brief exectutor for BLX2 instruction
void TVMEngine::exec_RSB(Instruct & inst ) 
{ 
    if (isConditionPass(inst)) {
		u32 rn = GET_TCREG(inst.info.dpi.rn);
        SET_TCREG(inst.info.dpi.rd, inst.info.dpi.shifter_operand - rn) ;
        
        if ((inst.info.dpi.flagS == 1) && (inst.info.dpi.rd == R15)){
            SET_TCCPSR(GET_TCSPSR());
        }
        else if (inst.info.dpi.flagS == 1) {
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, (GET_TCREG(inst.info.dpi.rd) ==0?1:0));
            SET_TCCPSR2(CPSR_FLAG_C, !isBorrowFrom(BIT_SIZE(u32), 
				inst.info.dpi.shifter_operand, rn));
            SET_TCCPSR2(CPSR_FLAG_V, isOverflowFrom(BIT_SIZE(u32),
				inst.info.dpi.shifter_operand,rn));
        }
    }
     
}

/// @brief exectutor for AND instruction
void TVMEngine::exec_AND(Instruct & inst ) 
{ 
    if (isConditionPass(inst)) {
        SET_TCREG(inst.info.dpi.rd,
            GET_TCREG(inst.info.dpi.rn) & inst.info.dpi.shifter_operand);
        if ((inst.info.dpi.flagS == 1) && (inst.info.dpi.rd == R15)) {
            SET_TCCPSR(GET_TCSPSR());
        }
        else if (inst.info.dpi.flagS == 1) {
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, (GET_TCREG(inst.info.dpi.rd) == 0?1:0));
            SET_TCCPSR2(CPSR_FLAG_C, (u8)inst.info.dpi.shifterCarryOut);
        }
    }
}

/// @brief exectutor for ADC instruction
void TVMEngine::exec_ADC(Instruct & inst ) 
{ 
	if (isConditionPass(inst)) {
		uew rn = GET_TCREG(inst.info.dpi.rn);
        u32 val = rn + inst.info.dpi.shifter_operand;
 
        val += GET_TCCPSR2(CPSR_FLAG_C);
        SET_TCREG(inst.info.dpi.rd, val);
        
        if ((inst.info.dpi.flagS == 1) && (inst.info.dpi.rd == R15)){
            SET_TCCPSR(GET_TCSPSR());
        }
        else if (inst.info.dpi.flagS == 1) {
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, !GET_TCREG(inst.info.dpi.rd));
            SET_TCCPSR2(CPSR_FLAG_C, isCarryFrom2(BIT_SIZE(u32), rn, 
                            inst.info.dpi.shifter_operand + GET_TCCPSR2(CPSR_FLAG_C)));
            SET_TCCPSR2(CPSR_FLAG_V, isOverflowFrom2(BIT_SIZE(u32), rn, 
                            inst.info.dpi.shifter_operand + GET_TCCPSR2(CPSR_FLAG_C)));
        }
    }
     
}
/// @brief exectutor for SBC instruction
void TVMEngine::exec_SBC(Instruct & inst ) 
{ 
   if (isConditionPass(inst)) {
		rn = GET_TCREG(inst.info.dpi.rn);
        u32 val = inst.info.dpi.shifter_operand - rn;
        val -=  !GET_TCCPSR2(CPSR_FLAG_C);
        SET_TCREG(inst.info.dpi.rd, val) ;
        
        if ((inst.info.dpi.flagS == 1) && (inst.info.dpi.rd == R15)) {
            SET_TCCPSR(GET_TCSPSR());
        }
        else if (inst.info.dpi.flagS == 1) {
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, (GET_TCREG(inst.info.dpi.rd) == 0?1:0));
            SET_TCCPSR2(CPSR_FLAG_C, !isBorrowFrom(BIT_SIZE(u32), rn,
                              inst.info.dpi.shifter_operand - GET_TCCPSR2(CPSR_FLAG_C)));
            SET_TCCPSR2(CPSR_FLAG_V, isOverflowFrom2(BIT_SIZE(u32), rn,
                                inst.info.dpi.shifter_operand - !GET_TCCPSR2(CPSR_FLAG_C)));
        }
    }
     
}
/// @brief exectutor for RSC instruction
void TVMEngine::exec_RSC(Instruct & inst ) 
{ 
    if (isConditionPass(inst)) {
		rn = GET_TCREG(inst.info.dpi.rn);
        SET_TCREG(inst.info.dpi.rd,
            inst.info.dpi.shifter_operand - rn - !GET_TCCPSR2(CPSR_FLAG_C)) ;
      
        if ((inst.info.dpi.flagS == 1) && (inst.info.dpi.rd == R15)) {
            SET_TCCPSR(GET_TCSPSR());
        }
        else if (inst.info.dpi.flagS == 1) {
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, (GET_TCREG(inst.info.dpi.rd) == 0?1:0));
            SET_TCCPSR2(CPSR_FLAG_C, !isBorrowFrom(BIT_SIZE(u8),
				inst.info.dpi.shifter_operand, rn - !GET_TCCPSR2(CPSR_FLAG_C)));
            SET_TCCPSR2(CPSR_FLAG_V, isOverflowFrom(BIT_SIZE(u8), 
				inst.info.dpi.shifter_operand, rn - !GET_TCCPSR2(CPSR_FLAG_C)));
        }
    }
    
}

/// @brief exectutor for BLX2 instruction
void TVMEngine::exec_TST(Instruct & inst ) 
{ 
   if (isConditionPass(inst)) {
        u32 val = GET_TCREG(inst.info.dpi.rn) & inst.info.dpi.shifter_operand;
        SET_TCCPSR2(CPSR_FLAG_N, (u8)((val >> 31) & 0x01));
        SET_TCCPSR2(CPSR_FLAG_Z, (val == 0?1:0));
        SET_TCCPSR2(CPSR_FLAG_C, inst.info.dpi.shifterCarryOut);
    }     
}
/// @brief The TEQ (Test Equivalence) instruction compares a register value with anotherarithmetic value. Thecondition flags are updated, based on the result of \n
/// logically exclusive-ORing the two values, so that subsequent instructions \n
/// can be conditionally executed.
void TVMEngine::exec_TEQ(Instruct & inst ) 
{ 
   if (isConditionPass(inst)) {
        u32 val = GET_TCREG(inst.info.dpi.rn) ^ inst.info.dpi.shifter_operand;
        SET_TCCPSR2(CPSR_FLAG_N, (u8)((val >> 31) & 0x01));
        SET_TCCPSR2(CPSR_FLAG_Z, (val == 0?1:0));
        SET_TCCPSR2(CPSR_FLAG_C, inst.info.dpi.shifterCarryOut);        
    }     
}
/// @brief The CMP (Compare) instruction compares a register value with another
/// arithmetic value. The condition flags are updated, based on the result of \n
/// subtracting the second arithmetic value from the register value, so that\n
/// subsequent instructions can be conditionally executed.
void TVMEngine::exec_CMP(Instruct & inst ) 
{
    if (isConditionPass(inst)) {
		u32 rn = GET_TCREG(inst.info.dpi.rn);
		s32 val = rn - inst.info.dpi.shifter_operand;
		SET_TCCPSR2(CPSR_FLAG_N, (u8)((val >> 31) & 0x01));
		SET_TCCPSR2(CPSR_FLAG_Z, (val == 0)?1:0);
		SET_TCCPSR2(CPSR_FLAG_C, !isBorrowFrom(BIT_SIZE(u32),
			rn, inst.info.dpi.shifter_operand));
		SET_TCCPSR2(CPSR_FLAG_V, isOverflowFrom2(BIT_SIZE(u32), 
			rn, inst.info.dpi.shifter_operand));
	}     
}
/// @brief The CMN (Compare Negative) instruction compares a register value with the\n
/// negative of another arithmetic value. The condition flags are updated,\n
/// based on the result of adding the second arithmetic value to the register \n
/// value, so that subsequent instructions can be conditionally executed.
void TVMEngine::exec_CMN(Instruct & inst ) 
{
	if (isConditionPass(inst)) {
		u32 rn = GET_TCREG(inst.info.dpi.rn);
		u32 val = rn + inst.info.dpi.shifter_operand;
		SET_TCCPSR2(CPSR_FLAG_N, (u8)((val >> 31) & 0x01));
		SET_TCCPSR2(CPSR_FLAG_Z, (val == 0?1:0));
		SET_TCCPSR2(CPSR_FLAG_C, isCarryFrom2(BIT_SIZE(u32),
			rn, inst.info.dpi.shifter_operand));
		SET_TCCPSR2(CPSR_FLAG_V, isOverflowFrom(BIT_SIZE(u32),
			rn, inst.info.dpi.shifter_operand));        
	}     
}
/// @brief The ORR (Logical OR) instruction performs a bitwise (inclusive) OR of the\n
/// value of register <Rn> with the value of <shifter_operand>, and stores the\n
/// result in the destination register <Rd>. The condition code flags are \n
/// optionally updated, based on the result.
void TVMEngine::exec_ORR(Instruct & inst ) 
{ 
   if (isConditionPass(inst)) {
        SET_TCREG(inst.info.dpi.rd,
            inst.info.dpi.shifter_operand | GET_TCREG(inst.info.dpi.rn));
        if ((inst.info.dpi.flagS == 1) && (inst.info.dpi.rd == R15)) {
            SET_TCCPSR(GET_TCSPSR());
        }
        else if (inst.info.dpi.flagS == 1){
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, (GET_TCREG(inst.info.dpi.rd) == 0?1:0));
            SET_TCCPSR2(CPSR_FLAG_C, inst.info.dpi.shifterCarryOut);
        }
    }     
}
/// @brief The MOV (Move) instruction moves the value of <shifter_operand> to the \n
/// destination register <Rd>.The condition code flags are optionally updated,\n
/// based on the result.
void TVMEngine::exec_MOV(Instruct & inst ) 
{ 
   if (isConditionPass(inst)) {
        SET_TCREG(inst.info.dpi.rd, inst.info.dpi.shifter_operand);
        if ((inst.info.dpi.flagS == 1) && (inst.info.dpi.rd == R15)) {
            SET_TCCPSR(GET_TCSPSR());
        }
        else if (inst.info.dpi.flagS == 1) {
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, (GET_TCREG(inst.info.dpi.rd) == 0?1:0));
            SET_TCCPSR2(CPSR_FLAG_C, inst.info.dpi.shifterCarryOut);
        }
    }     
}
/// @brief The BIC (Bit Clear) instruction performs a bitwise AND of the value of\n
/// register <Rn> with the complement of the value of <shifter_operand>, \n
/// and stores the result in the destination register <Rd>. The condition \n
/// code flags are optionally updated, based on the result.
void TVMEngine::exec_BIC(Instruct & inst ) 
{
    if (isConditionPass(inst)) {
        u32 val = GET_TCREG(inst.info.dpi.rn) & (~inst.info.dpi.shifter_operand);
        SET_TCREG(inst.info.dpi.rd, val);
        
		if ((inst.info.dpi.flagS == 1) && (inst.info.dpi.rd == R15)) {
            SET_TCCPSR(GET_TCSPSR());
        }
        else if (inst.info.dpi.flagS == 1) {
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, (GET_TCREG(inst.info.dpi.rd) == 0?1:0));
            SET_TCCPSR2(CPSR_FLAG_C, inst.info.dpi.shifterCarryOut);
        }
    }     
}
/// @brief The MVN (Move Negative) instruction moves the logical oneÅfs complement of\n
/// the value of <shifter_operand> to the destination register <Rd>. \n
/// The condition code flags are optionally updated,based on the result.
void TVMEngine::exec_MVN(Instruct & inst )
{ 
   if (isConditionPass(inst)) {
        SET_TCREG(inst.info.dpi.rd, !inst.info.dpi.shifter_operand) ;
        
        if ((inst.info.dpi.flagS == 1) && (inst.info.dpi.rd == R15)){
            SET_TCCPSR(GET_TCSPSR());
        }
        else if (inst.info.dpi.flagS == 1){
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, (GET_TCREG(inst.info.dpi.rd) == 0?1:0));
            SET_TCCPSR2(CPSR_FLAG_C, inst.info.dpi.shifterCarryOut);
        }
    }     
}
/// @brief The MLA (Multiply Accumulate) multiplies signed or unsigned operands to \n
/// produce a 32-bit result, which is then added to a third operand, and \n
/// written to the destination register. The condition code flags are \n
/// optionally updated, based on the result.
void TVMEngine::exec_MLA(Instruct & inst ) 
{ 
   if (isConditionPass(inst)) {
        s64 val = GET_TCREG(inst.info.dpi.rm) *
                     GET_TCREG(inst.info.dpi.rs) + GET_TCREG(inst.info.dpi.rn);
        SET_TCREG(inst.info.dpi.rd, (u32)(val & 0xffffffff)) ;
       
        if (inst.info.dpi.flagS == 1){
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, GET_TCREG(inst.info.dpi.rd) == 0?1:0); 
        }
    }     
}
/// @brief The MUL (Multiply) instruction is used to multiply signed or unsigned \n
/// variables to produce a 32-bit result. The condition code flags are \n
/// optionally updated, based on the result.
void TVMEngine::exec_MUL(Instruct & inst ) 
{ 
   if (isConditionPass(inst)) {
        s32 val = GET_TCREG(inst.info.dpi.rm) * GET_TCREG(inst.info.dpi.rs);
        SET_TCREG(inst.info.dpi.rd, (u32)(val & 0xffffffff)) ;
        
        if (inst.info.dpi.flagS == 1){
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.dpi.rd) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, GET_TCREG(inst.info.dpi.rd) == 0?1:0);  
        }
    }     
}
/// @brief The SMLAL (Signed Multiply Accumulate Long) instruction multiplies the \n
/// signed value of register <Rm> with the signed value of register <Rs> to \n
/// produce a 64-bit product. This product is added to the 64-bit value \n
/// held in <RdHi> and <RdLo>, and the sum is written back to <RdHi> and <RdLo>.\n
/// The condition code flags are optionally updated, based on the result.
void TVMEngine::exec_SMLAL(Instruct & inst ) 
{ 
    if (isConditionPass(inst)) {
        s64 val = GET_TCREG(inst.info.dpi.rm) * GET_TCREG(inst.info.dpi.rs);
        SET_TCREG(inst.info.mpl.rdlo, 
            (u32)(val & 0xffffffff) + GET_TCREG(inst.info.mpl.rdlo));

        SET_TCREG(inst.info.mpl.rdhi, 
            ((val >> 32) & 0xffffffff) + GET_TCREG(inst.info.mpl.rdhi)
            + isCarryFrom(BIT_SIZE(u32), (u32)(val & 0xffffffff),
                    GET_TCREG(inst.info.mpl.rdlo)));

        if (inst.info.dpi.flagS == 1) {
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.mpl.rdhi) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, 
                (GET_TCREG(inst.info.mpl.rdhi) == 0 &&
                GET_TCREG(inst.info.mpl.rdlo == 0))?1:0);  
        }
    }     
}
/// @brief The SMULL (Signed Multiply Long) instruction multiplies the signed value\n
/// of register <Rm> with the signed value of register <Rs> to produce a \n
/// 64-bit result. The upper 32 bits of the result are stored in <RdHi>.\n
/// The lower 32 bits are stored in <RdLo>. The condition code flags are \n
/// optionally updated, based on the 64-bit result.
void TVMEngine::exec_SMULL(Instruct & inst )
{ 
    if (isConditionPass(inst)) {
        s64 val = GET_TCREG(inst.info.dpi.rm) * GET_TCREG(inst.info.dpi.rs);
        SET_TCREG(inst.info.mpl.rdlo, (s32)(val & 0xffffffff));
        SET_TCREG(inst.info.mpl.rdhi, (s32)((val >> 32) & 0xffffffff));

        if (inst.info.dpi.flagS == 1) {
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.mpl.rdhi) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, 
                (GET_TCREG(inst.info.mpl.rdhi) == 0 &&
                GET_TCREG(inst.info.mpl.rdlo == 0))?1:0);  
        }
    }
}
/// @brief The UMLAL (Unsigned Multiply Accumulate Long) instruction multiplies \n
/// the unsigned value of register <Rm> with the unsigned value of register\n
/// <Rs> to produce a 64-bit product. This product is added to the 64-bit\n
/// value held in <RdHi> and <RdLo>, and the sum is written back to <RdHi> and
/// <RdLo>. The condition code flags are optionally updated, based on the result.
void TVMEngine::exec_UMLAL(Instruct & inst )
{ 
    if (isConditionPass(inst)) {
        s64 val = GET_TCREG(inst.info.dpi.rm) * GET_TCREG(inst.info.dpi.rs);
        
        SET_TCREG(inst.info.mpl.rdlo, 
            (u32)(val & 0xffffffff) + GET_TCREG(inst.info.mpl.rdlo));
        
        SET_TCREG(inst.info.mpl.rdhi,
            ((val >> 32) & 0xffffffff) + GET_TCREG(inst.info.mpl.rdhi)
            + isCarryFrom(BIT_SIZE(u32), (u32)(val & 0xffffffff), 
                          GET_TCREG(inst.info.mpl.rdlo)));

        if (inst.info.dpi.flagS == 1) {
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.mpl.rdhi) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, 
                        (GET_TCREG(inst.info.mpl.rdhi) == 0 &&  
                        GET_TCREG(inst.info.mpl.rdlo)) == 0?1:0);  
        }        
    }     
}
/// @brief The UMULL (Unsigned Multiply Long) instruction multiplies the unsigned \n
/// value of register <Rm> with the unsigned value of register <Rs> to produce\n
/// a 64-bit result. The upper 32 bits of the result are stored in <RdHi>.\n
/// The lower 32 bits are stored in <RdLo>. The condition code flags are \n
/// optionally updated, based on the 64-bit result.
void TVMEngine::exec_UMULL(Instruct & inst)
{ 
    if (isConditionPass(inst)) {
        s64 val = GET_TCREG(inst.info.dpi.rm) * GET_TCREG(inst.info.dpi.rs);
        SET_TCREG(inst.info.mpl.rdlo, (u32)(val & 0xffffffff));
        SET_TCREG(inst.info.mpl.rdhi, (u32)((val >> 32) & 0xffffffff));

        if (inst.info.dpi.flagS == 1) {
            SET_TCCPSR2(CPSR_FLAG_N, (u8)(GET_TCREG(inst.info.mpl.rdhi) >> 31));
            SET_TCCPSR2(CPSR_FLAG_Z, 
                (GET_TCREG(inst.info.mpl.rdhi) == 0 &&
                GET_TCREG(inst.info.mpl.rdlo)) == 0?1:0);  
        }  
    }
}
/// @brief The CLZ (Count Leading Zeros) instruction returns the number of binary \n
/// zero bits before the first binary one bit in a register value. The source\n
/// register is scanned from the most significant bit (bit[31]) towards the\n
/// least significant bit (bit[0]). The result value is 32 if no bits are set\n
/// in the source register, and zero if bit[31] is set.\n
/// This instruction does not update the condition code flags.
void TVMEngine::exec_CLZ(Instruct & inst)
{ 
    if (GET_TCREG(inst.info.msic.rm) == 0x00) {
        SET_TCREG(inst.info.msic.rd, 32);
    }
    else {
        std::bitset<32> bits = GET_TCREG(inst.info.msic.rm);
        u8 val = 0;
        for (u8 idx = 0; idx < 32; idx++) {
            if (!bits.test(31 - idx)) 
                val ++;
            else
                break;
        }
        SET_TCREG(inst.info.msic.rd, val);
    }   
}
/// @brief The MRS (Move PSR to General-purpose Register) instruction moves the value\n
/// of the CPSR or the SPSR of the current mode into a general-purpose register.\n
/// In the general-purpose register, the value can be examined or manipulated\n
/// with normal data-processing instructions.
void TVMEngine::exec_MRS(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
        if (inst.info.msr.flagR == 1)
            SET_TCREG(inst.info.msr.rd, GET_TCSPSR());
        else
            SET_TCREG(inst.info.msr.rd, GET_TCCPSR());
    } 
}
/// @brief The MSR (Move to Status Register from ARM Register) instruction transfers\n
/// the valueregister or immediate constant to the CPSR or the SPSR of the \n
/// current mode.
void TVMEngine::exec_MSR(Instruct & inst ) 
{
    if (isConditionPass(inst)) {
        u32 operand = 0;        
        if (inst.info.msr.optype == 1)
            operand = rotateShiftRight(inst.info.msr.immedia, inst.info.msr.rotateImm *2);
        else 
            operand = GET_TCREG(inst.info.msr.rm);

        std::bitset<16> mask = inst.info.msr.filedMask ;
        
        if (inst.info.msr.flagR == 0) {
            u32 spval = GET_TCCPSR();
            if (mask.test(0) && isInAPrivilegeMode())
                BitsCopy(&spval, 0, &operand, 0, 8);
        
            if (mask.test(1) && isInAPrivilegeMode())
                BitsCopy(&spval, 8, &operand, 8, 8);

            if (mask.test(2) && isInAPrivilegeMode())
                BitsCopy(&spval, 16, &operand, 16, 8);

            if (mask.test(3))
                BitsCopy(&spval, 24, &operand, 24, 8);

			SET_TCMODE(TransformMode(spval));
            SET_TCSPSR(spval);
        }        
        else {
            u32 spval = GET_TCSPSR();
            if (mask.test(0) && isCurrentModeHasSPSR())
                BitsCopy(&spval, 0, &operand, 0, 8);

            else if (mask.test(1) && isCurrentModeHasSPSR())
                BitsCopy(&spval, 8, &operand, 8, 8);

            else if (mask.test(2) && isCurrentModeHasSPSR())
                BitsCopy(&spval, 16, &operand, 16, 8);

            else if (mask.test(3) && isCurrentModeHasSPSR())
                BitsCopy(&spval, 24, &operand, 24, 8);
            SET_TCSPSR(spval);
        }
    }
}
/// @brief The LDR (Load Register) instruction loads a word from the memory address\n
/// calculated by <addressing_mode> and writes it to register <Rd>. If the\n
/// address is not word-aligned, the loaded value is rotated right by 8 times\n
/// the value of bits[1:0] of the address. For a little-endian memory system,\n
/// this rotation causes the addressed byte to occupy the least significant\n
/// byte of the register. For a big-endian memory system, it causes the \n
/// addressed byte to occupy bits[31:24] or bits[15:8] of the register,\n
/// depending on whether bit[0] of the address is 0 or 1 respectively.
void TVMEngine::exec_LDR(Instruct & inst ) 
{ 
	if (isConditionPass(inst)) {
		getOperandByAddressMode(inst);
		u32 mask = inst.info.mls.address & 0x03;
		u32 value = readMemory(inst.info.mls.address, 4); 

		if (mask == 0x00)
			value = value; 
		else if (mask == 0x01)
			value = rotateShiftRight(value, 8);
		else if (mask == 0x02)
			value = rotateShiftRight(value, 16);
		else  
			value = rotateShiftRight(value, 24);

		if(inst.info.mls.rd == R15) {
			SET_TCREG(inst.info.mls.rd , value & 0xFFFFFFFE);
			SET_TCCPSR2(5,value & 0x01);
		}
		else
			SET_TCREG(inst.info.mls.rd , value);    
	}
}
/// @brief The LDRB (Load Register Byte) instruction loads a byte from the memory\n
/// address calculated by <addressing_mode>, zero-extends the byte to a 32-bit\n
/// word, and writes the word to register
void TVMEngine::exec_LDRB(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
		getOperandByAddressMode(inst);
        SET_TCREG(inst.info.mls.rd, readMemory(inst.info.mls.address, 1));
    }
}
/// @brief The LDRBT (Load Register Byte with Translation) instruction loads a byte\n
/// from the memory address calculated by <post_indexed_addressing_mode>,\n
/// zero-extends the byte to a 32-bit word, and writes the word to register <Rd>
void TVMEngine::exec_LDRBT(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
		getOperandByAddressMode(inst);
        SET_TCREG(inst.info.mls.rd, readMemory(inst.info.mls.address, 1));
    }    
}
/// @brief The LDRH (Load Register Halfword) instruction loads a halfword from the\n
/// memory address calculated by <addressing_mode>, zero-extends the halfword\n
/// to a 32-bit word, and writes the word to register <Rd>. If the address is\n
/// not halfword-aligned, the result is UNPREDICTABLE.
void TVMEngine::exec_LDRH(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
        u32 data; 
		getOperandByAddressMode(inst);

        if ((inst.info.mls.address & 0x01) == 0x00)
            data = readMemory(inst.info.mls.address, 1);
        SET_TCREG(inst.info.mls.rd, data);
    }    
}
/// @brief The LDRSB (Load Register Signed Byte) instruction loads a byte from the\n
/// memory address calculated by <addressing_mode>, sign-extends the byte\n
/// to a 32-bit word, and writes the word to register <Rd>.
void TVMEngine::exec_LDRSB(Instruct & inst )
{ 
    if (isConditionPass(inst)) {
		getOperandByAddressMode(inst);

        s32 data = (s32)readMemory(inst.info.mls.address, 1);
        SET_TCREG(inst.info.mls.rd, signExtend(data, 8));
    }
}
///////////////////////////////////////////////////////////////////////////////
/// The LDRSH (Load Register Signed Halfword) instruction loads a halfword\n
/// from the memory address calculated by <addressing_mode>, sign-extends the\n
/// halfword to a 32-bit word, and writes the word to register <Rd>.\n
/// If the address is not halfword-aligned, the result is UNPREDICTABLE.
/// @param inst instruct to be executed
/// @return none 
///////////////////////////////////////////////////////////////////////////////
void TVMEngine::exec_LDRSH(Instruct & inst )
{ 
    if (isConditionPass(inst)) 
    {
        u32 data; //unpredicable, just so

		getOperandByAddressMode(inst);

        if (0x00 == (inst.info.mls.address & 0x01))
            data = readMemory(inst.info.mls.address, 2);
        SET_TCREG(inst.info.mls.rd, signExtend(data, 8));
    }    
}
/// @brief The LDRT (Load Register with Translation) instruction loads a word from \n
/// the memory address calculated by <addressing_mode> and writes it to\n
/// register <Rd>. If the address is not word-aligned, the loaded data is\n
/// rotated as for the LDR instruction
void TVMEngine::exec_LDRT(Instruct & inst ) 
{ 
    if (isConditionPass(inst)) {
        u32 value;
		getOperandByAddressMode(inst);

        u32 mask = inst.info.mls.address & 0x02;    
        if (mask == 0 )
            value = readMemory(inst.info.mls.address, 4);
        else if ((mask & 0x01) == 0x01)
            value = readMemory(inst.info.mls.address, 4) << 8;
        else if ((mask & 0x02)  == 0x02)
            value = readMemory(inst.info.mls.address, 4) << 16;
        else if ((mask & 0x03) == 0x03)
            value = readMemory(inst.info.mls.address, 4) << 24;
        
        SET_TCREG(inst.info.mls.rd, value);
    }     
}
/// @brief The STR (Store Register) instruction stores a word from register <Rd> to\n
/// the memory address calculated by <addressing_mode>.
void TVMEngine::exec_STR(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
		getOperandByAddressMode(inst);
        writeMemory(inst.info.mls.address, GET_TCREG(inst.info.mls.rd), 4);
    }   
} 
/// @brief The STRB (Store Register Byte) instruction stores a byte from the least\n
/// significant byte of register <Rd> to the memory address calculated by\n
/// <addressing_mode>.
void TVMEngine::exec_STRB(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
		getOperandByAddressMode(inst);
        writeMemory(inst.info.mls.address, 
            (u8)(GET_TCREG(inst.info.mls.rd) & 0xff), 1);
    }
}
/// @brief The STRBT (Store Register Byte with Translation) instruction stores a byte\n
/// from the least significant byte of register <Rd> to the memory address\n
/// calculated by <post_indexed_addressing_mode>. If the instruction is\n
/// executed when the processor is in a privileged mode, the memory system\n
/// is signaled to treat the access as if the processor were in User mode.
void TVMEngine::exec_STRBT(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
		getOperandByAddressMode(inst);
        writeMemory(inst.info.mls.address, (u8)(GET_TCREG(inst.info.mls.rd) & 0xff), 1);
    }
     
}
/// @brief The STRH (Store Register Halfword) instruction stores a halfword from\n
/// the least significant halfword of register <Rd> to the memory address\n
/// calculated by <addressing_mode>. If the address is not halfword-aligned,\n
/// the result is UNPREDICTABLE.
void TVMEngine::exec_STRH(Instruct & inst) { 
    if (isConditionPass(inst)) {
		getOperandByAddressMode(inst);
        writeMemory(inst.info.mls.address, 
            (u16)(GET_TCREG(inst.info.mls.rd) & 0xffff), 2);
    }
}
/// @brief The STRT (Store Register with Translation) instruction stores a word from\n
/// register <Rd> to the memory address calculated by\n
/// <post_indexed_addressing_mode>. If the instruction is executed when the \n
/// processor is in a privileged mode, the memory system is signaled to treat\n
/// the access as if the processor was in User mode.
void TVMEngine::exec_STRT(Instruct & inst) 
{ 
   if (isConditionPass(inst)) {
		getOperandByAddressMode(inst);
		writeMemory(inst.info.mls.address, GET_TCREG(inst.info.mls.rd), 4);
    }
     
}
/// @brief This form of the LDM (Load Multiple) instruction is useful for block loads,\n
/// stack operations and procedure exit sequences. It loads a non-empty subset,\n
/// or possibly all, of the general-purpose registers from sequential memory\n
/// locations.
void TVMEngine::exec_LDM(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
        std::bitset<sizeof(u16)*8> reglist = inst.info.lsm.reglist;

        if (inst.info.lsm.mode == LASM_IA_MODE) {
            /// IA mode caculate
            /// start_address = rn
            /// end_address = rn + (number_of_set_bits_in(register_list) * 4) - 4
            /// if falgW is equal to 1
            ///  (rn) = (rn) + number_of_set_bits_in(register_lst)*4
            inst.info.lsm.startAddr = GET_TCREG(inst.info.lsm.rn);
            inst.info.lsm.endAddr = 
                GET_TCREG(inst.info.lsm.rn) + reglist.count()*4 - 4;
            if (inst.info.lsm.flagW == 1) {
                u32 val = GET_TCREG(inst.info.lsm.rn) + reglist.count()*4;
                SET_TCREG(inst.info.lsm.rn, val);
            }
        }
        else if (inst.info.lsm.mode == LASM_IB_MODE) {
            /// IB mode address caculate.
            /// start_address = (rn) + 4
            /// end_address = (rn) + (number_of_set_bits_in(register_list) * 4)
            /// if falgW is equal to 1
            /// (rn) =  (rn) + number_of_set_bits_in(register_lst)*4
            inst.info.lsm.startAddr = GET_TCREG(inst.info.lsm.rn) + 4;
            inst.info.lsm.endAddr = 
                GET_TCREG(inst.info.lsm.rn) + reglist.count()*4;
           
            if (inst.info.lsm.flagW == 1)
            {
                u32 val = GET_TCREG(inst.info.lsm.rn) + reglist.count()*4;
                SET_TCREG(inst.info.lsm.rn, val);
            }
        }
        else if (inst.info.lsm.mode == LASM_DA_MODE) {            
             /// DA mode address caculate.
             /// start_address = (rn)-(number_of_set_bits_in(register_list) * 4) +4 
             /// end_address = (rn)
             /// if falgW is equal to 1
             /// rn = (rn) - number_of_set_bits_in(register_lst)*4
            inst.info.lsm.startAddr = 
                GET_TCREG(inst.info.lsm.rn) - reglist.count()*4 + 4;
            inst.info.lsm.endAddr = GET_TCREG(inst.info.lsm.rn);
           
            if (inst.info.lsm.flagW == 1) {
                u32 val = GET_TCREG(inst.info.lsm.rn) - reglist.count()*4;
                SET_TCREG(inst.info.lsm.rn, val);
            }
        }
        else if (inst.info.lsm.mode == LASM_DB_MODE) {
             /// IB mode address caculate.
             /// start_address = (rn)-(number_of_set_bits_in(register_list) * 4) 
             /// end_address = (rn)-4
             /// if falgW is equal to 1
             ///  rn = (rn) - number_of_set_bits_in(register_lst)*4
            inst.info.lsm.startAddr = GET_TCREG(inst.info.lsm.rn) - reglist.count()*4;
            inst.info.lsm.endAddr = GET_TCREG(inst.info.lsm.rn) - 4;
           
            if (inst.info.lsm.flagW == 1) {
                u32 val = GET_TCREG(inst.info.lsm.rn) - reglist.count()*4;
                SET_TCREG(inst.info.lsm.rn, val);
            }
        }

        std::bitset<32> valmask = inst.value;       

        if (inst.info.lsm.flagS == 0) { 
            ///LDM1
            ///This form of the LDM (Load Multiple) instruction is useful for
            ///block loads, stack operations and procedure exit sequences. 
            ///It loads a non-empty subset, or possibly all, of the general 
            /// purpose registers from sequential memory locations.

            u32 address = inst.info.lsm.startAddr;
            std::bitset<16>reglist = inst.info.lsm.reglist;
            for (u8 i = 0; i < R15; i++)  {
                if (reglist.test(i)) {
                    SET_TCREG(i, readMemory(address, 4));
                    address += 4;
                }
            }
            if (reglist.test(R15)) {
                u32 value = readMemory(address, 4);
                if (getArchitectVersion() >= 5) {
                    SET_TCREG(R15, value & 0xfffffffe);
                    SET_TCCPSR2(CPSR_FLAG_T, (u8)(value &0x01));
                }
                else {
                    SET_TCREG(R15, value & 0xfffffffc);
                }
                address += 4;
            }
            ASSERT(inst.info.lsm.endAddr == (address - 4));
        } 

        else if ( (inst.info.lsm.flagS == 1) && (valmask.test(15) == 1)) {
            /// LDM2
            /// This form of LDM loads User mode registers when the processor 
            /// is in a privileged mode (useful when performing process swaps,
            /// and in instruction emulators). The instruction loads a non-empty
            /// subset of the User mode general-purpose registers from sequential
            /// memory locations.
            u32 address = inst.info.lsm.startAddr;
            std::bitset<16>reglist = inst.info.lsm.reglist;
            for (u8 i = 0; i < R15; i++) {
                if (reglist.test(i)) {
                    SET_TCREG2(UserMode, i, readMemory(address, 4));
                    address += 4;
                }
                address += 4;
            }
            ASSERT(inst.info.lsm.endAddr == (address - 4));
        }

        else if ((inst.info.lsm.flagS == 1) && (valmask.test(15) == 0)) {
            /// LDM3
            /// This form of is useful for returning from an exception. 
            /// It loads a subset (or possibly all) of the general-purpose
            /// registers and the PC from sequential memory locations. 
            /// Also, the SPSR of the current mode is copied to the CPSR.
            u32 address = inst.info.lsm.startAddr;
            std::bitset<16>reglist = inst.info.lsm.reglist;
            for (u8 i = 0; i < R15; i++) {
                if (reglist.test(i)) {
                    SET_TCREG(i, readMemory(address, 4));
                    address += 4;
                }
            }
            SET_TCCPSR(GET_TCSPSR());

            u32 value = readMemory(address, 4);
            if ((getArchitectVersion() >= 5) && (GET_TCCPSR2(CPSR_FLAG_T) == 1)){
                SET_TCREG(R15, value & 0xfffffffe);
            }
            else {
                SET_TCREG(R15, value & 0xfffffffc);
            }
            address += 4;
            ASSERT(inst.info.lsm.endAddr == (address - 4));
        }       
    }     
}
/// @breif This form of the STM (Store Multiple) instruction stores a non-empty subset
/// (or possibly all) of the general-purpose registers to sequential memory locations.
void TVMEngine::exec_STM(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
        std::bitset<16> reglist = inst.info.lsm.reglist;
        if (inst.info.lsm.mode == LASM_IA_MODE) {
            /// IA mode address caculate
            /// start_address = rn
            /// end_address = rn + (number_of_set_bits_in(register_list) * 4) - 4
            /// if falgW is equal to 1
            /// (rn) = (rn) + number_of_set_bits_in(register_lst)*4
            ///
            inst.info.lsm.startAddr = GET_TCREG(inst.info.lsm.rn);
            inst.info.lsm.endAddr = 
                GET_TCREG(inst.info.lsm.rn) + reglist.count()*4 - 4;
            if (inst.info.lsm.flagW == 1) {
                u32 val = GET_TCREG(inst.info.lsm.rn) + reglist.count()*4;
                SET_TCREG(inst.info.lsm.rn, val);
            }
        }
        else if (inst.info.lsm.mode == LASM_IB_MODE) {
            /// IB mode address caculate
            /// start_address = (rn) + 4
            /// end_address = (rn) + (number_of_set_bits_in(register_list) * 4)
            /// if falgW is equal to 1
            ///  (rn) =  (rn) + number_of_set_bits_in(register_lst)*4
            inst.info.lsm.startAddr = GET_TCREG(inst.info.lsm.rn) + 4;
            inst.info.lsm.endAddr = 
                GET_TCREG(inst.info.lsm.rn) + reglist.count()*4;
           
            if (inst.info.lsm.flagW == 1) {
                u32 val = GET_TCREG(inst.info.lsm.rn) + reglist.count()*4;
                SET_TCREG(inst.info.lsm.rn, val);
            }
        }
        else if (inst.info.lsm.mode == LASM_DA_MODE) {
            /// DA mode address caculate
            /// start_address = (rn)-(number_of_set_bits_in(register_list) * 4) +4 
            /// end_address = (rn)
            /// if falgW is equal to 1
            ///  rn = (rn) - number_of_set_bits_in(register_lst)*4
            
            inst.info.lsm.startAddr = 
                GET_TCREG(inst.info.lsm.rn) - reglist.count()*4 + 4;
            inst.info.lsm.endAddr = GET_TCREG(inst.info.lsm.rn);
           
            if (inst.info.lsm.flagW == 1) {
                u32 val = GET_TCREG(inst.info.lsm.rn) - reglist.count()*4;
                SET_TCREG(inst.info.lsm.rn, val);
            }
        }
        else if (inst.info.lsm.mode == LASM_DB_MODE) {
            /// DB mode address caculate
            /// start_address = (rn)-(number_of_set_bits_in(register_list) * 4) 
            /// end_address = (rn)-4
            /// if falgW is equal to 1
            ///  rn = (rn) - number_of_set_bits_in(register_lst)*4            
            inst.info.lsm.startAddr = GET_TCREG(inst.info.lsm.rn) - reglist.count()*4;
            inst.info.lsm.endAddr = GET_TCREG(inst.info.lsm.rn) - 4;
           
            if (inst.info.lsm.flagW == 1) {
                u32 val = GET_TCREG(inst.info.lsm.rn) - reglist.count()*4;
                SET_TCREG(inst.info.lsm.rn, val);
            }
        }
        if (inst.info.lsm.flagS == 0) {
            /// STM1
            /// This form of the STM (Store Multiple) instruction stores a 
            /// non-empty subset (or possibly all) of the general-purpose
            /// registers to sequential memory locations.
            u32 address = inst.info.lsm.startAddr;
            std::bitset<16>reglist = inst.info.lsm.reglist;
            for (u8 i = 0; i < RegisterMax; i++) {
                if (reglist.test(i)) {
                    writeMemory(address, GET_TCREG(i), 4); 
                    address += 4;
                }
            }
            ASSERT(inst.info.lsm.endAddr == (address - 4));
        } 
        else {
            /// STM2
            /// This form of STM stores a subset (or possibly all) of the 
            /// User mode general-purpose registers to sequential memory locations.
            u32 address = inst.info.lsm.startAddr;
            std::bitset<16>reglist = inst.info.lsm.reglist;
            for (u8 i = 0; i < RegisterMax; i++) {
                if (reglist.test(i)) {
                    writeMemory(address, GET_TCREG2(UserMode, i), 4); 
                    address += 4;
                }
            }
            ASSERT(inst.info.lsm.endAddr == (address - 4));
        }
    }    
}
/// @brief The SWP (Swap) instruction swaps a word between registers and memory. 
/// SWP loads a word from the memory address given by the value of register <Rn>.
/// The value of register <Rm> is then stored to the memory address given by the
/// value of <Rn>, and the original loaded value is written to register <Rd>. If
/// the same register is specified for <Rd> and <Rm>, this instruction swaps the
/// value of the register and the value at the memory address.
void TVMEngine::exec_SWP(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
        u8 tmpbits = (u8)(GET_TCREG(inst.info.swi.rn) & 0x03);
        u32 val = readMemory(GET_TCREG(inst.info.swi.rn), 4);
        
        if (tmpbits == 0x00)
            val = val;
        else if (tmpbits == 0x01)
            val = rotateShiftRight(val, 8);
        else if (tmpbits == 0x00)
            val = rotateShiftRight(val, 16);
        else
            val = rotateShiftRight(val, 24);

        writeMemory(GET_TCREG(inst.info.swi.rn), GET_TCREG(inst.info.swi.rd), 4);
        SET_TCREG(inst.info.swi.rd, val);
    }
     
}
/// @brief The SWPB (Swap Byte) instruction swaps a byte between registers and memory. 
/// SWPB loads a byte from the memory address given by the value of register <Rn>.
/// The value of the least significant byte of register <Rm> is stored to the 
/// memory address given by <Rn>, the original loaded value is zero-extended 
/// to a 32-bit word, and the word is written to register <Rd>. If the same 
/// register is specified for <Rd> and <Rm>, this instruction swaps the value
/// of the least significant byte of the register and the byte value at the memory
/// address.
void TVMEngine::exec_SWPB(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
        u32 val = readMemory(GET_TCREG(inst.info.swi.rn), 1);
        writeMemory(GET_TCREG(inst.info.swi.rn), 
            (u8)(GET_TCREG(inst.info.swi.rm) & 0xff), 1);
        SET_TCREG(inst.info.swi.rd, val);
    }     
}
/// @brief The BKPT (Breakpoint) instruction causes a software breakpoint to occur.
/// This breakpoint can be handled by an exception handler installed on the 
/// prefetch abort vector. In implementations which also include debug hardware,
/// the hardware can optionally override this behavior and handle the breakpoint 
/// itself. When this occurs, the prefetch abort vector is not entered.
void TVMEngine::exec_BKPT(Instruct & inst )
{
    if (isConditionPass(inst)) {
        /// just so now
    }    
}
/// @brief The SWI (Software Interrupt) instruction causes a SWI exception
void TVMEngine::exec_SWI(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
        /// just so now
    }    
}
/// @brief The CDP (Coprocessor Data Processing) instruction tells the coprocessor 
/// whose number is cp_num to perform an operation that is independent of ARM
/// registers and memory. If no coprocessors indicate that they can execute
/// the instruction, an Undefined Instruction exception is generated.
void TVMEngine::exec_CDP(Instruct & inst )
{ 
    if (isConditionPass(inst)) {
        /// just so now
    }
}
/// @brief The LDC (Load Coprocessor) instruction loads memory data from the sequence
/// of consecutive memory addresses calculated by <addressing_mode> to the
/// coprocessor whose number is cp_num. If no coprocessors indicate that they
/// can execute the instruction, an Undefined Instruction exception is generated.
void TVMEngine::exec_LDC(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
        /// just so now
    }
}
/// @brief The MCR (Move to Coprocessor from ARM Register) instruction passes the 
/// value of register <Rd> to the coprocessor whose number is cp_num. If no 
/// coprocessors indicate that they can execute the instruction, an Undefined
/// Instruction exception is generated.
void TVMEngine::exec_MCR(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
        /// just so now
    }
} 
/// @brief The MRC (Move to ARM Register from Coprocessor) instruction causes the
/// coprocessor whose number is cp_num to transfer a value to an ARM register
/// or to the condition flags.
void TVMEngine::exec_MRC(Instruct & inst) 
{ 
    if (isConditionPass(inst)){
        /// just so now
    }
} 
/// @brief The STC (Store Coprocessor) instruction stores data from the coprocessor 
/// whose name is cp_num to the sequence of consecutive memory addresses 
/// calculated by <addressing_mode>. If no coprocessors indicate that they 
/// can execute the instruction, an Undefined Instruction exception is generated.
void TVMEngine::exec_STC(Instruct & inst) 
{ 
    if (isConditionPass(inst)) {
        u32 val = GET_TCREG(15) + inst.info.branch.targetAddress;
        SET_TCREG(14, GET_TCREG(R15) + 4);      
        SET_TCREG(15, val);
    }
     
}
/// @brief Get the operand by address mode.
void TVMEngine::getOperandByAddressMode_DPI(Instruct & inst) 
{
    u32 val = 0;
    using namespace utility;

    /* data process operands, accoring to the address mode.
     * result operand is caculated.
     */
    switch (inst.info.dpi.mode) {  
        //#<immediate>
    case DPI_IMMEDIATE_MODE:
        inst.info.dpi.shifter_operand = 
            rotateShiftRight(inst.info.dpi.immed_8, inst.info.dpi.rotate_imm * 2);
        if (inst.info.dpi.rotate_imm == 0) {
            inst.info.dpi.shifterCarryOut = GET_TCCPSR2(CPSR_FLAG_C);
        }
        else { //rotate_imm != 0
            inst.info.dpi.shifterCarryOut = (u8)(inst.info.dpi.shifter_operand >> 31);
        }
        break;

        //#<rm>
    case DPI_RM_MODE:
		if(inst.info.dpi.rm == R15)
            inst.info.dpi.shifter_operand = GET_TCREG(inst.info.dpi.rm) + 0x08;//just so ?
		else
			inst.info.dpi.shifter_operand = GET_TCREG(inst.info.dpi.rm);
        inst.info.dpi.shifterCarryOut = GET_TCCPSR2(CPSR_FLAG_C);
        break;

        //<rm>, LSL #<shift_imm>
    case DPI_RM_LSL_SHIFT_MODE:
        if ( inst.info.dpi.shift_imm == 0) {
            inst.info.dpi.shifter_operand = GET_TCREG(inst.info.dpi.rm);
            inst.info.dpi.shifterCarryOut = GET_TCCPSR2(CPSR_FLAG_C);
        }

        else {
            inst.info.dpi.shifter_operand = 
                logicShiftLeft(GET_TCREG(inst.info.dpi.rm), inst.info.dpi.shift_imm);
            inst.info.dpi.shifterCarryOut = (u8)(GET_TCREG(inst.info.dpi.rm) >> (32 - inst.info.dpi.shift_imm)) & 0x01;   
        }
        break;

        //<rm>, LSL <rs>
    case DPI_RM_LSL_RS_MODE:
        val = GET_TCREG(inst.info.dpi.rs)& 0xff; 
        if ( val == 0) {
            inst.info.dpi.shifter_operand = GET_TCREG(inst.info.dpi.rm);
            inst.info.dpi.shifterCarryOut = GET_TCCPSR2(CPSR_FLAG_C);
        }

        else if (val < 32) {
            inst.info.dpi.shifter_operand = 
                logicShiftLeft(GET_TCREG(inst.info.dpi.rm), (u8)val);
            inst.info.dpi.shifterCarryOut = 
                (u8)(GET_TCREG(inst.info.dpi.rm) >> (32 - val)); 
        }

        else if (val == 32) {
            inst.info.dpi.shifter_operand = 0;
            inst.info.dpi.shifterCarryOut = (u8)(val & 0x01);
        }

        else { //rs[7:0] > 32
            inst.info.dpi.shifter_operand = 0;
            inst.info.dpi.shifterCarryOut = 0;
        }

        //<rm>, LSR #shift_imm
    case DPI_RM_LSR_SHIFT_MODE:
        if ( inst.info.dpi.shift_imm == 0) {
            inst.info.dpi.shifter_operand = 0;
            inst.info.dpi.shifterCarryOut = (u8)(GET_TCREG(inst.info.dpi.rm) >> 31);
        }

        else {
            inst.info.dpi.shifter_operand = 
                logicShiftRight(GET_TCREG(inst.info.dpi.rm), inst.info.dpi.shift_imm);
            inst.info.dpi.shifterCarryOut =
                (u8)(GET_TCREG(inst.info.dpi.rm) >> (inst.info.dpi.shift_imm - 1));
        }
        break;
    
        //<rm>, LSR <rs>
    case DPI_RM_LSR_RS_MODE:
        val = GET_TCREG(inst.info.dpi.rs)& 0xff; 
        if ( val == 0) {
            inst.info.dpi.shifter_operand = GET_TCREG(inst.info.dpi.rm);
            inst.info.dpi.shifterCarryOut = GET_TCCPSR2(CPSR_FLAG_C);
        }

        else if (val < 32) {
            inst.info.dpi.shifter_operand = 
                logicShiftRight(GET_TCREG(inst.info.dpi.rm), (u8)val);
            inst.info.dpi.shifterCarryOut = 
                (u8)(GET_TCREG(inst.info.dpi.rm) >> (val - 1));                
        }

        else if (val == 32) {
            inst.info.dpi.shifter_operand = 0;
            inst.info.dpi.shifterCarryOut = (u8)((val >> 31) & 0x01);
        }

        else { //rs[7:0] > 32
            inst.info.dpi.shifter_operand = 0;
            inst.info.dpi.shifterCarryOut = 0;
        }
        break;

        //<rm>, ASR #shift_imm
    case DPI_RM_ASR_SHIFT_MODE:
        if ( inst.info.dpi.shift_imm == 0) {
            if ((GET_TCREG(inst.info.dpi.rm) & 0x8fffffff) == 0) {
                inst.info.dpi.shifter_operand = 0;
                inst.info.dpi.shifterCarryOut = (u8)(GET_TCREG(inst.info.dpi.rm) >> 31);
            }
            else {//rm[31] == 1
                inst.info.dpi.shifter_operand = 0xfffffff;
                inst.info.dpi.shifterCarryOut = (u8)(GET_TCREG(inst.info.dpi.rm) >> 31);

            }
        }
        else {
            inst.info.dpi.shifter_operand = 
                arithShiftRight(GET_TCREG(inst.info.dpi.rm), inst.info.dpi.shift_imm);
            inst.info.dpi.shifterCarryOut = 
                (u8)(GET_TCREG(inst.info.dpi.rm) >> (inst.info.dpi.shift_imm - 1));
        }
        break;

        //<rm>, ASR <rs>
    case DPI_RM_ASR_RS_MODE:
        if ((GET_TCREG(inst.info.dpi.rs) & 0xff) == 0) {
            inst.info.dpi.shifter_operand = GET_TCREG(inst.info.dpi.rm);
            inst.info.dpi.shifterCarryOut = GET_TCCPSR2(CPSR_FLAG_C);
        }
        else if ((GET_TCREG(inst.info.dpi.rs) & 0x0f) < 32) {
            inst.info.dpi.shifter_operand = 
                arithShiftRight(GET_TCREG(inst.info.dpi.rm),
                               (u8)((GET_TCREG(inst.info.dpi.rs) & 0x0f)));
            inst.info.dpi.shifterCarryOut = 
                (u8)(GET_TCREG(inst.info.dpi.rm) >> (GET_TCREG(inst.info.dpi.rs) - 1));
        }
        else {
            if ((GET_TCREG(inst.info.dpi.rm) & 0x8fff) == 0) {
                inst.info.dpi.shifter_operand = 0;
                inst.info.dpi.shifterCarryOut = (u8)(GET_TCREG(inst.info.dpi.rm) >> 31);
            }
            else {
                inst.info.dpi.shifter_operand = 0xffffffff;
                inst.info.dpi.shifterCarryOut = (u8)(GET_TCREG(inst.info.dpi.rm) >> 31);
            }
        }
        break;
        
        //<rm>, ROR #<shift_imm>
    case DPI_RM_ROR_SHIFT_MODE:
        if ( inst.info.dpi.shift_imm == 0) {
	        inst.info.dpi.shifter_operand = logicShiftLeft(GET_TCCPSR2(CPSR_FLAG_C),31)
		        |logicShiftRight(GET_TCREG(inst.info.dpi.rm),1);
			inst.info.dpi.shifterCarryOut = (u8)(GET_TCREG(inst.info.dpi.rm) & 0x01);
            //just so.
        }
        else {
            inst.info.dpi.shifter_operand = 
                rotateShiftRight(GET_TCREG(inst.info.dpi.rm), inst.info.dpi.shift_imm);
            inst.info.dpi.shifterCarryOut = 
                (u8)(GET_TCREG(inst.info.dpi.rm) >> (inst.info.dpi.shift_imm - 1));
        }
        break;

        //<rm>, ROR, <rs>
    case DPI_RM_ROR_RS_MODE:
        val = GET_TCREG(inst.info.dpi.rs); 
        if ( (val & 0xff) == 0) {
            inst.info.dpi.shifter_operand = GET_TCREG(inst.info.dpi.rm);
            inst.info.dpi.shifterCarryOut = GET_TCCPSR2(CPSR_FLAG_C);
        }

        else if ( (val & 0x0f) == 0) {
            inst.info.dpi.shifter_operand = GET_TCREG(inst.info.dpi.rm);
            inst.info.dpi.shifterCarryOut = (u8)(GET_TCREG(inst.info.dpi.rm) >> 31);
        }
        else { 
            inst.info.dpi.shifter_operand = 
                rotateShiftRight(GET_TCREG(inst.info.dpi.rm),
                                (u8)((GET_TCREG(inst.info.dpi.rm)& 0x0f)));
        }

        break;

        //<rm>, RRX
    case DPI_RM_RRX_MODE:
        inst.info.dpi.shifter_operand = logicShiftLeft(GET_TCCPSR2(CPSR_FLAG_C),31)
            |logicShiftRight(GET_TCREG(inst.info.dpi.rm),1);
        inst.info.dpi.shifterCarryOut = (u8)(GET_TCREG(inst.info.dpi.rm) & 0x01);
        break;
    default:
        break;
    };
}// end for data process instruction

/// Get the operand by address mode.
void TVMEngine::getOperandByAddressMode_LST(Instruct & inst) 
{
    u32 index = 0;
    u8 addOffset = 0;
    if (inst.info.mls.rn == R15)
        addOffset = 8;
    switch (inst.info.mls.mode) {
            //<rn>, +/-<offset>
        case LDSR_RN_OFFSET12_WB_MODE:
            if (inst.info.mls.flagU == 1) {
                inst.info.mls.address = GET_TCREG(inst.info.mls.rn) + addOffset
                    + inst.info.mls.offset ;        
            }
            else {
                inst.info.mls.address = GET_TCREG(inst.info.mls.rn) + addOffset
                    - inst.info.mls.offset;
            } 
            break;

            //<rn>, +/-<rm>
        case LDSR_RN_RM_WB_MODE:
            if (inst.info.mls.flagU == 1) {
                inst.info.mls.address = GET_TCREG(inst.info.mls.rn) + addOffset 
                    + GET_TCREG(inst.info.mls.rm);
            }
            else ﬂ{
                inst.info.mls.address = GET_TCREG(inst.info.mls.rn) + addOffset
                    - GET_TCREG(inst.info.mls.rm);
            }  
            break;

            //<Rn>, +/-<Rm>, Shift #<shift_imm>
        case LDSR_RN_RM_SHIFT_SHIFTM_WB_MODE:
            switch (inst.info.mls.shift) {
            case 0: //LSL
                index = logicShiftLeft(GET_TCREG(inst.info.mls.rm), inst.info.mls.shift_imm);
                break;
            case 1: //LSR
                if (inst.info.mls.shift_imm == 0) 
                    index = 0;
                else 
                    index = logicShiftRight(GET_TCREG(inst.info.mls.rm), inst.info.mls.shift_imm);
            case 2://ASR
            if (inst.info.mls.shift_imm = 0) { //ASR #32
                    std::bitset<sizeof(u32)*8> rmvalue = GET_TCREG(inst.info.mls.rm);
                    if (rmvalue.test(31) == true) 
                        index = 0xffffffff;
                    else 
                        index = 0;
                }
                else {
                    index = arithShiftRight(GET_TCREG(inst.info.mls.rm), inst.info.mls.shift_imm);
                }
                break;
            case 3://ROR or RRX
                if (inst.info.mls.shift_imm == 0) { //RRX
                    index = logicShiftLeft(GET_TCCPSR2(CPSR_FLAG_C), 31) | 
                            logicShiftRight(GET_TCREG(inst.info.mls.rm), 1);
                }
                else { //ROR
                    index = rotateShiftRight(GET_TCREG(inst.info.mls.rm),inst.info.mls.shift_imm);
                }
                break;
            default:
                break;
            };

            break;

        case LDSR_RN_OFFSET12_H_WB_MODE:
            //<rn>, #+/-<offset_12>
            if (inst.info.mls.flagU == 1) {
                inst.info.mls.address = 
                    GET_TCREG(inst.info.mls.rn) + inst.info.mls.offset;
            }
            else {
                inst.info.mls.address = 
                    GET_TCREG(inst.info.mls.rn) - inst.info.mls.offset;               

            }
            if (isConditionPass(inst)) {
                SET_TCREG(inst.info.mls.rn, inst.info.mls.address);
            }
            break;

        case LDSR_RN_RM_H_WB_MODE:
            //<rn>, #+/-<rm>
            if (inst.info.mls.flagU == 1) {
                inst.info.mls.address =
                    GET_TCREG(inst.info.mls.rn) + GET_TCREG(inst.info.mls.rm);
            }
            else {
                inst.info.mls.address =
                    GET_TCREG(inst.info.mls.rn) - GET_TCREG(inst.info.mls.rm);
            }
            break;

        case LDSR_RN_RM_SHIFT_SHIFTM_H_WB_MODE:
            //<Rn>, +/-<Rm>, Shift #<shift_imm>
            switch (inst.info.mls.shift) {
            case 0: //LSL
                index = GET_TCREG(inst.info.mls.rm) << inst.info.mls.shift_imm;
                break;
            case 1: //LSR
                if (inst.info.mls.shift_imm == 0) {
                    index = 0;
                }
                else {
                    index = GET_TCREG(inst.info.mls.rm) >> inst.info.mls.shift_imm;
                }
            case 2://ASR
                if (inst.info.mls.shift_imm = 0){ //ASR #32
                    std::bitset<sizeof(u32)*8> rmvalue = GET_TCREG(inst.info.mls.rm);
                    if (rmvalue.test(31) == true) 
                        index = 0xffffffff;
                    else 
                        index = 0;
                }
                else {
                    index = GET_TCREG(inst.info.mls.rm) >> inst.info.mls.shift_imm;
                }
                break;
            case 3://ROR or RRX
                if (inst.info.mls.shift_imm == 0) { //RRX
                    index = (GET_TCCPSR2(CPSR_FLAG_C) << 31 ) || 
                        (GET_TCREG(inst.info.mls.rm) >> 1);
                }
                else { //ROR
                    index = GET_TCREG(inst.info.mls.rm) >> inst.info.mls.shift_imm;
                }
                break;
            default:
                break;
            };
            if (inst.info.mls.flagU == 1) {
                inst.info.mls.address =
                    GET_TCREG(inst.info.mls.rn) + index;
            }
            else {
                inst.info.mls.address =
                    GET_TCREG(inst.info.mls.rn) - index;
            }
            if (isConditionPass(inst)) {
                SET_TCREG(inst.info.mls.rn, inst.info.mls.address);
            }
            break;

        case LDSR_RN_OFFSET12_V_WB_MODE:
            inst.info.mls.address = GET_TCREG(inst.info.mls.rn);
            if (isConditionPass(inst)) {
                if (inst.info.mls.flagU == 1) {
                    SET_TCREG(inst.info.mls.rn,
                        GET_TCREG(inst.info.mls.rn) + inst.info.mls.offset);
                }
                else {
                    SET_TCREG(inst.info.mls.rn,
                        GET_TCREG(inst.info.mls.rn) - inst.info.mls.offset);

                }
            }
            break;

        case LDSR_RN_RM_V_WB_MODE:
            inst.info.mls.address = GET_TCREG(inst.info.mls.rn);
            if (isConditionPass(inst)) {
                if (inst.info.mls.flagU == 1) {
                    SET_TCREG(inst.info.mls.rn,
                        GET_TCREG(inst.info.mls.rn) + GET_TCREG(inst.info.mls.rm));
                }
                else {
                    SET_TCREG(inst.info.mls.rn,
                        GET_TCREG(inst.info.mls.rn) - GET_TCREG(inst.info.mls.rm));
                }
            }
            break;

        case LDSR_RN_RM_SHIFT_V_WB_MODE:{
            switch (inst.info.mls.shift) {
                case 0: //LSL
                    index = GET_TCREG(inst.info.mls.rm) << inst.info.mls.shift_imm;
                    break;
                case 1: //LSR
                    if (inst.info.mls.shift_imm == 0) {
                        index = 0;
                    }
                    else {
                        index = GET_TCREG(inst.info.mls.rm) >> inst.info.mls.shift_imm;
                    }
                case 2://ASR
                    if (inst.info.mls.shift_imm = 0){ //ASR #32
                        std::bitset<sizeof(u32)*8> rmvalue = GET_TCREG(inst.info.mls.rm);
                        if (rmvalue.test(31) == true) 
                            index = 0xffffffff;
                        else 
                            index = 0;
                    }
                    else {
                        index = GET_TCREG(inst.info.mls.rm) >> inst.info.mls.shift_imm;
                    }
                    break;
                case 3://ROR or RRX
                    if (inst.info.mls.shift_imm == 0) { //RRX
                        index = (GET_TCCPSR2(CPSR_FLAG_C) << 31 ) || 
                            (GET_TCREG(inst.info.mls.rm) >> 1);
                    }
                    else { //ROR
                        index = GET_TCREG(inst.info.mls.rm) >> inst.info.mls.shift_imm;
                    }
                    break;
                default:
                    break;
                }; //end switch
                if (isConditionPass(inst)) {
                    if (inst.info.mls.flagU == 1) {
                        SET_TCREG(inst.info.mls.rn,
                            GET_TCREG(inst.info.mls.rn) + index);
                    }
                    else {
                        SET_TCREG(inst.info.mls.rn, GET_TCREG(inst.info.mls.rn) - index);
                    }
                }
            }
            break;
        case LDSR_RN_OFFSET8_MODE: {
                u8 offset8 = (inst.info.mls.immedH << 4)|inst.info.mls.immedL;
                if (inst.info.mls.flagU == 1) {
                    inst.info.mls.address = GET_TCREG(inst.info.mls.rn) + offset8;
                }
                else { 
                    inst.info.mls.address = GET_TCREG(inst.info.mls.rn) - offset8;
                }
            }
            break;
        case LDSR_RN_RM_MODE:
            if (inst.info.mls.flagU == 1) {
                inst.info.mls.address = 
                    GET_TCREG(inst.info.mls.rn) + GET_TCREG(inst.info.mls.rm);
            }
            else { 
                inst.info.mls.address =
                    GET_TCREG(inst.info.mls.rn) - GET_TCREG(inst.info.mls.rm);
            }
            break;
        case LDSR_RN_OFFSET8_H_MODE: {
                u8 offset8 = (inst.info.mls.immedH << 4)|inst.info.mls.immedL;
                if (inst.info.mls.flagU == 1){
                    inst.info.mls.address = GET_TCREG(inst.info.mls.rn) + offset8;
                }
                else {
                    inst.info.mls.address = GET_TCREG(inst.info.mls.rn) - offset8;
                }
                if (isConditionPass(inst)) {
                    SET_TCREG(inst.info.mls.rn, inst.info.mls.address);
                }
            }
            break;
        case LDSR_RN_RM_H_MODE: {
                if (inst.info.mls.flagU == 1){
                    inst.info.mls.address = 
                        GET_TCREG(inst.info.mls.rn) + GET_TCREG(inst.info.mls.rm);
                }
                else { 
                    inst.info.mls.address =
                        GET_TCREG(inst.info.mls.rn) - GET_TCREG(inst.info.mls.rm);
                }
                if (isConditionPass(inst)) {
                    SET_TCREG(inst.info.mls.rn, inst.info.mls.address);
                }
            }
            break;
        case LDSR_RN_OFFSET_V_MODE: {
                inst.info.mls.address = GET_TCREG(inst.info.mls.rn);
                u8 offset8 = (inst.info.mls.immedH << 4)|inst.info.mls.immedL;
                if (isConditionPass(inst)) {
                    if (inst.info.mls.flagU == 1) {
                        inst.info.mls.address = GET_TCREG(inst.info.mls.rn) + offset8;
                    }
                    else { 
                        inst.info.mls.address = GET_TCREG(inst.info.mls.rn) - offset8;
                    }
                }
            }
            break;
        case LDSR_RN_RM_V_MODE: {
                inst.info.mls.address = GET_TCREG(inst.info.mls.rn);
                if (isConditionPass(inst)) {
                    if (inst.info.mls.flagU == 1) {
                        SET_TCREG(inst.info.mls.rn, 
                            GET_TCREG(inst.info.mls.rn) + GET_TCREG(inst.info.mls.rm));
                    }
                    else { 
                        SET_TCREG(inst.info.mls.rn, 
                           GET_TCREG(inst.info.mls.rn) - GET_TCREG(inst.info.mls.rm));
                    }
                }
            }
            break;
        default:
                break;

    };
}


/// @brief Get the operand by address mode.
void TVMEngine::getOperandByAddressMode_LSM(Instruct & inst) 
{
    std::bitset<16> reglist = inst.info.lsm.reglist;

    switch (inst.info.lsm.mode) {
        //increament after
        case LASM_IA_MODE:
            inst.info.lsm.startAddr = GET_TCREG(inst.info.lsm.rn);        
            inst.info.lsm.endAddr = 
                GET_TCREG(inst.info.lsm.rn) + reglist.count()*4 - 4;
            if (isConditionPass(inst) && inst.info.lsm.flagW  == 1) {
                SET_TCREG(inst.info.lsm.rn, 
                    GET_TCREG(inst.info.lsm.rn) + reglist.count()*4);
            }
            break;

        case LASM_IB_MODE:
            inst.info.lsm.startAddr = GET_TCREG(inst.info.lsm.rn) + 4;
            inst.info.lsm.endAddr = 
                GET_TCREG(inst.info.lsm.rn) + reglist.count()*4;
        
            if (isConditionPass(inst) && inst.info.lsm.flagW  == 1) {
                SET_TCREG(inst.info.lsm.rn,
                    GET_TCREG(inst.info.lsm.rn) + reglist.count()*4);
            }
            break;

        case LASM_DA_MODE:
            inst.info.lsm.startAddr = 
                GET_TCREG(inst.info.lsm.rn) - reglist.count()*4 + 4;
            inst.info.lsm.endAddr = GET_TCREG(inst.info.lsm.rn);

            if (isConditionPass(inst) && inst.info.lsm.flagW  == 1) {
                SET_TCREG(inst.info.lsm.rn,
                    GET_TCREG(inst.info.lsm.rn) - reglist.count()*4);
            }
            break;

        case LASM_DB_MODE:
            inst.info.lsm.startAddr = 
                GET_TCREG(inst.info.lsm.rn) - reglist.count()*4;
            inst.info.lsm.endAddr = GET_TCREG(inst.info.lsm.rn) - 4;

            if (isConditionPass(inst) && inst.info.lsm.flagW  == 1) {
                SET_TCREG(inst.info.lsm.rn,
                    GET_TCREG(inst.info.lsm.rn) - reglist.count()*4);
            }
            break;
    };
}
/// @brief Get the operand by address mode.
void TVMEngine::getOperandByAddressMode_LSC(Instruct & inst) 
{
}

/// @brief get operand by addres mode
void TVMEngine::getOperandByAddressMode(Instruct & inst)
{
    if ((inst.type <= INST_MNV) && (inst.type >= INST_AND)) {
        return getOperandByAddressMode_DPI(inst);
    }    

    else if ((inst.type <= INST_STRT) && (inst.type >= INST_LDR)) {
        return getOperandByAddressMode_LST(inst);
    }
    else if ((inst.type <= INST_STM) && (inst.type >= INST_LDM)) {
        return getOperandByAddressMode_LSM(inst);
    }
    else if ((inst.type <= INST_STC) && (inst.type >= INST_CDP)) {
        return getOperandByAddressMode_LSC(inst);
    }
}

/// @breif read memory
u32 TVMEngine::readMemory(u32 address, u8 size) 
{
    return _bus->readData(address, size);
}
/// @brief write memory
u32 TVMEngine::writeMemory(u32 address, u32 data, u8 size) 
{
    return _bus->writeData(address, data, size);
}

/// @brief transfor engine mode
int TVMEngine::TransformMode(u32 val) 
{
	TVMEngine mode = InvalidMode;

	switch(val & 0x1f) {
    	case 0x00://User26
    		mode = UserMode;
    		break;
    	case 0x01://FIQ26
    		mode = FIQMode;
    		break;
    	case 0x02://IRQ26
    		mode = IRQMode;
    		break;
    	case 0x03://SVC26
    		mode = SupervisorMode;
    		break;
    	case 0x10://User
    		mode = UserMode;
    		break;
    	case 0x11://FIQ
    		mode = FIQMode;
    		break;
    	case 0x12://IRQ
    		mode = IRQMode;
    		break;
    	case 0x13://SVC
    		mode = SupervisorMode;
    		break;
    	case 0x17://ABT
    		mode = AbortMode;
    		break;
    	case 0x1B://UND
    		mode = UndefinedMode;
    		break;
    	default:
    		//do something ?
    		break;
    }
	return mode;
}

//read/write support
void TVMEngine::readWord(u32 address, u32 &value)
{
    value = _bus->readData(address, 4);
}
void TVMEngine::readHalfWord(u32 address, u32 &value)
{
    value = _bus->readData(address, 2);
}
void TVMEngine::readByte(u32 address, u32 &value)
{
    value = _bus->readData(address, 1);
}
void TVMEngine::writeWord(u32 address, u32 value)
{
    _bus->writeData(address, value, 4);
}
void TVMEngine::writeHalfword(u32 address, u32 value)
{
    _bus->writeData(address, value, 2);
}
void TVMEngine::writeByte(u32 address, u32 value)
{
    _bus->writeData(address, value, 1);
}

//register access interface
u32 TVMEngine::getRegister(u8 mode, u8 reg)
{
    if (mode == UserMode || mode == SystemMode) 
        return _curRegister[UserMode][reg];

    else if ((mode == FIQMode) && (reg > R7))
        return _curRegister[FIQMode][reg];

	else if ( (reg == R13) || (reg == R14))
        return _curRegister[mode][reg];
    else
        return _curRegister[UserMode][reg];
}
void TVMEngine::setRegister(u8 mode, u8 reg, u32 value)
{
    if (mode == UserMode || mode == SystemMode) 
            _curRegister[UserMode][reg] = value;

    else if ((mode == FIQMode) && (reg > R7) && (reg < R15))
        _curRegister[FIQMode][reg] = value;
    
	else if ( (reg == R13) || (reg == R14)) 
        _curRegister[mode][reg] = value;
    else
        _curRegister[UserMode][reg] = value;

}
u32 TVMEngine::getPC(u8 mode)
{
    return getRegister(mode, R15);
}
void TVMEngine::setPC(u8 mode, u32 value)
{
    setRegister(mode, R15, value);
}

u32 TVMEngine::getNextPC(u8 mode)
{    
    return 0;
}
u32 TVMEngine::getCPSR()
{
    return this->_cpsr;
}
void TVMEngine::setCPSR(u32 value)
{
    this->_cpsr = value;
}
u32 TVMEngine::getSPSR(u8 mode)
{
    if ((mode != UserMode) && (mode != SystemMode))
        return _spsr[mode];
    else
        return _spsr[UserMode];
}
void TVMEngine::setSPSR(u8 mode , u32 value)
{
    if ((mode != UserMode) && (mode != SystemMode))
        _spsr[mode] = value;
    else
        _spsr[UserMode] = value; 
}

