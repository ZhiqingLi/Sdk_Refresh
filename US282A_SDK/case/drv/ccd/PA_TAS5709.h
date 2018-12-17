//TAS5709µÄ¼Ä´æÆ÷¶¨Òå
#ifndef	__PACFG_H__
#define	__PACFG_H__

#define  SET_MUTE                               0x03
#define  SET_UNMUTE                             0x00
#define  R_MODE                                 1
#define	 W_MODE                                 0
#define  MAX_VOL_STEP                           41

//Register Map
#define TI_TAS5709_CLOCK_CONTROL		            0x00
#define TI_TAS5709_DEVICE_ID			              0x01
#define TI_TAS5709_ERROR_STATUS			            0x02
#define TI_TAS5709_SYSTEM_CONTROL_1		          0x03
#define TI_TAS5709_I2S_MODES			              0x04
#define TI_TAS5709_POWER_CONTROL		            0x05
#define TI_TAS5709_SOFTWARE_MUTE		            0x06
#define TI_TAS5709_MASTER_VOL			              0x07
#define TI_TAS5709_CHAN1_VOL			              0x08
#define TI_TAS5709_CHAN2_VOL			              0x09
#define TI_TAS5711_CHAN3_VOL			              0x0A
#define TI_TAS5709_VOLUME_CONFIG 	      	      0x0E
#define TI_TAS5709_MODULATION_LIMIT            	0x10

#define TI_TAS5709_IC_DELAY_PWM1               	0x11
#define TI_TAS5709_IC_DELAY_PWM2               	0x12
#define TI_TAS5709_IC_DELAY_PWM3               	0x13
#define TI_TAS5709_IC_DELAY_PWM4               	0x14

#define TI_TAS5709_START_STOP_PERIOD           	0x1A
#define TI_TAS5709_OSCILLATOR_TRIM             	0x1B
#define TI_TAS5709_BKND_ERR             	      0x1C

#define TI_TAS5709_INPUT_MULTIPLEXER            0x20		
#define TI_TAS5709_CHANNEL4_SOURCE_SEL          0x21		
#define TI_TAS5709_PWM_OUTPUT_MUX		            0x25		
#define TI_TAS5709_DRC_CONTROL                 	0x46
#define TI_TAS5709_EQ_CONTROL		                0x50

#define TI_TAS5709_I2C_ADDR			                (0x36)

#define TI_TAS5731_20_I2C_ADDR			            (0x54)

#define TI_TAS5731_01_I2C_ADDR			            (0x34)

//Register Map

#define TI_TAS5707_CLOCK_CONTROL		            0x00
#define TI_TAS5707_DEVICE_ID			              0x01
#define TI_TAS5707_ERROR_STATUS			            0x02
#define TI_TAS5707_SYSTEM_CONTROL_1		          0x03
#define TI_TAS5707_I2S_MODES			              0x04
#define TI_TAS5707_POWER_CONTROL		            0x05
#define TI_TAS5707_SOFTWARE_MUTE		            0x06
#define TI_TAS5707_MASTER_VOL			              0x07
#define TI_TAS5707_CHAN1_VOL			              0x08
#define TI_TAS5707_CHAN2_VOL			              0x09

#define TI_TAS5707_VOLUME_CONFIG 	      	      0x0E
#define TI_TAS5707_MODULATION_LIMIT            	0x10

#define TI_TAS5707_IC_DELAY_PWM1               	0x11
#define TI_TAS5707_IC_DELAY_PWM2               	0x12
#define TI_TAS5707_IC_DELAY_PWM3               	0x13
#define TI_TAS5707_IC_DELAY_PWM4               	0x14

#define TI_TAS5707_SHUTDOWN_GROUP_REGISTER     	0x19
#define TI_TAS5707_START_STOP_PERIOD           	0x1A
#define TI_TAS5707_OSCILLATOR_TRIM             	0x1B

#define TI_TAS5707_INPUT_MUX                   	0x20		//INPUT MUX CONFIGURATION
#define TI_TAS5707_INPUT_DOWN_MIX              	0x21		//INPUT DOWNMUX CONFIGURATION
#define TI_TAS5707_PWM_OUTPUT_MUX		            0x25		//PWM OUTPUT MUX CONFIGURATION
#define TI_TAS5707_CHANNEL1_BQ0                	0x29
#define TI_TAS5707_CHANNEL1_BQ1                	0x2A
#define TI_TAS5707_CHANNEL1_BQ2               	0x2B
#define TI_TAS5707_CHANNEL1_BQ3                	0x2C
#define TI_TAS5707_CHANNEL1_BQ4                	0x2D
#define TI_TAS5707_CHANNEL1_BQ5                	0x2E
#define TI_TAS5707_CHANNEL1_BQ6                	0x2F

#define TI_TAS5707_CHANNEL2_BQ0                	0x30
#define TI_TAS5707_CHANNEL2_BQ1                	0x31
#define TI_TAS5707_CHANNEL2_BQ2                	0x32
#define TI_TAS5707_CHANNEL2_BQ3                	0x33
#define TI_TAS5707_CHANNEL2_BQ4                	0x34
#define TI_TAS5707_CHANNEL2_BQ5                	0x35
#define TI_TAS5707_CHANNEL2_BQ6                	0x36

#define TI_TAS5707_DRC1_AE                     	0x3A
#define TI_TAS5707_DRC1_AA                     	0x3B
#define TI_TAS5707_DRC1_AD                     	0x3C
#define TI_TAS5707_DRC1_T                      	0x40
#define TI_TAS5707_DRC1_K                      	0x41
#define TI_TAS5707_DRC1_O                      	0x42
#define TI_TAS5707_DRC_CONTROL                 	0x46

#define TI_TAS5707_EQ_CONTROL		                0x50

#endif
