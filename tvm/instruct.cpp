/// tvm
/// virtual machine for toyable language

#include "instruct.h"

/// @brief instruct decoder entry method
void InstructDecoder::decode(Instruct &inst) {
    u8 *buf = (u8*)&inst.value;
    u8 size = 32;
    u8 kind = bitsUnpack2Byte(buf,25, 3);
    inst._condit = bitsUnpack2Byte(buf, 28, 4);

    if ( (inst._condit == 0x0f) && (kind != 0x05) ) {
        return EInvalidInstruct;
    }
    switch (kind) {
        case 0x00:
            {
                u8 tmp1 = bitsUnpack2Byte(buf, 23, 2);
                u8 tmp2 = bitsUnpack2Byte(buf, 20, 1);
                u8 tmp3 = bitsUnpack2Byte(buf, 7, 1);
                u8 tmp4 = bitsUnpack2Byte(buf, 4, 1);

                if (( tmp1 == 0x02) && (tmp2 == 0x00)){
                    ret = decode_Misc(buf, size, inst);
                }
                else if ((tmp3 == 0x00) && (tmp4 == 0x01)){
                    ret = decode_DataProcessRegShift(buf, size, inst);
                }
                else  if ((tmp4 == 0x01) && (tmp4 == 0x01)) {
                    ret = decode_MultiExtraLoad(buf, size, inst);
                }
                else if (tmp4 == 0x00){
                    ret = decode_DataProcessImedShift(buf, size, inst);
                }
                else 
                    ret  = decode_Undefined(buf, size, inst);
                break;
            }
        case 0x01:
            {
                u8 tmp1 = bitsUnpack2Byte(buf, 23, 5);
                u8 tmp2 = bitsUnpack2Byte(buf, 20, 2);

                if ( ( tmp2 == 0x06) && (tmp2 == 0)) {
                    ret = decode_Undefined(buf, size, inst);
                }
                else if ( (tmp1 == 0x0E) && (tmp2 == 2)) {
                    ret = decode_MoveImedToStatusReg(buf, size, inst);
                }
                else { 
                    ret = decode_DataProcessImed(buf, size, inst);
                }
                break;
            }
        case 0x02:
            ret = decode_LoadStore(buf, size, inst);
            break;
        case 0x03:
            {
                u8 tmp1 = bitsUnpack2Byte(buf, 4, 1);
                if ( (tmp1 == 0x1)){
                    ret = decode_Undefined(buf, size, inst);
                }
                else {
                    ret = decode_LoadStore(buf, size, inst);
                }
                break;
            }
        case 0x04:
            ret = decode_LoadStoreMulti(buf, size, inst);
            break;
        case 0x05:
            ret = decode_BranchLink(buf, size, inst);
            break;
        case 0x06:
            ret = decode_CoprocessorLoadStore(buf, size, inst);
            break;
        case 0x07:
            {
                u8 tmp1 = bitsUnpack2Byte(buf, 24, 4);
                u8 tmp2 = bitsUnpack2Byte(buf, 4, 1);

                if ( ( tmp2 == 0x0E) && (tmp2 == 0)) {
                    ret = decode_CoprocessorDataProcess(buf, size, inst);
                }
                else if ( (tmp1 == 0x0E) && (tmp2 == 1)){
                    ret = decode_CoprocessorRegTransfer(buf, size, inst);
                }
                else if ( tmp1 == 0x0F){
                    ret = decode_SWI(buf, size, inst);
                }
                else {
                    ret = decode_Undefined(buf, size, inst);
                }
            break;
            }
        default:
            ret = decode_Undefined(buf, size, inst);
            break;

     };

    return ret;
 }

void InstructDecoder::decode_DataProcessImedShift(u8 *buf, u8 size, Instruct &inst) {
    inst.info.dpi.opcode = bitsUnpack2Byte(buf, 21, 4);
    inst.info.dpi.flagI = bitsUnpack2Byte(buf, 25, 1);
    inst.info.dpi.flagS = bitsUnpack2Byte(buf, 20, 1);
    inst.info.dpi.rn = bitsUnpack2Byte(buf, 16, 4);
    inst.info.dpi.rd = bitsUnpack2Byte(buf, 12, 4);
    inst.info.dpi.shift_imm = bitsUnpack2Byte(buf, 5, 2);
    inst.info.dpi.rm = bitsUnpack2Byte(buf, 0, 4);
    inst.type = static_cast<InstructType>(inst.info.dpi.opcode + INST_AND);
    
    getAddressMode(buf, size, inst.info.dpi.mode, inst);
 }

void InstructDecoder::decode_Misc(u8 *buf, u8 size, Instruct &inst) {
    void ret = ENoError;
    u8 tmp1 = bitsUnpack2Byte(buf, 20, 3);
    u8 tmp2 = bitsUnpack2Byte(buf,  4, 4);

    if (( tmp1 == 0x06) && (tmp2 == 0x01)) {
        inst.type = INST_CLZ;
        inst.info.msic.rd = bitsUnpack2Byte(buf, 12, 4);
        inst.info.msic.rm = bitsUnpack2Byte(buf, 0, 4);
    }
    else if ((tmp1 == 0x02) && (tmp2 == 0x01))
        ret = decode_BranchLink(buf, size, inst);
    else if ((tmp1 == 0x02) && (tmp2 == 0x03))
        ret = decode_BranchLink(buf, size, inst);
    else if ((tmp1 == 0x02) && (tmp2 == 0x07))
        ret = decode_SWI(buf, size, inst);
    else 
       // throw exception
}

void InstructDecoder::decode_DataProcessRegShift(u8 *buf, u8 size, Instruct &inst) {
    inst.info.dpi.opcode = bitsUnpack2Byte(buf, 21, 4);
    inst.info.dpi.flagI = 0;
    inst.info.dpi.flagS = bitsUnpack2Byte(buf, 20, 1);
    inst.info.dpi.rn = bitsUnpack2Byte(buf, 16, 4);
    inst.info.dpi.rd = bitsUnpack2Byte(buf, 12, 4);
    inst.info.dpi.rs = bitsUnpack2Byte(buf, 8, 4);
    inst.info.dpi.shift_imm = bitsUnpack2Byte(buf, 5, 2);
    inst.info.dpi.rm = bitsUnpack2Byte(buf, 0, 4);
           
    inst.type = static_cast<InstructType>(inst.info.dpi.opcode + INST_AND);

    getAddressMode(buf, size, inst.info.dpi.mode, inst);
       // throw exception
}


void InstructDecoder::decode_MultiExtraLoad(u8 *buf, u8 size, Instruct &inst) {
    u8 tmp1 = bitsUnpack2Byte(buf, 21, 4);
    u8 tmp2 = bitsUnpack2Byte(buf,  4, 4);

    if ( (tmp1 == 0x00) && (tmp2 == 0x09)) {
        inst.type = INST_MUL;
        if (bitsUnpack2Byte(buf, 12, 4) != 0x00) {
                   // throw exception
        }
        inst.info.mpl.flagS = bitsUnpack2Byte(buf,  20, 1);
        inst.info.mpl.rd = bitsUnpack2Byte(buf, 16, 4);
        inst.info.mpl.rs = bitsUnpack2Byte(buf,  8, 4);
        inst.info.mpl.rm = bitsUnpack2Byte(buf,  0, 4);
    }
    else if ( (tmp1 == 0x01) && (tmp2 == 0x09)) {
        inst.type = INST_MLA;
        if (bitsUnpack2Byte(buf, 4, 4) != 0x00) {
                  // throw exception
        }
        inst.info.mpl.flagS = bitsUnpack2Byte(buf,  20, 1);
        inst.info.mpl.rd = bitsUnpack2Byte(buf, 16, 4);
        inst.info.mpl.rn = bitsUnpack2Byte(buf, 12, 4);
        inst.info.mpl.rs = bitsUnpack2Byte(buf,  8, 4);
        inst.info.mpl.rm = bitsUnpack2Byte(buf,  0, 4);
    }
    else if ((tmp1 == 0x01) && (tmp2 == 0x09)) {
        inst.type = INST_SMULL;
        inst.info.mpl.flagS = bitsUnpack2Byte(buf, 20, 1);
        inst.info.mpl.rdhi = bitsUnpack2Byte(buf, 16, 4);
        inst.info.mpl.rdlo = bitsUnpack2Byte(buf, 12, 4);
        inst.info.mpl.rs = bitsUnpack2Byte(buf,  8, 4);
        inst.info.mpl.rm = bitsUnpack2Byte(buf,  0, 4);
    }
    else if ((tmp1 == 0x06) && (tmp2 == 0x09)) {
        inst.type = INST_SMULL;
        inst.info.mpl.flagS = bitsUnpack2Byte(buf, 20, 1);
        inst.info.mpl.rdhi = bitsUnpack2Byte(buf, 16, 4);
        inst.info.mpl.rdlo = bitsUnpack2Byte(buf, 12, 4);
        inst.info.mpl.rs = bitsUnpack2Byte(buf,  8, 4);
        inst.info.mpl.rm = bitsUnpack2Byte(buf,  0, 4);
    }
    else if ((tmp1 == 0x07) && (tmp2 == 0x09)) {
        inst.type = INST_SMLAL;
        inst.info.mpl.flagS = bitsUnpack2Byte(buf, 20, 1);
        inst.info.mpl.rdhi = bitsUnpack2Byte(buf, 16, 4);
        inst.info.mpl.rdlo = bitsUnpack2Byte(buf, 12, 4);
        inst.info.mpl.rs = bitsUnpack2Byte(buf, 8, 4);
        inst.info.mpl.rm = bitsUnpack2Byte(buf, 0, 4);
    }
    else if ((tmp1 == 0x04) && (tmp2 == 0x09)) {
        inst.type = INST_UMULL;
        inst.info.mpl.flagS = bitsUnpack2Byte(buf, 20, 1);
        inst.info.mpl.rdhi = bitsUnpack2Byte(buf, 16, 4);
        inst.info.mpl.rdlo = bitsUnpack2Byte(buf, 12, 4);
        inst.info.mpl.rs = bitsUnpack2Byte(buf,  8, 4);
        inst.info.mpl.rm = bitsUnpack2Byte(buf,  0, 4);
    }
    else if ((tmp1 == 0x05) && (tmp2 == 0x09)) {
        inst.type = INST_UMLAL;
        inst.info.mpl.flagS = bitsUnpack2Byte(buf, 20, 1);
        inst.info.mpl.rdhi = bitsUnpack2Byte(buf, 16, 4);
        inst.info.mpl.rdlo = bitsUnpack2Byte(buf, 12, 4);
        inst.info.mpl.rs = bitsUnpack2Byte(buf,  8, 4);
        inst.info.mpl.rm = bitsUnpack2Byte(buf,  0, 4);
    }
  
    // throw exception
}

void InstructDecoder::decode_DataProcessImed(u8 *buf, u8 size, Instruct &inst) {
    inst.info.dpi.opcode = bitsUnpack2Byte(buf, 21, 4);
    inst.info.dpi.flagI = 1;
    inst.info.dpi.flagS = bitsUnpack2Byte(buf, 20, 1);
    inst.info.dpi.rn = bitsUnpack2Byte(buf, 16, 4);
    inst.info.dpi.rd = bitsUnpack2Byte(buf, 12, 4);
    inst.info.dpi.rotate_imm = bitsUnpack2Byte(buf, 8, 4);
    inst.info.dpi.immed_8 = bitsUnpack2Byte(buf, 0, 8);    
    inst.type = static_cast<InstructType>(inst.info.dpi.opcode + INST_AND);

    getAddressMode(buf, size, inst.info.dpi.mode, inst);
        
    // throw exception
}

void InstructDecoder::decode_Undefined(u8 *buf, u8 size, Instruct &inst) {
       // throw exception
}

void InstructDecoder::decode_MoveImedToStatusReg(u8 *buf, u8 size, Instruct &inst) {
    if (bitsUnpack2Byte(buf, 20, 2) == 1) {
        inst.type = INST_MRS;
        inst.info.msr.flagR = bitsUnpack2Byte(buf, 22, 1);    
        inst.info.msr.rd = bitsUnpack2Byte(buf, 12, 4);
    }
    else {
        inst.type = INST_MSR;
        if (bitsUnpack2Byte(buf, 25, 1) == 1)
        {
            inst.info.msr.flagR = bitsUnpack2Byte(buf, 22, 1);
            inst.info.msr.filedMask = bitsUnpack2Byte(buf, 16, 4);
            inst.info.msr.rm = bitsUnpack2Byte(buf, 0, 4);
        }
        else 
        {
            inst.info.msr.flagR = bitsUnpack2Byte(buf, 22, 1);
            inst.info.msr.filedMask = bitsUnpack2Byte(buf, 16, 4);
            inst.info.msr.rotateImm = bitsUnpack2Byte(buf, 8, 4);
            inst.info.msr.immedia = bitsUnpack2Byte(buf, 0, 8);
        }
    }
}


void InstructDecoder::decode_LoadStore(u8 *buf, u8 size, Instruct &inst) {
    //load/store word or unsigned byte instructions
    if (bitsUnpack2Byte(buf, 26, 2) == 0x01) {
        inst.info.mls.flagI = bitsUnpack2Byte(buf, 25, 1);
        inst.info.mls.flagP = bitsUnpack2Byte(buf, 24, 1);
        inst.info.mls.flagU = bitsUnpack2Byte(buf, 23, 1);
        inst.info.mls.flagB = bitsUnpack2Byte(buf, 22, 1);
        inst.info.mls.flagW = bitsUnpack2Byte(buf, 21, 1);
        inst.info.mls.flagL = bitsUnpack2Byte(buf, 20, 1);
        inst.info.mls.rn = bitsUnpack2Byte(buf, 16, 4);
        inst.info.mls.rd = bitsUnpack2Byte(buf, 12, 4);
        inst.info.mls.mode = bitsUnpack2Byte(buf, 0, 12);

        if (inst.info.mls.flagI == 0) {
            inst.info.mls.offset = bitsUnpack2Byte(buf, 0, 12);
        }
        else {
            inst.info.mls.rm = bitsUnpack2Byte(buf, 0, 4);
            inst.info.mls.shift_imm = bitsUnpack2Byte(buf, 7, 5);
            inst.info.mls.shift = bitsUnpack2Byte(buf, 5, 2);
        }
        //load instruction
        if (inst.info.mls.flagL == 0x01) {
            if (inst.info.mls.flagP == 0)
            {
                if ((inst.info.mls.flagB == 1) &&(inst.info.mls.flagW == 1)) {
                    inst.type = INST_LDRBT;
                }
                else if ((inst.info.mls.flagB == 0) && (inst.info.mls.flagW == 1)){
                    inst.type = INST_LDRT;
                }
            }
            else {
                if (inst.info.mls.flagB == 1){
                    inst.type = INST_LDRB;
                }
                else { 
                    inst.type = INST_LDR;
                }
            }
        }
        //store instrcution
        else {
            //byte
            if (inst.info.mls.flagB == 0x01) {
                if ((inst.info.mls.flagW = 0x01) && (inst.info.mls.flagP = 0x00)){
                    inst.type = INST_STRBT;    
                }                
                else {
                    inst.type = INST_STRB;
                }
            }
            else {
                inst.type = INST_STRB;
            }
        }    
    
    }
    //load/store half word or singed byte instructions
    else {
        inst.info.mls.flagP = bitsUnpack2Byte(buf, 24, 1);
        inst.info.mls.flagU = bitsUnpack2Byte(buf, 23, 1);
        inst.info.mls.flagI = bitsUnpack2Byte(buf, 22, 1);
        inst.info.mls.flagW = bitsUnpack2Byte(buf, 21, 1);
        inst.info.mls.flagL = bitsUnpack2Byte(buf, 20, 1);
        inst.info.mls.rn = bitsUnpack2Byte(buf, 16, 4);
        inst.info.mls.rd = bitsUnpack2Byte(buf, 12, 4);
        inst.info.mls.mode = bitsUnpack2Byte(buf, 8, 4);
        inst.info.mls.flagS = bitsUnpack2Byte(buf, 6, 1);
        inst.info.mls.flagH = bitsUnpack2Byte(buf, 5, 1);

        if (inst.info.mls.flagI == 0) {
            inst.info.mls.offset = bitsUnpack2Byte(buf, 0, 12);
        }
        else {
            inst.info.mls.rm = bitsUnpack2Byte(buf, 0, 4);
            inst.info.mls.shift_imm = bitsUnpack2Byte(buf, 7, 5);
            inst.info.mls.shift = bitsUnpack2Byte(buf, 5, 2);
        }
        
        //load instructions
        if (inst.info.mls.flagL == 0x01) {
            //a signed byte
            if (inst.info.mls.flagH == 0 ) { 
                inst.type = INST_LDRSB;
            }
            //a signed half word
            else if (inst.info.mls.flagS == 1) {
                inst.type = INST_LDRSH;
            }
            else {
                inst.type = INST_Undefined;
            }
        }
        else {
            //a signed byte
            if (inst.info.mls.flagH == 0x01 ) { 
                inst.type = INST_STRH;
            }
            else {
                inst.type = INST_Undefined;
            }
        }

    }

    getAddressMode(buf, size, inst.info.mls.mode, inst);
}

void InstructDecoder::decode_LoadStoreMulti(u8 *buf, u8 size, Instruct &inst) {
    inst.info.lsm.flagP = bitsUnpack2Byte(buf, 24, 1);
    inst.info.lsm.flagU = bitsUnpack2Byte(buf, 23, 1);
    inst.info.lsm.flagS = bitsUnpack2Byte(buf, 22, 1);
    inst.info.lsm.flagW = bitsUnpack2Byte(buf, 21, 1);
    inst.info.lsm.flagL = bitsUnpack2Byte(buf, 20, 1);
    inst.info.lsm.rn = bitsUnpack2Byte(buf, 16, 4);
    inst.info.lsm.reglist = bitsUnpack2Word(buf, 0, 16);

    //wether it is stm or ldm
    if ( inst.info.lsm.flagL == 1) {
        inst.type = INST_LDM;
    }
    else {
        inst.type = INST_STM;
    }
    
    //determin the address mode 
    if ((inst.info.lsm.flagP == 0 ) && (inst.info.lsm.flagU == 1))  {
        inst.info.lsm.mode = LASM_IA_MODE;
    }
    else if ((inst.info.lsm.flagP == 1 ) && (inst.info.lsm.flagU == 1)) {
        inst.info.lsm.mode = LASM_IB_MODE;
    }
    else if ((inst.info.lsm.flagP == 0 ) && (inst.info.lsm.flagU == 0))  {
        inst.info.lsm.mode = LASM_DA_MODE;
    }
    else if ((inst.info.lsm.flagP == 1 ) && (inst.info.lsm.flagU == 0)) {
        inst.info.lsm.mode = LASM_DB_MODE;
    }
}

void InstructDecoder::decode_BranchLink(u8 *buf, u8 size, Instruct &inst) {
    u8 opcode = bitsUnpack2Byte(buf, 25, 3);

    if ( (inst._condit == 0xF) && (opcode == 0x05)) {
        inst.info.branch.flagH = bitsUnpack2Byte(buf, 25, 1);
        inst.type = INST_BLX1;
        inst.info.branch.flagL = bitsUnpack2Byte(buf, 24, 1);
        inst.info.branch.targetAddress = bitsUnpack2Byte(buf, 0, 24);
    }

    else if (opcode == 0x05) {
        inst.type = INST_BL;
        inst.info.branch.flagL = bitsUnpack2Byte(buf, 24, 1);
        inst.info.branch.targetAddress = bitsUnpack2Byte(buf, 0, 24);
    }

    else if ((opcode == 0x00) &&
        (bitsUnpack2Byte(buf, 20, 5) == 0x12) &&
        (bitsUnpack2Byte(buf, 4, 4) == 0x01)) {
        inst.type = INST_BX;
        inst.info.branch.rm = bitsUnpack2Byte(buf, 0, 4);
    }
    else if ((opcode == 0) &&  
        (bitsUnpack2Byte(buf, 20, 5) == 0x12) && 
        (bitsUnpack2Byte(buf, 4, 4) == 0x03)) {
        inst.type = INST_BLX2;
        inst.info.branch.rm = bitsUnpack2Byte(buf, 0, 4);
    }
    else 
              // throw exception
  }

void InstructDecoder::decode_CoprocessorLoadStore(u8 *buf, u8 size, Instruct &inst) {
    inst.info.cop.flagP = bitsUnpack2Byte(buf, 24, 1);
    inst.info.cop.flagU = bitsUnpack2Byte(buf, 23, 1);
    inst.info.cop.flagN = bitsUnpack2Byte(buf, 22, 1);
    inst.info.cop.flagW = bitsUnpack2Byte(buf, 21, 1);
    inst.info.cop.rn    = bitsUnpack2Byte(buf, 16, 4);
    inst.info.cop.crd   = bitsUnpack2Byte(buf, 12, 4);
    inst.info.cop.cp_num = bitsUnpack2Byte(buf, 8, 4);
    inst.info.cop.offset = bitsUnpack2Byte(buf, 0, 8);

    if (bitsUnpack2Byte(buf, 22, 1) == 1) {
        inst.type = INST_LDC;
    }
    else {
        inst.type = INST_STC;
    }  
  }

void InstructDecoder::decode_CoprocessorDataProcess(u8 *buf, u8 size, Instruct &inst) {
    inst.info.cop.opcode1 = bitsUnpack2Byte(buf, 20, 4);
    inst.info.cop.crn = bitsUnpack2Byte(buf, 16, 4);
    inst.info.cop.crd = bitsUnpack2Byte(buf, 12, 4);
    inst.info.cop.cp_num = bitsUnpack2Byte(buf, 8, 4);
    inst.info.cop.opcode2 = bitsUnpack2Byte(buf, 5, 3);
    inst.info.cop.crm = bitsUnpack2Byte(buf, 0, 4);
    inst.type = INST_CDP;
}

void InstructDecoder::decode_CoprocessorRegTransfer(u8 *buf, u8 size, Instruct &inst) {
    void ret = ENoError;
    inst.info.cop.opcode1 = bitsUnpack2Byte(buf, 21, 4);
    inst.info.cop.crn = bitsUnpack2Byte(buf, 16, 4);
    inst.info.cop.crd = bitsUnpack2Byte(buf, 12, 4);
    inst.info.cop.cp_num = bitsUnpack2Byte(buf, 8, 4);
    inst.info.cop.opcode2 = bitsUnpack2Byte(buf, 5, 3);
    inst.info.cop.crm = bitsUnpack2Byte(buf, 0, 4);

    if (bitsUnpack2Byte(buf, 20, 1) == 1) {
        inst.type = INST_MRC;
    }
    else {
        inst.type = INST_MCR;
    }
}

void InstructDecoder::decode_SWI(u8 *buf, u8 size, Instruct &inst) {
    u8 tmpval = bitsUnpack2Byte(buf, 24, 4);
    if (tmpval == 0xf) {
        inst.type = INST_SWI;
        inst.info.swi.imed24 = bitsUnpack2Byte(buf, 0, 24);
    }
    else {
        tmpval = bitsUnpack2Byte(buf, 20, 4);
        if (tmpval == 0x00) {
            inst.type = INST_SWP;
            inst.info.swi.rn = bitsUnpack2Byte(buf, 16, 4);
            inst.info.swi.rd = bitsUnpack2Byte(buf, 12, 4);
            inst.info.swi.rm = bitsUnpack2Byte(buf, 0, 4);
        }
        else if (tmpval == 0x04) {
            inst.type = INST_SWPB;
            inst.info.swi.rn = bitsUnpack2Byte(buf, 16, 4);
            inst.info.swi.rd = bitsUnpack2Byte(buf, 12, 4);
            inst.info.swi.rm = bitsUnpack2Byte(buf, 0, 4);
        }
        else 
                // throw exception

    }

    // throw exception
}

void InstructDecoder::getAddressMode(u8*buf, u8 size, u16 &mode, Instruct &inst) {
    //only for data process type instruction
    //no optimize
    if ((inst.type <= INST_MNV) && (inst.type >= INST_AND)) {
        if ((bitsUnpack2Byte(buf, 25, 3) == 0x01))
            mode = DPI_IMMEDIATE_MODE;
        else if (bitsUnpack2Byte(buf, 4, 8) == 0x00) 
            mode = DPI_RM_MODE;
        else if (bitsUnpack2Byte(buf, 4, 3) == 0x00)
            mode = DPI_RM_LSL_SHIFT_MODE;
        else if (bitsUnpack2Byte(buf, 4, 4) == 0x01)
            mode = DPI_RM_LSL_RS_MODE;
        else if (bitsUnpack2Byte(buf, 4, 3) == 0x02)
            mode = DPI_RM_LSR_SHIFT_MODE;
        else if (bitsUnpack2Byte(buf, 4, 4) == 0x03)
            mode = DPI_RM_LSR_RS_MODE;
        else if (bitsUnpack2Byte(buf, 4, 4) == 0x04)
            mode = DPI_RM_ASR_SHIFT_MODE;
        else if (bitsUnpack2Byte(buf, 4, 4) == 0x05)
            mode = DPI_RM_ASR_RS_MODE;
        else if (bitsUnpack2Byte(buf, 4, 3) == 0x06)
            mode = DPI_RM_ROR_SHIFT_MODE;
        else if (bitsUnpack2Byte(buf, 4, 4) == 0x07)
            mode = DPI_RM_ROR_RS_MODE;
        else if (bitsUnpack2Byte(buf, 4, 8) == 0x06)
            mode = DPI_RM_RRX_MODE;
        else 
            mode = MIN_MODE;
     }// end for data process instruction

    else if ((inst.type <= INST_STRT) && (inst.type >= INST_LDR)) {

        if ( (inst.info.mls.flagI == 0) &&(inst.info.mls.flagP == 1) &&  (inst.info.mls.flagW == 0)) {
            mode = LDSR_RN_OFFSET12_WB_MODE;
        }
        else if ( (inst.info.mls.flagI == 1) && (inst.info.mls.flagP == 1) && (inst.info.mls.flagW == 0)) {
            if (bitsUnpack2Byte(buf, 4, 8) == 0x00) {
                mode = LDSR_RN_RM_WB_MODE;
            }
            else if (bitsUnpack2Byte(buf, 4, 1) == 0x00) {
                mode = LDSR_RN_RM_SHIFT_SHIFTM_WB_MODE;
            }
            else 
                      // throw exception

        }
        else if ( (inst.info.mls.flagI == 0) && (inst.info.mls.flagP == 1) && (inst.info.mls.flagW == 1)) {
            mode = LDSR_RN_OFFSET12_H_WB_MODE;

        }
        else if ( (inst.info.mls.flagI == 1) &&
             (inst.info.mls.flagP == 1) &&
             (inst.info.mls.flagW == 1))
        {
            if (bitsUnpack2Byte(buf, 4, 8) == 0x00) {
                mode = LDSR_RN_RM_H_WB_MODE;
            }
            else if (bitsUnpack2Byte(buf, 4, 1) == 0x00) {
                mode = LDSR_RN_RM_SHIFT_SHIFTM_H_WB_MODE;
            }
            else 
                      // throw exception
        }
        else if ( (inst.info.mls.flagI == 0) && (inst.info.mls.flagP == 0) && (inst.info.mls.flagW == 0)) {
            mode = LDSR_RN_OFFSET12_V_WB_MODE;

        }
        else if ( (inst.info.mls.flagI == 1) && (inst.info.mls.flagP == 0) && (inst.info.mls.flagW == 0)) {
            if (bitsUnpack2Byte(buf, 4, 8) == 0x00) {
                mode = LDSR_RN_RM_V_WB_MODE;
            }
            else if (bitsUnpack2Byte(buf, 4, 1) == 0x00) {
                mode = LDSR_RN_RM_SHIFT_V_WB_MODE;
            }
            else 
                     // throw exception
        }  
        else
                       // throw exception

    }// end for ldr/str instruction
}

