/****************************************************************************
*                                                                           *
* win_type_def.h -- Basic Windows Type Definitions                          *
*                                                                           *
*                                                                           *
****************************************************************************/


#ifndef _WIN_TYPE_DEF_
#define _WIN_TYPE_DEF_

#ifdef __cplusplus
extern "C" {
#endif

typedef long				LONG;
typedef char				*PSZ;
typedef unsigned int                    DWORD;
typedef int				BOOL;
typedef unsigned char                   BYTE;
typedef unsigned short                  WORD;
typedef float				FLOAT;
typedef FLOAT				*PFLOAT;
typedef BOOL				*LPBOOL;
typedef BYTE                            *LPBYTE;
typedef int				*LPINT;
typedef WORD				*LPWORD;
typedef long				*LPLONG;
typedef DWORD				*LPDWORD;
typedef void				*LPVOID;
typedef int				INT;
typedef unsigned int                    UINT;
typedef unsigned int                    *PUINT;
typedef void				*HANDLE;
typedef char				*LPSTR;
typedef char				*LPTSTR;
typedef const char			*LPCSTR;

typedef const char			*LPCTSTR;
typedef unsigned short                  *LPWSTR;
typedef const unsigned short            *LPCWSTR;
typedef unsigned short                   USHORT;
typedef char 				CHAR;
typedef short 				SHORT;
typedef unsigned long                   ULONG;
typedef unsigned char                   UCHAR;
typedef SHORT 				*LPSHORT;
typedef LPVOID 				*LPLPVOID;
typedef ULONG 				*LPULONG;
typedef USHORT 				*LPUSHORT;


/* Types use for passing & returning polymorphic values */
typedef UINT WPARAM;
typedef LONG LPARAM;
typedef LONG LRESULT;

#define TRUE                                     (1)
#define FALSE                                    (0)

#define MAX_PATH 			260

#define INFINITE 			0xFFFFFFFF

#define VOID            	void

#define MAX_COMPUTERNAME_LENGTH         (31)
#define S_OK                                   (0L)

//#ifndef max
//#define max(a,b)            (((a) > (b)) ? (a) : (b))
//#endif

//#ifndef min
//#define min(a,b)            (((a) < (b)) ? (a) : (b))
//#endif

#define MINCHAR     0x80
#define MAXCHAR     0x7f
#define MINSHORT    0x8000
#define MAXSHORT    0x7fff
#define MINLONG     0x80000000
#define MAXLONG     0x7fffffff
#define MAXBYTE     0xff
#define MAXWORD     0xffff
#define MAXDWORD    0xffffffff

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define SAFE_DELETE(p) if(p){delete p; p=NULL;}

typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, *LPRECT;

typedef const RECT * LPCRECT;

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *LPPOINT;

typedef struct tagSIZE
{
    LONG        cx;
    LONG        cy;
} SIZE, *PSIZE, *LPSIZE;

typedef SIZE               SIZEL;
typedef SIZE               *PSIZEL, *LPSIZEL;

#ifdef __cplusplus
}
#endif

#endif /* _WIN_TYPE_DEF_ */

