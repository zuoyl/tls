/// tvm
/// virtual machine for toyable language

#ifndef TVMINSTRUCT_INC
#define TVMINSTRUCT_INC

 enum  InstructType {
    INST_INVALID = -1,
    INST_B,         //branch  //1 
    INST_BL,        //branch with link
    INST_BLX1,       //branch with link and exchange
    INST_BLX2,
    INST_BX,        //branch and exchange instructions
    INST_AND,
    INST_EOR,
    INST_SUB,
    INST_RSB,
    INST_ADD,
    INST_ADC,
    INST_SBC,
    INST_RSC,
    INST_TST,
    INST_TEQ, 
    INST_CMP,
    INST_CMN, 
    INST_ORR,
    INST_MOV,
    INST_BIC,    //20
    INST_MNV,
    INST_MLA,
    INST_MUL,
    INST_SMLAL,
    INST_SMULL,
    INST_UMLAL,
    INST_UMULL,
    INST_CLZ,       //count leading zeros
    INST_MRS,       //move PSR to general registers
    INST_MSR,       //move general registers to PSR
    INST_LDR,       //load u32
    INST_LDRB,      //load u8
    INST_LDRBT,     //load u8 with user privilege
    INST_LDRH,      //load half u32
    INST_LDRSB,     //load signed u8
    INST_LDRSH,     //load sined half u32
    INST_LDRT,      //load u32 with user privilege
    INST_STR,       //store u32
    INST_STRB,      //store u8s
    INST_STRBT,     //store u8 with user privilgge
    INST_STRH,      //store half u32
    INST_STRT,      //store u32 with user privilege
    INST_LDM,
    INST_STM,
    INST_SWP,
    INST_SWPB,
    INST_BKPT,
    INST_SWI,
    INST_CDP,       //coprocess data process
    INST_LDC,       //load coprocess registers
    INST_MCR,       //move to coprocess from ARM registers
    INST_MRC,       //move ARM registers from coprocessor
    INST_STC,       //store coprocessor registers
    INST_NOP,
    INST_Undefined, //undefined instruction
    INST_MAX
};


//instruct address mode
enum  InstructAddressMode {
    MIN_MODE = -1,
    //data process instruction addressmode 
    DPI_IMMEDIATE_MODE,                 //#immediate 
    DPI_RM_MODE,                        //<Rm>  
    DPI_RM_LSL_SHIFT_MODE,              //<Rm>, LSL #<shift_imm> 
    DPI_RM_LSL_RS_MODE,                 //<Rm>, LSL <Rs>
    DPI_RM_LSR_SHIFT_MODE,              //<Rm>, LSL <Rs> #<shift_imm>
    DPI_RM_LSR_RS_MODE,                 //<Rm>, LSR <Rs>
    DPI_RM_ASR_SHIFT_MODE,              //<Rm>, ASR #<shift_imm>
    DPI_RM_ASR_RS_MODE,                 //<Rm>, ASR <Rs>
    DPI_RM_ROR_SHIFT_MODE,              //<Rm>, ROR #<shift_imm>
    DPI_RM_ROR_RS_MODE,                 //<Rm>, ROR <Rs>
    DPI_RM_RRX_MODE,                    //<Rm>, RRX

    //load and store address mode for word and unsigned bytes
    LDSR_RN_OFFSET12_WB_MODE,           //[<Rn>, #+/-<offset12>] 
    LDSR_RN_RM_WB_MODE,                 //[<Rn>, #+/-<Rm>]  
    LDSR_RN_RM_SHIFT_SHIFTM_WB_MODE,    //[<Rn>, #+/-<Rm>, <shift> #<shift_imm>] 
    LDSR_RN_OFFSET12_H_WB_MODE,         //[<Rn>, #+/-<offset12>]!        
    LDSR_RN_RM_H_WB_MODE,               //[<Rn>, #+/-<Rm>]!
    LDSR_RN_RM_SHIFT_SHIFTM_H_WB_MODE,  //[<Rn>, #+/-<Rm>, <shift> #<shift_imm>]! 
    LDSR_RN_OFFSET12_V_WB_MODE,         //[<Rn>], #+/-<offset12>
    LDSR_RN_RM_V_WB_MODE,               //[<Rn>], #+/-<Rm>
    LDSR_RN_RM_SHIFT_V_WB_MODE,         //[<Rn>], #+/-<Rm>, <shift> #<shift_imm>

    //misc load and store address mode
    LDSR_RN_OFFSET8_MODE,       //[<Rn>, #+/-<offset_8>]
    LDSR_RN_RM_MODE,            //[<Rn>, #+/-<rm>]
    LDSR_RN_OFFSET8_H_MODE,     //[<Rn>, #+/-<offset_8>]!
    LDSR_RN_RM_H_MODE,          //[<Rn>, #+/-<rm>]!
    LDSR_RN_OFFSET_V_MODE,      //[<Rn>],#+/-<offset_8>
    LDSR_RN_RM_V_MODE,          //[<Rn>],#+/-<rm>

    //load and store multiply instruction
    LASM_IA_MODE,                //increment after
    LASM_IB_MODE,                //increment before
    LASM_DA_MODE,                //decrease after
    LASM_DB_MODE,                 //decrease before

    //load and store coprocessor mode
    LASC_RN_OFFSET84_MODE,      //[<Rn>, #+/-<offset_8>*4]
    LASC_RN_OFFSET84_H_MODE,    //[<Rn>, #+/-<offset_8>*4]!
    LASC_RN_OFFSET84_V_MODE,    //[<Rn>], #+/-<offset_8>*4
    LASC_RN_OPTION_MODE,        //[<Rn>, <option>

    MAX_MODE
};

enum  InstructConditionType {
    COND_MIN = -1,
    COND_EQ,        //equal
    COND_NE,        //not equal
    COND_CS,        //carry seted/unsined higer or same
    COND_CC,        //carry clear/unsigned lower 
    COND_MI,        //minus or negative
    COND_PL,        //plus or positional or zero
    COND_VS,        //overfollow
    COND_VC,        //not overfollow
    COND_HI,        //unsigned higer
    COND_LS,        //unsined lower or same
    COND_GE,        //singed greater or equal
    COND_LT,        //signed less than
    COND_GT,        //signed greater than
    COND_LE,        //signed less equal
    COND_Always,    //no confitional
    COND_NV,
    COND_MAX
};

#define MSR_FIELD_C 0x01
#define MSR_FIELD_X 0x02
#define MSR_FIELD_S 0x04
#define MSR_FIELD_F 0x08


class  InstructDecoder;

//template <class InstructDecoder = InstructDecoder>
struct  Instruct {
    u32 value;
    InstructType type;
    u16 opcode;          //opcode  
    u8 condit;      //condition flag
  
   union { 
       //data process innstruct
        struct tagDataProcessInfo {
            u8 rn;
            u8 rd;
            u8 rs;
            u8 rm;
            u8 shift_imm;
            u8 shift;
            u8 opcode;
            u8 flagI;
            u8 flagS;
            u8 rotate_imm;
            u8 immed_8;
            u32 shifter_operand;
            u32 immediate;
            u8 shifterCarryOut;
            u16 mode;
        }dpi;

        //msicellaneous load and store insruct
        struct tagMsicLoadStoreInfor {
            u8 flagI;
            u8 flagP;
            u8 flagU;
            u8 flagB;
            u8 flagW;
            u8 flagL;
            u8 flagS;
            u8 flagH;
            u8 rn;
            u8 rd;
            u8 rm;
            u8 shift_imm;
            u8 shift;
            u16 offset;
            u16 mode;
            u8 immedH;
            u8 immedL;            
            u32 address;
        }mls;

        //load and store mulitply instruction 
        struct tagLoadStoreMultiInfo {
            u8 flagP;
            u8 flagU;
            u8 flagS;
            u8 flagW;
            u8 flagL;
            u8 rn;
            u16 reglist;
            u32 startAddr;
            u32 endAddr;
            u16 mode;
        }lsm;

        //multiply instruction inforation
        struct tagMultiExtraLoadInfo {
            u8 rd;
            u8 rn;
            u8 rs;
            u8 rm;
            u8 rdhi;
            u8 rdlo;
            u8 hiOffset;
            u8 loOffset;
            u8 flagS;
            u8 flagU;
            u8 flagP;
            u8 flagW;
            u8 flagL;
            u8 flagH;
        }mpl;

        struct tagMSRInfo {
            u8 filedMask;
            u8 rotateImm;
            s8 immedia;
            u8 flagR;
            u8 rm;
            u8 rd;
            u8 optype;
        }msr;

        struct tagCoprocessorInfo {
            u8 flagP;
            u8 flagU;
            u8 flagN;
            u8 flagW;
            u8 flagL;
            u8 rn;
            u8 crd;
            u8 crn;
            u8 crm;
            u8 cp;
            s8 offset;
            u8 option;
            s16 opcode1;
            s16 opcode2;
            u8 cp_num;
            u8 coproc;
            u16 mode;
        }cop;


        struct BranchInfo {
            u8 flagL;
            u8 flagH;
            u8 rm;
            s32 signed_immed_24;
            s32 targetAddress;
        }branch;

        struct SWIInfo {
            u32 swiNbr;       //swi interrupt number
            u32 targetAddress;
            s32 imed24;
            u8  rn;
            u8  rd;
            u8  rm;
        }swi;
        
        struct MsicInfo {
            u8 rd;
            u8 rm;
            u8 rn;
        }msic;
    } info;
};


class InstructDecoder {
public:
    static InstructDecoder* instance();
    void decode(Instruct &inst);
private:
    void decode_DataProcessImedShift(u8 *buf, u8 size, Instruct &inst);
    void decode_Misc(u8 *buf, u8 size, Instruct *inst);
    void decode_DataProcessRegShift(u8 *buf, u8 size, Instruct &instt);
    void decode_MultiExtraLoad(u8 *buf, u8 size, Instruct &inst);
    void decode_DataProcessImed(u8 *buf, u8 size, Instruct &inst);
    void decode_Undefined(u8 *buf, u8 size, Instruct *inst);
    void decode_MoveImedToStatusReg(u8 *buf, u8 size, Instruct &instt);
    void decode_LoadStore(u8 *buf, u8 size, Instruct &inst);
    void decode_LoadStoreMulti(u8 *buf, u8 size, Instruct &inst);
    void decode_BranchLink(u8 *buf, u8 size, Instruct *inst);
    void decode_CoprocessorLoadStore(u8 *buf, u8 size, Instruct &instt);
    void decode_CoprocessorDataProcess(u8 *buf, u8 size, Instruct &instt);
    void decode_CoprocessorRegTransfer(u8 *buf, u8 size, Instruct &inst);
    void decode_SWI(u8 *buf, u8 size, Instruct &inst);
    void getAddressMode(u8*buf, u8 size, u16 &mode, Instruct &inst);
    InstructDecoder(){};
    ~InstructDecoder(){};
};

#endif
