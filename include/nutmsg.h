#ifndef __NUTMSG_H__
#define __NUTMSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nutinc.h"

#define NUT_MSG_DATA_LEN   32

struct _NutMsg{
	int size;
	union{
		char  data[NUT_MSG_DATA_LEN];
		char  ret[NUT_MSG_DATA_LEN];
	};
	char * retp;

};

typedef struct _NutMsg NutMsg;




#ifdef __cplusplus
}
#endif

#endif
