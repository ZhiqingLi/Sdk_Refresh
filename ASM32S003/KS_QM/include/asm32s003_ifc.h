/*
  ******************************************************************************
  * @file    asm32s003_ifc.h
  * @author  A1semi
  * @version V1.20
  * @date    2017/11/20
  ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _asm32s003_ifc_header_H
#define _asm32s003_ifc_header_H

/* Includes ------------------------------------------------------------------*/
#include "asm32s003.h"
/******************************************************************************
************************* A1semi IFC Registers Definition **************************
******************************************************************************/
#define   IFC_CLK_EN          (0x01ul)
#define   IFC_CLK_DIS         (0xfeul)
#define   EnIFCClk            (IFC->CEDR = (IFC_CLK_EN))
#define   DisIFCClk           (IFC->CEDR = (IFC_CLK_DIS))

#define   USER_KEY            (0x5A5A5A5Aul)
#define   AUTO_KEY            (0x5555AAAAul)
#define   NO_KEY              (0x00000000ul)
#define   SetUserKey          (IFC->KR = (USER_KEY))
#define   ClrUserKey          (IFC->KR = (NO_KEY))
#define   StartOp             (0x01ul)

#define  IFC_CLKEN  (0x01ul) //IFC CLKEN
#define  CSP_IFC_SET_CEDR(ifc, val) (ifc->CEDR = (val))

#define  IFC_SWRST  (0x01ul) //IFC SWRST 
#define  CSP_IFC_SET_SRR(ifc, val) (ifc->SRR = (val))

#define  HIDM0  ((0x0ul)<<8) //HID0
#define  HIDM1  ((0x1ul)<<8) //HID1
#define  HIDM2  ((0x2ul)<<8) //HID2
#define  HIDM3  ((0x3ul)<<8) //HID3
// IFC Command
#define  PROGRAM    (0x01ul)
#define  PAGE_ERASE (0x02ul)
#define  CHIP_ERASE (0x03ul)
#define  IF0_ERASE  (0x04ul)
#define  HDP_1K_ENABLE (0x09ul)
#define  HDP_2K_ENABLE (0x0Aul)
#define  HDP_4K_ENABLE (0x0Bul)
#define  HDP_FULL_ENABLE (0x0Cul)
#define  RDP_ENABLE (0x0Dul)
#define  DBP_ENABLE (0x0Eul)
#define  EXTRST_EN (0x0Ful)
//#define  CMD0   (0x0ul)
//#define  CMD1   (0x1ul)
//#define  CMD2   (0x2ul)
//#define  CMD3   (0x3ul)
//#define  CMD4   (0x4ul)
//#define  CMD5   (0x5ul)
//#define  CMD6   (0x6ul)
//#define  CMD7   (0x7ul)
//#define  CMD8   (0x8ul)
//#define  CMD9   (0x9ul)
//#define  CMDA   (0x0Aul)
//#define  CMDB   (0x0Bul)
//#define  CMDC   (0x0Cul)
//#define  CMDD   (0x0Dul)
//#define  CMDE   (0x0Eul)
//#define  CMDF   (0x0Ful)
#define  CSP_IFC_SET_CMR(ifc, val) (ifc->CMR = (val))
#define  CSP_IFC_GET_CMR(ifc) (ifc->CMR)

#define  START  (0x01ul) //IFC START 
#define  CSP_IFC_SET_CR(ifc, val) (ifc->CR = (val))
#define  CSP_IFC_GET_CR(ifc) (ifc->CR) 

#define  PF_WAIT0 (0x0ul) //PGM Flash Wait
#define  PF_WAIT1 (0x1ul) //PGM Flash Wait
#define  PF_WAIT2 (0x2ul) //PGM Flash Wait
#define  PF_WAIT3 (0x3ul) //PGM Flash Wait
#define  CACHEEN  ((0x01ul)<<4) //cache enable
#define  PFETCHEN ((0x01ul)<<6) //Pre Fetch enable
#define  DF_WAIT0 ((0x0ul)<<8) //DATA Flash Wait
#define  DF_WAIT1 ((0x1ul)<<8) //DATA Flash Wait
#define  DF_WAIT2 ((0x2ul)<<8) //DATA Flash Wait
#define  DF_WAIT3 ((0x3ul)<<8) //DATA Flash Wait
#define  DF_ECCEN ((0x1ul)<<15) //DATA Flash ECC enable
#define  TMRBACEN ((0x1ul)<<31)  //Boot Area protection in user mode
#define  CSP_IFC_SET_MR(ifc, val) (ifc->MR = (val))
#define  CSP_IFC_GET_MR(ifc)  (ifc->MR)

#define  CSP_IFC_SET_AR(ifc, val) (ifc->PF_AR = (val))
#define  CSP_IFC_GET_AR(ifc) (ifc->PF_AR)

#define  USER_KEY (0x5A5A5A5Aul)
#define  AUTO_KEY (0x5555AAAAul)
#define  NO_KEY   (0x00000000ul)
#define  CSP_IFC_SET_KR(ifc, val) (ifc->KR = (val))
#define  CSP_IFC_GET_KR(ifc) (ifc->KR)

//IFC IMSCR
#define SEND  (0x01ul)  //Set END
#define CEND  (0x00ul)  //Clear END

#define SPRT_ERR  ((0x01ul)<<12)
#define CPRT_ERR  ((0x00ul)<<12)

#define SUND_ERR  ((0x01ul)<<13)
#define CUND_ERR  ((0x00ul)<<13)

#define SADDR_ERR  ((0x01ul)<<14)
#define CADDR_ERR  ((0x00ul)<<14)

#define SOVW_ERR  ((0x01ul)<<15)
#define COVW_ERR  ((0x00ul)<<15)

//#define CSP_IFC_SET_IMSCR(ifc, val) ((ifc->IMSCR)=(val))
//#define CSP_IFC_GET_IMSCR(ifc) (ifc->IMSCR)

#define CSP_IFC_GET_RISR(ifc) (ifc->RISR)
#define CSP_IFC_GET_MISR(ifc) (ifc->MISR)

#define CSP_IFC_SET_ICR(ifc,val) ((ifc->ICR)= (val))
#define CSP_IFC_SET_ICLR(ifc,val) ((ifc->ICLR)= (val))


#define WLDIS  	(0x01ul)
#define YMXALL  ((0x01ul)<<1)
#define MARGIN  ((0x01ul)<<2)
#define HREFMON ((0x01ul)<<3)
#define BLMON   ((0x01ul)<<4)
#define WLMON   ((0x01ul)<<5)
#define PUMPDIS ((0x01ul)<<6)
#define IPGM    ((0x01ul)<<7)
#define TIOSEL0 ((0x00ul)<<8)
#define TIOSEL1 ((0x01ul)<<8)
#define TIOSEL2 ((0x02ul)<<8)
#define TIOSEL3 ((0x03ul)<<8)
#define TIOSEL4 ((0x04ul)<<8)
#define TIOSEL5 ((0x05ul)<<8)
#define TIOSEL6 ((0x06ul)<<8)
#define TIOSEL7 ((0x07ul)<<8)
#define TCMREN  ((0x01ul)<<11)
#define EXHVEN  ((0x01ul)<<12)
#define ECCEN   ((0x01ul)<<16)
#define BACEN   ((0x01ul)<<17)
#define MAS     ((0x01ul)<<18)
#define FUSERD  ((0x01ul)<<19)
#define RDBIST  ((0x01ul)<<20)
#define HSLAT   ((0x01ul)<<24)

#define CSP_IFC_SET_DR(ifc,val) ((ifc->PF_DR)=(val))
#define CSP_IFC_GET_DR(ifc) (ifc->PF_DR)
/**
  * @brief  IFC CMR register
  */
typedef enum
{
	Program_Byte      =  (0x01ul),
	Page_Erase        =  (0x02ul),
	Chip_Erase        =  (0x03ul),
	Option_Erase      =  (0x04ul),
	HDP_1K_EN         =  (0x09ul),
	HDP_2K_EN         =  (0x0aul),
	HDP_4K_EN         =  (0x0bul),
	HDP_FULL_EN       =  (0x0cul),
	RDP_EN            =  (0x0dul),
	DEBUGP_EN         =  (0x0eul),
	ExRST_EN          =  (0x0ful)
}IFC_CMR_TypeDef;
/**
  * @brief  IFC HID register
  */
typedef enum
{
	HID_Normal        =  (0x00ul<<8),
	HID_User          =  (0x01ul<<8),
	HID_Rsvd1         =  (0x02ul<<8),
	HID_Rsvd2         =  (0x03ul<<8),
}IFC_HID_TypeDef;
/**
  * @brief  IFC INT mode
  */
typedef enum
{
	End_INT           =  (0x01ul),
	Port_INT          =  ((0x01ul)<<12),
	Undefine_INT      =  ((0x01ul)<<13),
	Add_INT           =  ((0x01ul)<<14),
	OverWrite_INT     =  ((0x01ul)<<15)
}IFC_INT_TypeDef;
/**
  * @brief  IFC page address
  */
typedef enum
{
	PROM_PageAdd0_Erase          =  ((CSP_REGISTER_T)0x00000000),
	PROM_PageAdd1_Erase          =  ((CSP_REGISTER_T)0x00000400),
	PROM_PageAdd2_Erase          =  ((CSP_REGISTER_T)0x00000800),
	PROM_PageAdd3_Erase          =  ((CSP_REGISTER_T)0x00000C00),
	PROM_PageAdd4_Erase          =  ((CSP_REGISTER_T)0x00001000),
	PROM_PageAdd5_Erase          =  ((CSP_REGISTER_T)0x00001400),
	PROM_PageAdd6_Erase          =  ((CSP_REGISTER_T)0x00001800),
	PROM_PageAdd7_Erase          =  ((CSP_REGISTER_T)0x00001C00),
	PROM_PageAdd8_Erase          =  ((CSP_REGISTER_T)0x00002000),
	PROM_PageAdd9_Erase          =  ((CSP_REGISTER_T)0x00002400),
	PROM_PageAdd10_Erase         =  ((CSP_REGISTER_T)0x00002800),
	PROM_PageAdd11_Erase         =  ((CSP_REGISTER_T)0x00002C00),
	PROM_PageAdd12_Erase         =  ((CSP_REGISTER_T)0x00003000),
	PROM_PageAdd13_Erase         =  ((CSP_REGISTER_T)0x00003400),
	PROM_PageAdd14_Erase         =  ((CSP_REGISTER_T)0x00003800),
	PROM_PageAdd15_Erase         =  ((CSP_REGISTER_T)0x00003C00),
	PROM_PageAdd16_Erase         =  ((CSP_REGISTER_T)0x00004000),
	PROM_PageAdd17_Erase         =  ((CSP_REGISTER_T)0x00004400),
	PROM_PageAdd18_Erase         =  ((CSP_REGISTER_T)0x00004800),
	PROM_PageAdd19_Erase         =  ((CSP_REGISTER_T)0x00004C00),
	PROM_PageAdd20_Erase         =  ((CSP_REGISTER_T)0x00005000),
	PROM_PageAdd21_Erase         =  ((CSP_REGISTER_T)0x00005400),
	PROM_PageAdd22_Erase         =  ((CSP_REGISTER_T)0x00005800),
	PROM_PageAdd23_Erase         =  ((CSP_REGISTER_T)0x00005C00),
	PROM_PageAdd24_Erase         =  ((CSP_REGISTER_T)0x00006000),
	PROM_PageAdd25_Erase         =  ((CSP_REGISTER_T)0x00006400),
	PROM_PageAdd26_Erase         =  ((CSP_REGISTER_T)0x00006800),
	PROM_PageAdd27_Erase         =  ((CSP_REGISTER_T)0x00006C00),
	PROM_PageAdd28_Erase         =  ((CSP_REGISTER_T)0x00007000),
	PROM_PageAdd29_Erase         =  ((CSP_REGISTER_T)0x00007400),
	PROM_PageAdd30_Erase         =  ((CSP_REGISTER_T)0x00007800),
	PROM_PageAdd31_Erase         =  ((CSP_REGISTER_T)0x00007C00),
	PROM_PageAdd32_Erase         =  ((CSP_REGISTER_T)0x00008000),
	PROM_PageAdd33_Erase         =  ((CSP_REGISTER_T)0x00008400),
	PROM_PageAdd34_Erase         =  ((CSP_REGISTER_T)0x00008800),
	PROM_PageAdd35_Erase         =  ((CSP_REGISTER_T)0x00008C00),
	DROM_PageAdd0_Erase          =  ((CSP_REGISTER_T)0x10000000),
	DROM_PageAdd1_Erase          =  ((CSP_REGISTER_T)0x10000400),
	DROM_PageAdd2_Erase          =  ((CSP_REGISTER_T)0x10000800),
	DROM_PageAdd3_Erase          =  ((CSP_REGISTER_T)0x10000C00)
}IFC_ROMSELETED_TypeDef;

#define   EnNormalProgram     (IFC->CMR =(Program_Byte|HID_Normal))
#define   EnUserProgram       (IFC->CMR =(Program_Byte|HID_User))
#define   DisNormlProgram     (IFC->CMR =(NO_KEY))
#define   DisUserProgram      (IFC->CMR =(NO_KEY))
#define   GetCMRStatus        (IFC->CMR)
#define   WrProgramAdd(val)   (IFC->PF_AR=(val))
#define   WrProgramData(val)  (IFC->PF_DR=(val))
#define   GetProgramAdd       (IFC->PF_AR)
#define   StartProgram        (IFC->CR=(StartOp))
#define   StartErase          (IFC->CR=(StartOp))
#define   GetCRStatus         (IFC->CR)
#define   EnPageErase         (IFC->CMR=(Page_Erase|HID_Normal))
#define   EnUserPageErase     (IFC->CMR=(Page_Erase|HID_User))
#define   EnChipErase         (IFC->CMR=(Chip_Erase|HID_User))
#define   WaitCompleted       (while((GetCRStatus)!=0))
#define   SetIFCCommand(val)  (IFC->CMR =(val|HID_User))

#define   ClrIFCEnd           ((IFC->ICR)= (End_INT))
#define   ClrPortErro         ((IFC->ICR)= (Port_INT))
#define   ClrUndefineErro     ((IFC->ICR)= (Undefine_INT))
#define   ClrAddErro          ((IFC->ICR)= (Add_INT))
#define   ClrOverWriteErro    ((IFC->ICR)= (OverWrite_INT))
#define   GetIFCRawStatus     (IFC->RISR)
#define   GetIFCMaskStatus    (IFC->MISR)

/** @addtogroup IFC_Exported_functions
  * @{
  */	
extern void ProgramData(unsigned int FlashAdd,unsigned int DataSize,unsigned char *BufArry);
extern void ChipErase(void);
extern void PageErase(IFC_ROMSELETED_TypeDef XROM_PageAdd_Erase);
extern void ProgramOneWord(unsigned int ProgramAdd,unsigned int WrData);
extern unsigned int ReadOneWord(unsigned int RdAdd);
extern void ReadDataArry(unsigned int RdStartAdd,unsigned int DataLength,unsigned char *DataArryPoint);
extern void IFC_CMR_Set(IFC_CMR_TypeDef IFC_CMR);
extern void IFC_interrupt_CMD(FunctionalStatus NewState ,IFC_INT_TypeDef IFC_INT_x);
extern void IFC_Int_Enable(void);
extern void IFC_Int_Disable(void);
extern void FlashData_Write(void);
extern void FlashData_Read(void);
#endif   /**< asm32s003_syscon_H */

/******************* (C) COPYRIGHT 2016 A1semi Chip *****END OF FILE****/