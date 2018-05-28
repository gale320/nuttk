#ifndef __NUTMODULE_H__
#define __NUTMODULE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nutinc.h"
#include "nutmsg.h"


enum _ModuleStatus{
	MOD_NOT_INIT,
	MOD_INIT,
	MOD_RUNNING,
	MOD_STOP,
	MOD_FREEZ,
	MOD_DESTROY,
	MOD_ERROR
};
typedef enum _ModuleStatus ModuleStatus;
typedef enum _ModuleStatus ModStatus;

struct _NutModule {
	const char * name;
	const char * cmd;
//	const char * option[CMD_OPTION_MAX];
//	bool (*optHandle[CMD_OPTION_MAX])(Command *cmd);
	const char * help;
	const char * descrip;
	const char * version;
	bool (*init)();
	bool (*destroy)();
	bool (*handle)(NutMsg *msg);
	ModStatus status;
};

typedef struct _NutModule NutModule;
typedef struct _NutModule NutMod;


bool nut_mod_create(NutModule ** mod);
bool nut_mod_destroy(NutModule * mod);

bool nut_mod_list_create(NutModule ** modList);
bool nut_mod_list_destroy(NutModule * modList);



#ifdef __cplusplus
}
#endif

#endif
