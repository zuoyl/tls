/// tvm
/// virtual machine for toyable language

#ifndef TVM_ENGINm_INC
#define TVM_ENGINm_INC

#include <iostream>
#include <string>
#include <map>
#include <bitset>

using namespace std;

 
enum TVMEnginMode  {
    InvalidMode = -1,
    UserMode,              // User mode
    SystemMode,            // System mode
    TVMEngineModeMax
 };

enum TVMRegister {
    RegisterMin = -1,
    R0, R1, R2, R3, R4, R5, R6,
    R7, R8, R9, R10, R11, R12, 
    R13,R14, R15,
    TVMRegisterMax
};


class  TVMEngine {
public:
    TVMEngine(int mode){}  
    TVMEngine(){}
    void run();
    void reset(u32 reason);
    void stop(u32 reason);
    //instruction exectution's control interface
    void executeInstrcutions(u32 ninst);
    //read/write support
    void readWord(u32 address, u32 &value);
    void readHalfWord(u32 address, u32 &value);
    void readByte(u32 address, u32 &value);
    void writeWord(u32 address, u32 value);
    void writeHalfword(u32 address, u32 value);
    void writeByte(u32 address, u32 value);

    //register access interface
    u32 getRegister(u8 mode, u8 reg);
    void setRegister(u8 mode, u8 reg, u32 value);
 
    void registerExceptionHandler(TVMExceptionHandler handler, u32 param);
    void unregisetExceptionHandler();
 

private:
    void fetch(Instruct &inst);
    void decode(Instruct &inst);
    void exec(Instruct &inst);

    //individual instruction execution functions    
    void exec_B(Instruct &inst);
    void exec_BL(Instruct &inst);
    void exec_BLX1(Instruct &inst);
    void exec_BLX2(Instruct &inst);
    void exec_BX(Instruct &inst);
    void exec_AND(Instruct &inst);
    void exec_EOR(Instruct &inst);
    void exec_SUB(Instruct &inst);
    void exec_RSB(Instruct &inst);
    void exec_ADD(Instruct &inst);
    void exec_ADC(Instruct &inst);
    void exec_SBC(Instruct &inst);
    void exec_RSC(Instruct &inst);    
    void exec_TST(Instruct &inst);
    void exec_TEQ(Instruct &inst);
    void exec_CMP(Instruct &inst);
    void exec_CMN(Instruct &inst);
    void exec_ORR(Instruct &inst);
    void exec_MOV(Instruct &inst);
    void exec_BIC(Instruct &inst);
    void exec_MVN(Instruct &inst);
    void exec_MLA(Instruct &inst);
    void exec_MUL(Instruct &inst);
    void exec_SMLAL(Instruct &inst);
    void exec_SMULL(Instruct &inst);
    void exec_UMLAL(Instruct &inst);
    void exec_UMULL(Instruct &inst);
    void exec_CLZ(Instruct &inst);
    void exec_MRS(Instruct &inst);
    void exec_MSR(Instruct &inst);
    void exec_LDR(Instruct &inst);
    void exec_LDRB(Instruct &inst);
    void exec_LDRBT(Instruct &inst);
    void exec_LDRH(Instruct &inst);
    void exec_LDRSB(Instruct &inst);
    void exec_LDRSH(Instruct &inst);
    void exec_LDRT(Instruct &inst);
    void exec_STR(Instruct &inst); 
    void exec_STRB(Instruct &inst);
    void exec_STRBT(Instruct &inst);
    void exec_STRH(Instruct &inst);
    void exec_STRT(Instruct &inst);
    void exec_LDM(Instruct &inst);
    void exec_STM(Instruct &inst);
    void exec_SWP(Instruct &inst);
    void exec_SWPB(Instruct &inst);
    void exec_BKPT(Instruct &inst);
    void exec_SWI(Instruct &inst);
    void exec_CDP(Instruct &inst);
    void exec_LDC(Instruct &inst); 
    void exec_MCR(Instruct &inst); 
    void exec_MRC(Instruct &inst); 
    void exec_STC(Instruct &inst);    
private:
    void getOperandByAddressMode(Instruct &inst);
    void getOperandByAddressMode_DPI(Instruct &inst);
    void getOperandByAddressMode_LST(Instruct &inst);
    void getOperandByAddressMode_LSM(Instruct &inst);
    void getOperandByAddressMode_LSC(Instruct &inst);

    u32 readMemory(u32 address, u8 size);
    u32 writeMemory(u32 address, u32 data, u8 size);
    bool isConditionPass(Instruct &inst);
    void getAddressMode(u8*buf, u8 size, u16 &mode, Instruct &inst);
    inline u8 getCPSRValue(u8 field) {
        std::bitset<32> cpsr = m_cpsr;
        return cpsr.test(field);
    }
    inline void setCPSRValue(u8 field, u8 value) {
        if (field != CPSRm_FLAGm_MODE) {
            std::bitset<32> cpsr = m_cpsr;
            cpsr.set(field, (value == 1)?true:false);
            m_cpsr = (u32)cpsr.tom_ulong();
        }
    }

    inline bool isCurrentModeHasSPSR(){
        return ((m_mode != NormalMode) && (m_mode != SystemMode));
    }
    inline bool isInAPrivilegeMode() {
        return (m_mode != NormalMode);
    }
	inline void resetAllRegister() {
		memset(m_regs,0x00,sizeof(m_regs));
	}

	inline void bakRegister() {
		memcpy(m_regsb,m_regs,sizeof(m_regsb));
	}
	inline void bakCPSR() {
		m_cpsrb = m_cpsr;
	}
    TVMEngineMode TransformMode(u32 val);
    
protected:
    //registers definition
    u32 m_regs[TVMEgnineModeMax][TVMRegisterMax];
    u32 m_regsb[TVMEgnineModeMax][TVMRegisterMax];
    u32 m_spsr[TVMEgnineModeMax];
    u32 m_cpsr;  
    u32 m_cpsrb;

    bool m_mode;
    bool m_isThumb;
    ExceptionHandler m_exceptHandler;
    u32 m_exceptParam;
    u32 m_haltReason;
    bool m_isHalted;
};

} //namespace vxware


#endif //PROCESSORm_INCm_
