#ifndef __NUTERROR_H__
#define __NUTERROR_H__

#ifdef __cplusplus
extern "C" {
#endif

enum _NutState {
    NUT_OK               = 0,
    NUT_ERR            = 1,
	NUT_WARNING,
	NUT_FATAL,
	NUT_ERR_MALLOC,
	NUT_ERR_OUT_RANGE,
    NUT_ERR_INVALID_CAPACITY,
    NUT_ERR_INVALID_RANGE,
    NUT_ERR_MAX_CAPACITY,
    NUT_ERR_KEY_NOT_FOUND,
    NUT_ERR_VALUE_NOT_FOUND,
    NUT_ERR_OUT_OF_RANGE,
	NUT_ERR_NOT_FIND,
    NUT_ITER_END,
};

typedef enum _NutState NutState;

struct _NutError{
	int num;
	const char str;
	char * info;
};

typedef struct _NutError NutError;

void nut_error_out(NutError * nuterror);
void nut_error_handle(NutError * nuterror);

#ifdef __cplusplus
}
#endif

#endif
