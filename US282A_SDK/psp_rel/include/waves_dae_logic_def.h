#ifndef __WAVES_DAE_LOGIC_DEF__
#define __WAVES_DAE_LOGIC_DEF__

#define TUNING_STATUS_SHIFT         (0)
#define TUNING_STATUS_MASK          (0x03<<TUNING_STATUS_SHIFT)

#define INPUT_PARA_ENABLE_SHIFT     (2)
#define INPUT_PARA_ENABLE_MASK      (0x01<<INPUT_PARA_ENABLE_SHIFT)

#define BIN_NUMBER_SHIFT            (3)
#define BIN_NUMBER_MASK             (0x07<<BIN_NUMBER_SHIFT)



typedef enum
{
   TUNING     =  0x01,              //处于调音状态
   NO_TUNING  =  0x02,              //处于非调音状态
   ERROR      =  0x03               //错误
}tuning_status_e;

typedef enum
{
   DISENABLE  =  0x00,              
   ENABLE     =  0x01               
}enable_e;

typedef struct
{
    uint8 tuning_status:2;          //tuning_status_e
    
    uint8 input_para_enable:1;      //enable_e;
    uint8 bin_number:3;             //bin_number:使用第几套音效参数 
    
    uint8 reserve:2;         
} waves_t;

   
#endif
