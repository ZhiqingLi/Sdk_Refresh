#ifndef _APP_EQU_H_
#define _APP_EQU_H_

//audio equalizer
typedef struct
{
    int16 x[2][3];
    int16 y[2][2];
    int    a[2];
    int    b[3];
    uint8 flag_enable;
}__PACKED_POST__ aud_equ_t;


typedef   struct _app_eq_para_t
{
    uint8    flag;
    int32   a[2];
    int32   b[3];
}__PACKED_POST__ app_eq_para_t;

typedef struct
{
    uint8 flag_type;     //add by zjw 131008  0:shelving  1:peak 
	uint8 flag_low_high;
    int    gain;
    int    fc;
    int    fs;
    int    bandwidth;    //add by zjw 131008
}__PACKED_POST__ aud_equ_conf_t;


//int aud_apply_equalizer( int index, aud_equ_conf_t *conf );
void aud_euqalizer_enable( int index, uint8 enable );
void app_equalizer_calc( int16 samplel, int16 sampler, int16 *outl, int16 *outr );
int16 app_equalizer_calc_hfp( int16 sample );
void app_equalizer_a2dp_init( app_eq_para_t * app_a2dp_equ );
int app_equalizer_get_enable_count( void );
void app_equalizer_show( void );
void app_equalizer_a2dp_apply(void);
void app_equalizer_hfp_apply(void);
void app_equalizer_init(void);
void app_equalizer_recover_enable( void );

#endif
