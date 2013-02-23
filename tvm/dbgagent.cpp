/// tvm
/// virtual machine for toyable language

#include "dbgagent.h"

static void dbgAgentExceptionHandler(u32 type, u32 address, u32 reason, u32 inst, u32 param);

s32  DbgAgebt::open(u32 mode)
{
    m_mode = mode;
    switch (mode)
    {
    case eDbgModeRDI:        
        {
            ///if the protocol is RDI, socket must be initialized.
               WSADATA wsaData;
	        WORD wVersionRequested = MAKEWORD(1, 1);
	        int nResult = WSAStartup(wVersionRequested, &wsaData);
	        if (nResult != 0)
		        return 1;
        }
        break;

    default:
        return eDbgModeNull;
        break;
    };
    return 1;
}
///close the debug interface
void DbgAgebt::close()
{
    switch (m_mode)
    {
    case eDbgModeRDI:        
        {
            ///if the protocol is RDI, socket must be initialized.
            WSACleanup();
        }
        break;
    default:
        break;
    };
    m_mode = DbgAgentInterface::eDbgModeNull;

}
s32 DbgAgebt::execute()
{
    if (m_mode == DbgAgentInterface::eDbgModeRDI)
        return executeRDI();

    else if (m_mode == DbgAgentInterface::eDbgModeMDI)
        return executeMDI();

    else if (m_mode == DbgAgentInterface::eDbgModeUDI)
        return executeUDI();
    else
        return -1;

    return 0;
}

void DbgAgebt::pause(void *arg)
{
}

s32 DbgAgebt::executeRDI()
{
    //create the dbg connection
    struct sockaddr_in serverAddr;
    struct sockaddr_in cliAddr;
    SOCKET listenHandle;
    int i;
    u32 ret;

    /// Open a socket
    listenHandle = socket (AF_INET, SOCK_STREAM, 0);
    if (listenHandle < 0)
    {
        return EXIT_FAILURE;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(VXDBG_SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenHandle, (const sockaddr *)&serverAddr, sizeof (serverAddr)) < 0)
    {
        closesocket(listenHandle);
        return EXIT_FAILURE;
    }
    if (listen(listenHandle, 0) < 0)
    {
        closesocket(listenHandle);
        return EXIT_FAILURE;
    }

    i = sizeof(cliAddr);
    _dbgSocket = accept(listenHandle, (sockaddr *)&cliAddr, &i);
    if (_dbgSocket < 0)
    {
        closesocket(listenHandle);
        return EXIT_FAILURE;
    }

    ///read dbg instruction from the connection
    struct fd_set readfds;
    u8 msgchar;
    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(_dbgSocket, &readfds);
        
        //if (select (1, &readfds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0) < 0)
        //    continue;
        if (remoteReadChar(msgchar) < 0)
            continue;

        switch (msgchar)
        {
        case RDP_Start:
            m_engine->reset(0, true);
            m_engine->enableDebugMode();
            m_engine->registerExceptionHandler(VxDbgAnent_ExceptionHandler, (u32)this);
            remoteWriteChar(RDP_Return);
            break;
        case RDP_End:
            m_engine->disableDebugMode();
            m_engine->unregisetExceptionHandle();
            remoteWriteChar(RDP_Return);
            break;
        case RDP_Read:
            {
                u32 address;
                u32 size;
                remoteReadWord(address);
                remoteReadWord(size);
                u8 *ptr = new u8[size];
                //read data  from target
                ret = readMemory(address, ptr, size);
                // write data to remote debugger
                remoteWriteChar(RDP_Return);
                remoteWriteChar((u8)ret);
                remoteWriteWord(size); 
                for (u32 bytes = 0; bytes < size; bytes ++) {
                    if (remoteWriteChar(*(ptr + bytes)) < 0)
                        break;
                }
                delete[] ptr;
                break;
            }
        case RDP_Write:
            {
                u32 address;
                u32 size;
                u32 idx;
                remoteReadWord(address);
                remoteReadWord(size);
                u8 *ptr = new u8[size];
                //read data  from target
                for (idx = 0; idx < size; idx ++)  {
                   if (remoteReadChar(*(ptr + idx)) < 0)
                   {
                       ret = RDIError_Error;
                       break;;
                   }
                }
                ret = writeMemory(address, ptr, size);
                //send response
                delete[] ptr;
                remoteWriteChar(RDP_Return);
                remoteWriteChar((u8)ret);
                remoteWriteWord(idx);                
                break;
            }
        case RDP_CPUread:
            {
                u8 mode;
                u32 mask;
                remoteReadChar(mode);
                remoteReadWord(mask);
                u8 * ptr = new u8[sizeof(u32)*RDINumCPURegs];
                ret = readCpuState(mode, mask, ptr, sizeof(u32)*RDINumCPURegs);
                remoteWriteChar(RDP_Return);
                //write these register value to peer

                u32 k, j;
                for (k = 1, j = 0; k != 0x80000000; k*=2) {
                    if (k & mask) {
                        remoteWriteWord(*((u32 *)ptr + j));
                        j ++;                    
                    }
                }
                delete[] ptr;
                remoteWriteChar((u8)j);
                remoteWriteChar((u8)ret);
                break;
            }
        case RDP_CPUwrite:
            {
                u8 mode;
                u32 mask;
                remoteReadChar(mode);
                remoteReadWord(mask);
                u8 * ptr = new u8[sizeof(u32)*RDINumCPURegs];
                //read these register value to peer
                u32 k, j;
                for (k = 1, j = 0; k != 0x80000000; k*=2) {
                    if (k & mask) {
                        remoteReadWord(*((u32 *)ptr + j));
                        j ++;                    
                    }
                }
                ret = writeCpuState(mode, mask, ptr, sizeof(u32)*RDINumCPURegs);
                remoteWriteChar(RDP_Return);
                delete[] ptr;
                remoteWriteChar((u8)ret);
                break;
            }
            break;
        case RDP_CPread:
            break;
        case RDP_CPwrite:
            break;
        case RDP_SetBreak:
            {
                u32 address;
                u32 bound = 0;
                u8  type;
                BreakptHandle handle;

                remoteReadWord(address);
                remoteReadChar(type);
                if ((type & 0x0f) >= 5) 
                    remoteReadWord(bound);

                ret = this->setBreakpt(address, type, (u16)bound, &handle);
                remoteWriteChar(RDP_Return);
                remoteWriteWord(handle);
                remoteWriteChar((u8)ret);
                break;
            }
        case RDP_ClearBreak:
            {
                BreakptHandle handle;
                remoteReadWord(handle);
                ret = this->delBreakpt(handle);
                remoteWriteChar(RDP_Return);
                remoteWriteChar((u8)ret);
                break;
            }
        case RDP_SetWatch:
            {
                u32 address;
                u32 bound;
                u8 type;
                u8 dataType;
                BreakptHandle handle;

                remoteReadWord(address);
                remoteReadChar(type);
                remoteReadChar(dataType);
                if ((type & 0xf) >= 5) 
                    remoteReadWord(bound);
                ret = this->setWatchpt(address, type, dataType, (u16)bound, &handle);

                remoteWriteChar(RDP_Return);
                remoteWriteWord(handle);
                remoteWriteChar((u8)ret);
                break;
            }
        case RDP_ClearWatch:
            {
                BreakptHandle handle;
                remoteReadWord(handle);
                ret = this->delWatchpt(handle);
                remoteWriteChar(RDP_Return);
                remoteWriteChar((u8)ret);
                break;
            }
        case RDP_Execute:
            {
                u8 result;
                remoteReadChar(result);
                BreakptHandle handle;
                ret = this->execute(&handle);
                remoteWriteChar(RDP_Return);
                if (result & 0x80)
                    remoteWriteWord(handle);
                remoteWriteChar((u8)ret);
                break;
            }
        case RDP_Step:
            {
                u8 result;
                u32 instnbr;
                BreakptHandle handle;
                remoteReadChar(result);
                remoteReadWord(instnbr);
                ret = this->step(instnbr, &handle);
                if (result & 0x80)
                    remoteWriteWord(handle);
                remoteWriteChar((u8)ret);
                break;
            }
        case RDP_Info:
            {
                u32 infotype;
                remoteReadWord(infotype);
                switch (infotype) 
                {
                case RDIInfo_Target:
                    break;
                case RDISet_RDILevel:
                    break;
                case RDISet_Cmdline:
                    break;
                case RDIInfo_Step:
                    break;
                case RDIVector_Catch:
                    break;
                case RDIInfo_Points:
                    break;
                default:
                    break;
                }
                break;
            }
        case RDP_OSOpReply:
            remoteWriteChar(RDP_Fatal);
            break;
        case RDP_Reset:

            break;
        default:
            break;

        }

    }
    closesocket(listenHandle);
    closesocket(_dbgSocket);
    return 0;
}
s32 DbgAgebt::executeMDI()
{
    return 0;
}
s32 DbgAgebt::executeUDI()
{
    return 0;
}


u32 DbgAgebt::readMemory(u32 address, u8*buffer, u32 size)
{
    u32 idx;
    u32 value;
    for (idx = 0; idx < size; idx ++)
    {
        if (m_engine->readByte(address, value) < 0)
            return -1;
        buffer[idx] = (u8)(value &0xff);
        address ++;
    }
    return idx; 
    
}
u32 DbgAgebt::writeMemory(u32 address, u8*buffer, u32 size)
{
    u32 idx;
    for (idx = 0; idx < size; idx ++)
    {
        if (m_engine->writeByte(address, buffer[idx]) < 0)
            return -1;
        address ++;
    }
    return idx; 
}

///read cpu state
u32 DbgAgebt::readCpuState(u32 mode, u32 mask, u8*buffer, u32 size)
{
    return 0;
}

u32 DbgAgebt::writeCpuState(u32 mode, u32 mask, u8*buffer, u32 size)
{
    return 0;
}

u32 DbgAgebt::readCopState(u8 cpnum, u32 mask, u8*buffer, u32 size)
{
    return 0;
}
u32 DbgAgebt::writeCopState(u8 cpnum, u32 mask, u8*buffer, u32 size)
{
    return 0;
}
u32 DbgAgebt::setBreakpt(u32 address, u8 mode, u16 bound, BreakptHandle *handle)
{
    BreakptNode *node = new BreakptNode();
    node->address = address;
    node->bound = bound;
    node->type = mode;
    node->handle = (BreakptHandle)node;
    m_engine->readWord(address, node->inst);
    m_engine->writeWord(address, 0xffffffffL);
    _breakptList.push_back(node);
    *handle =(BreakptHandle)node;
    return 0;
}
u32 DbgAgebt::delBreakpt(BreakptHandle handle)
{
    std::list<BreakptNode *>::iterator ite;
    for (ite = _breakptList.begin(); ite != _breakptList.end(); ite++)
    {
        BreakptNode * node = (BreakptNode*)*ite;
        if (node->handle == handle) 
        {
            _breakptList.erase(ite);
            delete node;
            return 0;
        }
    }
    return -1;
}
u32 DbgAgebt::setWatchpt(u32 address, u8 type, u8 dataType, u16 bound, WatchptHandle *handle)
{
    WatchptNode *node = new WatchptNode();
    node->address = address;
    node->datatype = dataType;
    node->type = type;
    node->handle = (WatchptHandle)node;
    _watchptList.push_back(node);
    *handle = (WatchptHandle)node;
    return 0;

}  
u32 DbgAgebt::delWatchpt(BreakptHandle handle)
{
    std::list<WatchptNode *>::iterator ite;
    for (ite = _watchptList.begin(); ite != _watchptList.end(); ite++)
    {
        WatchptNode * node = (WatchptNode *)(*ite);
        if (node->handle == handle) 
        {
            _watchptList.erase(ite);
            delete node;
            return 0;
        }
    }
    return -1;
}
u32 DbgAgebt::execute(BreakptHandle *handle)
{
    return 0;
}
u32 DbgAgebt::step(u32 ninst, BreakptHandle *handle)
{
    m_engine->executeInstrcution(ninst);
    return 0;
}
u32 DbgAgebt::info(u8 type, u16 *arg1, u16 *arg2)
{
    return 0;
}

s32 DbgAgebt::localPrint(void *arg, const  char *fmt, va_list ap)
{
    return 0;
}
s32 DbgAgebt::localReadChar(u8 &c)
{
    return 0;
}
s32 DbgAgebt::localWriteChar(u8 c)
{
    return 0;
}
s32 DbgAgebt::localReadWord(u32 &value)
{
    return 0;
}
s32 DbgAgebt::localWirteWord(u32 value)
{
    return 0;
}
s32 DbgAgebt::localReadHalfWord(u32 & value)
{
    return 0;
}
s32 DbgAgebt::localWriteHalfWord(u32 value)
{
    return 0;
}

s32 DbgAgebt::remotePrint(void *arg, const  char *fmt, va_list ap)
{
    return 0;
}
s32 DbgAgebt::remoteReadChar(u8 &c)
{
    if (recv(_dbgSocket, (char *)&c, 1, 1) < 1) 
        return -1;
    return 0;
}
s32 DbgAgebt::remoteWriteChar(u8 c)
{   
    if (send(_dbgSocket, (const char *)&c, 1, 1) < 0) 
        return -1;
    return 0;
}
s32 DbgAgebt::remoteReadWord(u32 &value)
{
    u8 a, b, c,d;
    if (remoteReadChar(a) < 0) return -1;
    if (remoteReadChar(b) < 0) return -1;
    if (remoteReadChar(c) < 0) return -1;
    if (remoteReadChar(d) < 0) return -1;
    value = a | b << 8 | c << 16 | d << 24;
    return 0;
}
s32 DbgAgebt::remoteWriteWord(u32 value)
{
    if (remoteWriteChar((u8)(value & 0xff)) < 0)
        return -1;
    if (remoteWriteChar((u8)((value & 0xff00) >> 8)) < 0)
        return -1;
    if (remoteWriteChar((u8)((value & 0xff0000) >> 16)) < 0)
        return -1;
    if (remoteWriteChar((u8)((value & 0xff000000) >> 24)) < 0)
        return -1;
    return 0;
}
s32 DbgAgebt::remoteReadHalfWord(u32 & value)
{
    u8 a, b;
    if (remoteReadChar(a) < 0) return -1;
    if (remoteReadChar(b) < 0) return -1;
    value = a | b << 8 ;
    return 0;
}
s32 DbgAgebt::remoteWriteHalfWord(u32 value)
{
    if (remoteWriteChar((u8)(value & 0xff)) < 0)
        return -1;
    if (remoteWriteChar((u8)((value & 0xff00) >> 8)) < 0)
        return -1;
    return 0;
}
void VxDbgAnent_ExceptionHandler(u32 type, u32 address, u32 reason, u32 inst, u32 param)
{
    DbgAgebt *agent = reinterpret_cast<DbgAgebt *>(param);
    if (type == 0 && inst == 0xFFFFFFFFL)  //debug support
    {
        //check to see wether the breakpoint is ok
        std::list<BreakptNode*>::iterator ite;
        for (ite = agent->_breakptList.begin(); ite != agent->_breakptList.end(); ite++)
        {
            BreakptNode * node = (BreakptNode*)(*ite);
            switch (node->type)  {
	        case RDIPoint_EQ:
	            if (address == node->address)  
                    break;
                continue;
	        case RDIPoint_GT:
	            if (address > node->address)
	                break;
	            continue;
	        case RDIPoint_GE:
	            if (address >= node->address)
	                break;
	            continue;
	        case RDIPoint_LT:
	            if (address < node->address)
	                break;
	            continue;
	        case RDIPoint_LE:
	            if (address <= node->address)
	                break;
	            continue;
	        case RDIPoint_IN:
	            if (node->address <= address && address < node->address + node->bound)
	                break;
	            continue;
	        case RDIPoint_OUT:
	            if (node->address > address || address >= node->address + node->bound)
	                break;
	            continue;
	        case RDIPoint_MASK:
	            if ((address & node->bound) == node->address)
	                break;
	            continue;
	        }
            //a mached breakpoint is found, just stop it.
            agent->m_engine->stop(0);
        }
    }//debug exception handler
}
