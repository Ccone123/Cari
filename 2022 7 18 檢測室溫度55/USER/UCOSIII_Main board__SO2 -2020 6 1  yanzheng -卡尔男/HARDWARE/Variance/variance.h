#ifndef _VARIANCE_H
#define _VARIANCE_H

#include "sys.h"
#define  FLITE_NUM 100
#define		ELM_CNT 	(8)	


float filtStack (float *source , float readFiltBuf[],u8* readCnt,u8 isRand);


#endif


