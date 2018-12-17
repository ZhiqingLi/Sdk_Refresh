#ifndef _NET_APP_H_
#define _NET_APP_H_


#define IPV4_ADDR(a)    (((u8*)(a))[0]) & 0xff, (((u8*)(a))[1]) & 0xff, (((u8*)(a))[2]) & 0xff, (((u8*)(a))[3]) & 0xff

typedef void (*net_app_func)(s32, char **);


typedef struct net_app_info_st
{
    char            *app_name;
    net_app_func    app_func;
    bool            need_to_restrict; /* Restrict task just used one thread*/
} net_app_info;


/**
* The arguments for the net_app task. 
*/
typedef struct net_app_args_st 
{
    char            *cmd_buf; /* to hold command line buffer */
    net_app_info    *app_info;
} net_app_args;



s32 net_app_init(void);
s32 net_app_run(s32 argc, char *argv[]);
s32 net_app_show(void);
void net_app_task(void *args);




#endif /* _NET_APP_H_ */

