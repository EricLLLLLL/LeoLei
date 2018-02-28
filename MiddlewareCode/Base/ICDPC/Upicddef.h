#ifndef UPICDdef_h
#define UPICDdef_h

////////////////////////////////////////////////////////////////////////////////
// HeadName= Card data processing
// Date    = 2001/08/15
// ＶＲＴ  = 01-00-00-00
////////////////////////////////////////////////////////////////////////////////
// error reason
#define INTERFACE_ERR				2
#define BUFFER_ERR					3
#define UNVOLATILIZATION_ERR		4			 
#define LOGIC_ERR					5
#define NO_INFO 					6
#define PARAM_ERR					7

#define AP_MSG WM_USER + 100
#define ICDPC_ICRESPONSEMESSAGEDATAANALYSIS		AP_MSG + 5260					 
#define ICDPC_TLVFORMATANALYSIS					AP_MSG + 5261					 
#define ICDPC_SETTAG							AP_MSG + 5262					 
#define ICDPC_SETTLVLENGTH						AP_MSG + 5263					 
#define ICDPC_SETTLV							AP_MSG + 5264					 
#define ICDPC_GETCOMMANDMESSAGE					AP_MSG + 5265					 

// item length
#define INFOMANAGEMENTSIZE			18432
#define CandidateNUM				64
#define AIDListSize					1024
#define CandidateNUM_MAX			64
#define COMMANDMESSAGEDATASIZE		2048
#define DOLTAGCOUNT					32
#define TAGDATASIZE					128
#define SCRIPTID					10

#define WORD_MAX					0xffff
#define SKIP_ALL					WORD_MAX

// Data format
#define BINARY														0	// BINARY
#define AN															1	// 字母和数字
#define ANS															2	// 字母、数字、特殊字符
#define CN															3	// Compressed digital
#define CH															4	// characters
#define NUM															5	// num
#define DATE														6	// date
#define UC															7	// check use
#define VA															8	// Variable

// Information management
#define MANDATORY													10
#define OPTION														11
#define UNPACK														12
#define PACK														13
#define CONSTANT													14
#define VAR															15
#define IC															16
#define TM															17
#define ISSUER														18
#define SP															19

// Information management get use
#define TAG_READ_FLG												0x100

#define IC_ITEM														0
#define IC_BUFFSIZE													IC_ITEM			+ 1
#define IC_FORMAT													IC_BUFFSIZE		+ 1
#define IC_PACK														IC_FORMAT		+ 1
#define IC_CLEARSIZE												IC_PACK			+ 1
#define IC_VALUE													IC_CLEARSIZE	+ 1
#define IC_ACQUIRED_ADDRESS											IC_VALUE		+ 1
#define IC_MINISIZE													IC_ACQUIRED_ADDRESS	+ 1
#define IC_ALL_MEMBER												IC_MINISIZE		+ 1

// Return value
#define OK				0
#define NG				1
#define NODATA			2

// sift flag
#define ON	1
#define OFF	0


#endif
