/// tvm
/// virtual machine for toyable language

#ifndef VXDEBUG_INC
#define VXDEBUG_INC

#include "tvm.h"

typedef unsigned int BreakptHandle;
typedef unsigned int WatchptHandle;

using namespace std;

struct BreakptNode  {	
    u32 address;		//The address of this breakpoint 
    u8  type;		    //The type of comparison
    u32 bound;		    //The other address for a range */
    u32 inst;            //instruction
    BreakptHandle handle;
};

struct WatchptNode		
    u32 address;		//The address of this watchpoint 
    u8  type;		    //The type of comparison
    u8  datatype;		//The type of access to watch for 
    u32 bound;		    //The other address for a range
    WatchptHandle handle;
};

class DbgAgent {
public:
    DbgAgent(TVMEngine * engine);
    DbgAgent(){}

    s32 open(u32 mode);
    void close();
    s32 execute();

    u32 readMemory(u32 address, u8*buffer, u32 size);
    u32 writeMemory(u32 address, u8*buffer, u32 size);

    ///read cpu state
    u32 readCpuState(u32 mode, u32 mask, u8*buffer, u32 size);
    u32 writeCpuState(u32 mode, u32 mask, u8*buffer, u32 size);
    u32 readCopState(u8 cpnum, u32 mask, u8*buffer, u32 size);
    u32 writeCopState(u8 cpnum, u32 mask, u8*buffer, u32 size);
    u32 setBreakpt(u32 address, u8 mode, u16 bound, BreakptHandle *handle);
    u32 delBreakpt(BreakptHandle handle);
    u32 setWatchpt(u32 address, u8 type, u8 dataType, u16 bound, BreakptHandle *handle);  
    u32 delWatchpt(BreakptHandle handle);
    u32 execute(BreakptHandle *handle);
    u32 step(u32 ninst, BreakptHandle *handle);
    u32 info(u8 type, u16 *arg1, u16 *arg2);
    
 private:
    s32 executeRDI();
    void pause(void *arg);

    s32 localPrint(void *arg, const  char *fmt, va_list ap);
    s32 localReadChar(u8 &c);
    s32 localWriteChar(u8 c);
    s32 localReadWord(u32 &value);
    s32 localWirteWord(u32 value);
    s32 localReadHalfWord(u32 & value);
    s32 localWriteHalfWord(u32 value);
private:
    TVMEngine *m_engine;
    u32 m_mode;
    list<BreakptNode *> m_breakptList;
    list<WatchptNode *> m_watchptList;
};
#endif
