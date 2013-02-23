#ifndef TVMUTIL_INC
#define TVMUTIL_INC

#include "tvm.h"

// Size in bits of data type
#define BIT_SIZE(type) (sizeof(type) * 8)

#ifndef MAX
#define MAX(a, b) (((a) >= (b))?(a):(b))
#define MIN(a, b) (((a) <= (b))?(a):(b))
#endif


template <class T>
bool cast_string_to_number(T & t, const std::string & str,
    std::ios_base & (*f)(std::ios_base&)) {
    std::istringstream iss(str);
    return !(iss >> f >> t).fail();
}

template <class T>
bool cast_string_to_number(T & t,const char * str,
    std::ios_base & (*f)(std::ios_base&)) {
    std::istringstream iss(str);
    return !(iss >> f >> t).fail();
}

inline u32 rotateShiftRight(u32 val, u8 bit) {
    u32 result = 0;
    _asm {
        mov eax, val
        mov cl, bit
        ror eax, cl
        mov result, eax
    }
    return result;
}

inline u32 rotateShiftLeft(u32 val, u8 bit) {
    u32 result = 0;
    _asm {
        mov eax, val
        mov cl, bit
        rol eax, cl
        mov result, eax
    }
    return result;
}
inline u32 logicShiftLeft(u32 val, u8 bit) {
    return val << bit;
}
inline u32 logicShiftRight(u32 val, u8 bit) {   
    return val >> bit;
}
inline u32 arithShiftLeft(u32 val, u8 bit) {
    return val << bit;
}
inline u32 arithShiftRight(u32 val, u8 bit) {
    return val >> bit;
}
inline s32 signExtend(u32 val, u8 bit) {
    s32 result = val;
    if (bit < 32)
        result = ((s32)(val << (32 - bit))) >> (32 - bit);
    return result;
}

inline bool isOverflowFrom2(u8 bitsize, u32 op1, u32 op2) {
    u32 result = 0; 
    if (bitsize == BIT_SIZE(u16)) {
        __asm {
            mov ax, word ptr op1
            mov bx, word ptr op2
            mov cx, 1
            sub ax, bx
            cmovo ax, cx
            mov word ptr result, ax            
        }
    }
    else if (bitsize == BIT_SIZE(u32)) {
        __asm {
            mov eax, dword ptr op1
            mov eax, dword ptr op2
            mov ecx, 1
            sub eax, ebx
            cmovo eax, ecx
            mov result, eax
        }
    }
    return (result == 1?true:false);
}

inline bool isCarryFrom2(u8 bitsize, u32 op1, u32 op2) {
    u32 result = 0; 
    if (bitsize == BIT_SIZE(u16)) {
        __asm {
            mov ax, word ptr op1
            mov bx, word ptr op2
            mov cx, 1
            sub ax, bx
            cmovc ax, cx
            mov word ptr result, ax
        }
    }
    else if (bitsize == BIT_SIZE(u32)) {
        __asm {
            mov eax, dword ptr op1
            mov ebx, dword ptr op2
            mov ecx, 1
            sub eax, ebx
            cmovc eax, ecx
            mov result, eax
        }
    }

    return (result == 1?true:false);
}

inline bool isCarryFrom(u8 bitsize, u32 op1, u32 op2) {
    u32 result = 0;
    if (bitsize == BIT_SIZE(u16)) {
        __asm {
            mov ax, word ptr op1
            mov bx, word ptr op2
            mov cx, 1
            cmp ax, bx
            cmovc ax, cx
            mov word ptr result, ax
        }
    }
    else if (bitsize == BIT_SIZE(u32)) {
        __asm {
            mov eax, dword ptr op1
            mov ebx, dword ptr op2
            mov ecx, 1
            cmp eax, ebx
            cmovc eax, ecx
            mov result, eax
        }
    }
    return (result == 1?true:false);
}
inline bool isOverflowFrom(u8 bitsize, u32 op1, u32 op2)
{ 
    u32 result = 0; 
    if (bitsize == BIT_SIZE(u16)) {
        __asm {
            mov ax, word ptr op1
            mov bx, word ptr op2
            mov cx, 1
            add ax, bx
            cmovo ax, cx
            mov word ptr result, ax
        }
    }
    else if (bitsize == BIT_SIZE(u32)) {
        __asm {
            mov eax, dword ptr op1
            mov ebx, dword ptr op2
            mov ecx, 1
            add ebx, ebx
            cmovo eax, ecx
            mov result, eax
        }
    }
    return (result == 1?true:false);
}

inline bool isBorrowFrom(u8 bitsize, u32 op1, u32 op2) {
    u32 result = false;
    if (bitsize == BIT_SIZE(u16)) {
        __asm {
            mov ax, word ptr op1
            mov bx, word ptr op2
            mov cx, 1
            sub ax, bx
            cmovs ax, cx
            mov word ptr result, ax
        }
    }
    else if (bitsize == BIT_SIZE(u32)) {
        __asm {
            mov eax, dword ptr op1
            mov ebx, dword ptr op2
            mov ecx, 1
            sub eax, ebx
            cmovs eax, ecx
            mov result, eax
        }
    }
    return (result == 1?true:false);
}

    


// Macro to set destination buffer with the given
// data item
#define FLUSH_DATA(dst, data) (*(dst) = (data))


// Create a mask of bits sized to the number of bits
// in the given type
#define COPY_MASK(type) ((0xffffffff) >> (32 - BIT_SIZE(type)))

// Masks the number of bits give by length starting
// at the given offset.  Unlike MASK and MASK_AND_SHIFT,
// this macro only creates that mask, it does not
// operate on the data buffer
#define MASK_B(offset, len) \
  ((0xff >> offset) & (0xff << (8 - (offset + len))))


// Masks the bits in data at the given offset for
// given number of width bits
inline u8 MASK(u8 width, u8 offset, u8 data) {
    if (width == BIT_SIZE(data))
        return data;
    else {
        u8 tmp = 0xff << (8 - width % 8 - offset);
        tmp >>= (8 - width%8 - offset);
        tmp &= data;
        return tmp;    
    }

}

// Same as the macro MASK except also shifts the data
// in the result by the given number of shift bits
#define MASK_AND_SHIFT(width, offset, shift, data)  \
                  ((((signed) (shift)) < 0) ?       \
                    MASK((width), (offset), (data)) << -(shift) :  \
                    MASK((width), (offset), (data)) >>  (((unsigned) (shift)))) 

#define MASK_AND_SHIFT_RIGHT(width, offset, shift, data)   \
                    (MASK((width), (offset), (data)) >>  ((unsigned)(shift)))

#define MASK_AND_SHIFT_LEFT(width, offset, shift, data)    \
                    (MASK((width), (offset), (data)) << ((unsigned)(shift)))


#define LITLEND_PTR_ADD(p,i)     (((((dword)(p))^1)+(i))^1)

inline u8 bitsUnpack2Byte( u8 *src, u8 pos, u8 len) {
   u8 result = 0;
   
   src += pos/8;
   pos %= 8;
   
   if ((8 - (pos + len)) >= 0) { //in one byte
      result = (u8)MASK(len, pos, *src) >> pos;
   }
   else {
     result = (u8)MASK(8-pos, pos, *src) >> pos;
     src++;
     len -= (8 - pos);
     u8 tmp = 0x00;
     if ( len > 0 ) {
         tmp = (u8)MASK(len, 0, *src);
         tmp = tmp << (8-pos);
         result |= tmp;
     }
   }

   return result;
}

inline u16  bitsUnpack2Word(u8* src, u8 pos, u8 len) { 
   u16 result = 0;
   
   src += pos/8;
   pos %= 8;
   
   if ((8 - (pos + len)) >= 0) { //in one byte
      result = (u8)MASK(len, pos, *src) >> pos;
   }
   else {
     result = (u8)MASK(8-pos, pos, *src) >> pos;
     src++;
     len -= (8 - pos);
     u16 tmp = 0x00;
     if ( len > 0 ) {
         tmp = (u8)MASK(len, 0, *src);
         tmp = tmp << (8-pos);
         result |= tmp;
     }
   }

   return result;
}

inline u32 bitsUnpack2Dword(u8 src[], u8 pos, u8 len) { 
  u8 result = 0;
  s32 rshift = 0;

  src += pos/8;
  pos %= 8;

  rshift = MAX( 8 - (pos + len), 0);
  if ( rshift > 0 )  {
     result = MASK_AND_SHIFT(len, pos, rshift, *src);
  }
  else {

     result = MASK(8-pos, pos, *src);
     src++;
     len -= 8 - pos;

     for ( ; len >= 8  ; len -= 8) {
        result = ( result<<8 ) | *src++;
     }

     if ( len > 0 ) {
         result = ( result<<len ) | (*src >> (8-len));  // if any bits left
     }
  }

  return result;
}

inline void bitsCopy(void *srcptr, u32  spos, void  *dstptr, u32  dpos, u32  len ) {

    u32 tmp = 0xffffffff;
	u32 dst = *(u32 *)dstptr;
	u32 src = *(u32 *)srcptr;

	if(dpos > 0)
		tmp = tmp >> dpos;
	tmp = tmp << (32 - len);
	tmp = tmp >> (32 - dpos - len);

	dst &= tmp;
	src &= ~tmp;
	
	src |= dst;

	*(u32 *)srcptr = src;

	return;
} 

#endif //TVMUTIL_INC