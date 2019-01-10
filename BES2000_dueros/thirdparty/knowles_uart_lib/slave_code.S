	.globl __hw_start
	.globl __hw_end
	.section .flash_rodata
__hw_start:
    .incbin "../../../thirdparty/knowles_uart_lib/knowles_firmware/sysConfig.bin"
__hw_end:

	.globl __vs_start
	.globl __vs_end
	.section .flash_rodata
__vs_start:
    .incbin "../../../thirdparty/knowles_uart_lib/knowles_firmware/boskoApp.bin"
__vs_end:

	.globl __alexa_start
	.globl __alexa_end
	.section .flash_rodata
__alexa_start:
    .incbin "../../../thirdparty/knowles_uart_lib/knowles_firmware/alexa.bin"
__alexa_end: