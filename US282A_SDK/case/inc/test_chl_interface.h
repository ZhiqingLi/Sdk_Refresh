#ifndef _TEST_CHANNEL_INTERFACE_H            //∑¿÷π÷ÿ∂®“Â
#define _TEST_CHANNEL_INTERFACE_H

#include "psp_includes.h"
#include "case_independent.h"
#include "common_bat_charge.h"

typedef void* (*test_chl_op_i)(void*, void*, void*);


typedef enum
{
    CMD_T_CL_RECIEVE_DATA = 0, 
    CMD_T_CL_REQUEST_SEND_CALLBACK, 
    CMD_T_CL_SEND_DATA_DO, 
    CMD_T_CL_MAX, 
} test_chl_cmd_e;


extern void * ud_op_entry(void *param1, void *param2, void *param3, test_chl_cmd_e cmd)__FAR__;

#define test_chl_recieve_data(a,b)       ud_op_entry((void*)(a), (void*)(b), (void*)(0), CMD_T_CL_RECIEVE_DATA)

#define test_request_send_callback(a)    ud_op_entry((void*)(a), (void*)(0), (void*)(0), CMD_T_CL_REQUEST_SEND_CALLBACK)

#define test_send_data()                  ud_op_entry((void*)(0), (void*)(0), (void*)(0), CMD_T_CL_SEND_DATA_DO)

#endif

