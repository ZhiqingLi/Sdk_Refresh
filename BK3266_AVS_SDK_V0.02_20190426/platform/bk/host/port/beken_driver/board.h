/* -------------------------------------------------------------------------- */
/* We have provided the software delivered to you for evaluation purposes     */
/* only. It is authorized for use by participants of the specified evaluation */
/* for up to 60 days, unless further use is officially granted by CAST upon   */
/* request.  If you are not an authorized user, please destroy this source    */
/* code file and notify CAST immediately that you inadvertently received an   */
/* unauthorized copy.                                                         */
/* -------------------------------------------------------------------------- */


#ifndef _BOARD_H_
#define _BOARD_H_

#define MC_ENABLED		0
#define IC_ENABLE		0
#define IC_SIZE         	8192
#define DC_ENABLE 	    	0
#define DC_SIZE         	8192

#define IN_CLK  		63000000
#define TICKS_PER_SEC   	100
#define STACK_SIZE		0x10000

// fake peripheral ports for simple_bench
#define CAST_CHAR  		0x90000000
#define CAST_NUM  		0x90000004

#define CAST_INTRMASK  		0x9000000C
#define CAST_INTRSR      	0x90000010
#define CAST_INTRCLR 	        0x90000020

#define CAST_SR          	0x90000014

#define CAST_TT_CNT             0x90000018
#define CAST_TT_INT             0x9000001C

#define CAST_ENDSIM             0x900000F0

#endif
