#ifndef MF_CLASS
#define MF_CLASS
#include "win_type_def.h"
#include "stdio.h"

class MFile
{
	public:
		int mfinit();
		int mfwrite(char* content);
		int mfclose();
	private:
		FILE *fp;
};
#endif
