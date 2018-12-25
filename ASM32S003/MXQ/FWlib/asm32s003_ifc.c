/*
  ******************************************************************************
  * @file    asm32s003_ifc.c
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "asm32s003_ifc.h"

/* define --------------------------------------------------------------------*/
volatile U8_T IFC_ENDFlag;
volatile U8_T IFC_PRT_ERRFlag;
volatile U8_T IFC_UND_ERRFlag;
volatile U8_T IFC_ADDR_ERRFlag;
volatile U8_T IFC_OVW_ERRFlag;				//IFC interrupt flag
#define	_FlashData_BUFSIZE		10
unsigned char FlashDataArry[_FlashData_BUFSIZE];
unsigned char DataBuffer[_FlashData_BUFSIZE];   //Read out data buffer	
/* externs--------------------------------------------------------------------*/


/*************************************************************
//Enable or Disable IFC Interrupt when Operate FlashData 
//EntryParameter:FlashAdd、DataSize、*BufArry
//ReturnValue:NONE
*************************************************************/
void ProgramData(unsigned int FlashAdd,unsigned int DataSize,unsigned char *BufArry)
{
	int i,DataBuffer;
	
	for(i=0;i<((DataSize+3)/4);i++)   //sizeof structure
	{
		SetUserKey;
		WrProgramAdd(FlashAdd);
		DataBuffer=*BufArry+(*(BufArry+1)<<8)+(*(BufArry+2)<<16)+(*(BufArry+3)<<24);
		WrProgramData(DataBuffer);
		EnNormalProgram;
		StartProgram;
		while((GetCRStatus)!=0);
		BufArry  +=4;
		FlashAdd +=4;
	}
}
/*************************************************************
// ReadFlashData fuction return Data arry save in Flash
//EntryParameter:RdStartAdd、DataLength、*DataArryPoint
//ReturnValue:NONE
*************************************************************/
void ReadDataArry(unsigned int RdStartAdd,unsigned int DataLength,unsigned char *DataArryPoint)
{
	unsigned int i,Buffer;
	
	for(i=0;i<((DataLength+3)/4);i++)
	{
		Buffer=*(volatile unsigned int *)RdStartAdd;
		*DataArryPoint=Buffer;
		*(DataArryPoint+1)=Buffer>>8;
		*(DataArryPoint+2)=Buffer>>16;
		*(DataArryPoint+3)=Buffer>>24;
		RdStartAdd +=4;
		DataArryPoint +=4;
	}
}
/*************************************************************
//ProgramOneWord fuction use to write four data into chip's Rom
//EntryParameter:ProgramAdd、WrData
//ProgramAdd：0x00000000~0x00007c00;0x10000000~0x10000C00
//WrDat：4BYTE
//ReturnValue:NONE
*************************************************************/   
void ProgramOneWord(unsigned int ProgramAdd,unsigned int WrData)
{
	volatile unsigned int Result=0;
	do
	{
		SetUserKey;
		WrProgramAdd(ProgramAdd);
		WrProgramData(WrData);
		EnNormalProgram;
		StartProgram;
		while((GetCRStatus)!=0);
		Result=*(volatile unsigned int *)ProgramAdd;
	}
	while(Result!= WrData);
}
/*************************************************************
//ReadFlashWord fuction return Data save in Flash
//EntryParameter:NONE
//ReturnValue:NONE
*************************************************************/  
unsigned int ReadOneWord(unsigned int RdAdd)
{
	unsigned int DataBuffer=*(volatile unsigned int *)RdAdd;
	return DataBuffer;
}
/*************************************************************
//ChipErase fuction
//EntryParameter:NONE
//ReturnValue:NONE
*************************************************************/
void ChipErase(void)
{
	SetUserKey;
	EnChipErase;
	StartErase;
	while((GetCRStatus)!=0);
	while((GetIFCRawStatus & Undefine_INT)==Undefine_INT)ClrUndefineErro;
}
/*************************************************************
//PageErase fuction
//EntryParameter:XROM_PageAdd_Erase:
//XROM_PageAdd_Erase:PROM_PageAdd0_Erase~PROM_PageAdd35_Erase
//DROM_PageAdd0_Erase~DROM_PageAdd3_Erase
//ReturnValue:NONE
*************************************************************/
void PageErase(IFC_ROMSELETED_TypeDef XROM_PageAdd_Erase)
{
	SetUserKey;
	EnPageErase;
	WrProgramAdd(XROM_PageAdd_Erase);
	StartErase;
	while((GetCRStatus)!=0);
	while((GetIFCRawStatus & Add_INT)==Add_INT)ClrAddErro;;
}
/*************************************************************/
// IFC CMR Set
//EntryParameter:IFC_CMR
//IFC_CMR:Program_Byte/Page_Erase/Chip_Erase/Option_Erase/HDP_1K_EN
//HDP_2K_EN/HDP_4K_EN/HDP_FULL_EN/RDP_EN/DEBUGP_EN/ExRST_EN
//ReturnValue:NONE
/*************************************************************/  
void IFC_CMR_Set(IFC_CMR_TypeDef IFC_CMR)
{
	SetUserKey;
	SetIFCCommand(IFC_CMR);
	StartProgram;
	while((GetCRStatus)!=0);
}
/*************************************************************
//Enable or Disable IFC Interrupt when Operate FlashData 
//EntryParameter:IFC_INT_x
//IFC_INT_x:End_INT、Port_INT、Undefine_INT、Add_INT、OverWrite_INT
//ReturnValue:NONE
*************************************************************/
void IFC_interrupt_CMD(FunctionalStatus NewState ,IFC_INT_TypeDef IFC_INT_x)
{
	if(NewState != DISABLE)
	{
		IFC->ICR =IFC->ICR|IFC_INT_x;
	}
	else 
	{
		IFC->ICR =IFC->ICR & (~IFC_INT_x);
	}
}

/*************************************************************/
//IFC Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void IFC_Int_Enable(void)
{
    IFC->ICLR=0XF000;					//CLAER IFC INT status
	INTC_ISER_WRITE(IFC_INT);    
}

/*************************************************************/
//IFC Interrupt enable
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/
void IFC_Int_Disable(void)
{
    INTC_ICER_WRITE(IFC_INT);    
}
/*************************************************************/
//IFC DATA Write example
//EntryParameter:FlashDataArry[_FlashData_BUFSIZE]
//start address=0x10000000
//ReturnValue:NONE
/*************************************************************/
void FlashData_Write(void)
{
	unsigned int i;
	for(i=0;i<_FlashData_BUFSIZE;i++)
	{
		//FlashDataArry[i]=i;
		DataBuffer[i]=0;
	}
	EnIFCClk;
	PageErase(DROM_PageAdd0_Erase);					///
	ProgramData(0x10000000,_FlashData_BUFSIZE,&FlashDataArry[0]);
	ReadDataArry(0x10000000,_FlashData_BUFSIZE,&DataBuffer[0]);
	for(i=0;i<_FlashData_BUFSIZE;i++)
	{
		while(DataBuffer[i]!=FlashDataArry[i]);
	}
	DisIFCClk;
}

/*************************************************************/
//IFC DATA Read example
//EntryParameter:None
//start address=0x10000000
//ReturnValue:DataBuffer
/*************************************************************/
void FlashData_Read(void)
{
	EnIFCClk;
	ReadDataArry(0x10000000,_FlashData_BUFSIZE,&DataBuffer[0]);
	DisIFCClk;
}
/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/