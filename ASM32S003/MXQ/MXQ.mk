##
## Auto Generated makefile by CDK
## Do not modify this file, and any manual changes will be erased!!!   
##
## BuildSet
ProjectName            :=MXQ
ConfigurationName      :=BuildSet
WorkspacePath          :=F:/Soft_sdk/ASM32S003/
ProjectPath            :=F:/Soft_sdk/ASM32S003/MXQ/
IntermediateDirectory  :=Obj
OutDir                 :=$(IntermediateDirectory)
User                   :=yfa1
Date                   :=05/07/2019
CDKPath                :=C:/Program Files (x86)/C-Sky/C-Sky Development Kit
LinkerName             :=csky-abiv2-elf-gcc
LinkerNameoption       :=
SIZE                   :=csky-abiv2-elf-size
CHECKSUM               :=crc32
SharedObjectLinkerName :=
ObjectSuffix           :=.o
DependSuffix           :=.d
PreprocessSuffix       :=.i
DisassemSuffix         :=.asm
IHexSuffix             :=.ihex
ExeSuffix              :=.elf
LibSuffix              :=.a
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
UnPreprocessorSwitch   :=-U
SourceSwitch           :=-c 
ObjdumpSwitch          :=-S
ObjcopySwitch          :=-O ihex
OutputFile             :=ASM32S003_Release
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="MXQ.txt"
MakeDirCommand         :=mkdir
LinkOptions            :=-mcpu=ck802  -nostartfiles -Wl,--gc-sections  -T"$(ProjectPath)/ckcpu.ld"
IncludeCPath           :=$(IncludeSwitch)"$(ProjectPath)" $(IncludeSwitch)"$(ProjectPath)//TK_Parameter" $(IncludeSwitch)"$(ProjectPath)/include" 
IncludeAPath           :=$(IncludeSwitch)"$(ProjectPath)" -Wa,$(IncludeSwitch)"$(ProjectPath)" $(IncludeSwitch)"$(ProjectPath)//TK_Parameter" -Wa,$(IncludeSwitch)"$(ProjectPath)//TK_Parameter" $(IncludeSwitch)"$(ProjectPath)include" -Wa,$(IncludeSwitch)"$(ProjectPath)include" 
Libs                   :=$(LibrarySwitch)_S003TKey_V1_20 
ArLibs                 := "lib_S003TKey_V1_20" 
LibPath                :=$(LibraryPathSwitch)"$(ProjectPath)" 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       :=csky-abiv2-elf-ar rcu
CXX      :=csky-abiv2-elf-g++
CC       :=csky-abiv2-elf-gcc
AS       :=csky-abiv2-elf-gcc
OBJDUMP  :=csky-abiv2-elf-objdump
OBJCOPY  :=csky-abiv2-elf-objcopy
CXXFLAGS :=-mcpu=ck802    -O2  -g3  -Wall -mistack 
CFLAGS   :=-mcpu=ck802    -O2  -g3  -Wall -mistack 
ASFLAGS  :=-mcpu=ck802    -Wa,--gdwarf2    


Objects0=$(IntermediateDirectory)/arch_crt0$(ObjectSuffix) $(IntermediateDirectory)/arch_asm32s003_mem_init$(ObjectSuffix) $(IntermediateDirectory)/main$(ObjectSuffix) $(IntermediateDirectory)/asm32s003_initial$(ObjectSuffix) $(IntermediateDirectory)/asm32s003_interrupt$(ObjectSuffix) $(IntermediateDirectory)/FWlib_asm32s003$(ObjectSuffix) $(IntermediateDirectory)/FWlib_asm32s003_adc$(ObjectSuffix) $(IntermediateDirectory)/FWlib_asm32s003_ck802$(ObjectSuffix) $(IntermediateDirectory)/FWlib_asm32s003_coret$(ObjectSuffix) $(IntermediateDirectory)/FWlib_asm32s003_counta$(ObjectSuffix) \
	$(IntermediateDirectory)/FWlib_asm32s003_gpio$(ObjectSuffix) $(IntermediateDirectory)/FWlib_asm32s003_gtc$(ObjectSuffix) $(IntermediateDirectory)/FWlib_asm32s003_i2c$(ObjectSuffix) $(IntermediateDirectory)/FWlib_asm32s003_ifc$(ObjectSuffix) $(IntermediateDirectory)/FWlib_asm32s003_led$(ObjectSuffix) $(IntermediateDirectory)/FWlib_asm32s003_syscon$(ObjectSuffix) $(IntermediateDirectory)/FWlib_asm32s003_uart$(ObjectSuffix) $(IntermediateDirectory)/FWlib_asm32s003_pwm$(ObjectSuffix) $(IntermediateDirectory)/FWlib_timeout_api$(ObjectSuffix) $(IntermediateDirectory)/key_coding_key$(ObjectSuffix) \
	$(IntermediateDirectory)/key_gpio_key$(ObjectSuffix) $(IntermediateDirectory)/key_key$(ObjectSuffix) $(IntermediateDirectory)/key_power_key$(ObjectSuffix) $(IntermediateDirectory)/__rt_entry$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PostBuild MakeIntermediateDirs
all: $(IntermediateDirectory)/$(OutputFile)

$(IntermediateDirectory)/$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	@echo linking...
	@$(LinkerName) $(OutputSwitch)"$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix)" $(LinkerNameoption) @$(ObjectsFileList) $(LibraryPathSwitch)F:/Soft_sdk/ASM32S003/MXQ/Lib $(LibrarySwitch)_S003TKey_V1_20   $(LibPath) $(Libs) $(LinkOptions)
	@echo generating hex file...
	@$(OBJCOPY) $(ObjcopySwitch) "$(ProjectPath)/$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix)"  "$(ProjectPath)/Obj/$(OutputFile)$(IHexSuffix)" 
	@echo size of target:
	@$(SIZE) "$(ProjectPath)$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix)" 
	@echo -n "checksum value of target:  "
	@$(CHECKSUM) "$(ProjectPath)/$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix)" 
	
MakeIntermediateDirs:
	@test -d Obj || $(MakeDirCommand) Obj

	@test -d Lst || $(MakeDirCommand) Lst

$(IntermediateDirectory)/.d:
	@test -d Obj || $(MakeDirCommand) Obj
	@test -d Lst || $(MakeDirCommand) Lst


PreBuild:


##
## Objects
##
$(IntermediateDirectory)/arch_crt0$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/arch/crt0.S  
	@echo assembling crt0.S...
	@$(AS) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/arch/crt0.S" $(ASFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/arch_crt0$(ObjectSuffix) $(IncludeAPath)
	@$(CC) $(CFLAGS) $(IncludeAPath) -MG -MP -MT$(IntermediateDirectory)/arch_crt0$(ObjectSuffix) -MF$(IntermediateDirectory)/arch_crt0$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/arch/crt0.S"

Lst/arch_crt0$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/arch/crt0.S
	@echo generating preprocess file of crt0.S...
	@$(CC) $(CFLAGS)$(IncludeAPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/arch_crt0$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/arch/crt0.S"

$(IntermediateDirectory)/arch_asm32s003_mem_init$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/arch/asm32s003_mem_init.c  
	@echo compiling asm32s003_mem_init.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/arch/asm32s003_mem_init.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/arch_asm32s003_mem_init$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/arch_asm32s003_mem_init$(ObjectSuffix) -MF$(IntermediateDirectory)/arch_asm32s003_mem_init$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/arch/asm32s003_mem_init.c"

Lst/arch_asm32s003_mem_init$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/arch/asm32s003_mem_init.c
	@echo generating preprocess file of asm32s003_mem_init.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/arch_asm32s003_mem_init$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/arch/asm32s003_mem_init.c"

$(IntermediateDirectory)/main$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/main.c  
	@echo compiling main.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/main.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/main$(ObjectSuffix) -MF$(IntermediateDirectory)/main$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/main.c"

Lst/main$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/main.c
	@echo generating preprocess file of main.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/main$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/main.c"

$(IntermediateDirectory)/asm32s003_initial$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/asm32s003_initial.c  
	@echo compiling asm32s003_initial.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/asm32s003_initial.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/asm32s003_initial$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/asm32s003_initial$(ObjectSuffix) -MF$(IntermediateDirectory)/asm32s003_initial$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/asm32s003_initial.c"

Lst/asm32s003_initial$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/asm32s003_initial.c
	@echo generating preprocess file of asm32s003_initial.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/asm32s003_initial$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/asm32s003_initial.c"

$(IntermediateDirectory)/asm32s003_interrupt$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/asm32s003_interrupt.c  
	@echo compiling asm32s003_interrupt.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/asm32s003_interrupt.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/asm32s003_interrupt$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/asm32s003_interrupt$(ObjectSuffix) -MF$(IntermediateDirectory)/asm32s003_interrupt$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/asm32s003_interrupt.c"

Lst/asm32s003_interrupt$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/asm32s003_interrupt.c
	@echo generating preprocess file of asm32s003_interrupt.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/asm32s003_interrupt$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/asm32s003_interrupt.c"

$(IntermediateDirectory)/FWlib_asm32s003$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003.c  
	@echo compiling asm32s003.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003.c"

Lst/FWlib_asm32s003$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003.c
	@echo generating preprocess file of asm32s003.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003.c"

$(IntermediateDirectory)/FWlib_asm32s003_adc$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_adc.c  
	@echo compiling asm32s003_adc.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_adc.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003_adc$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003_adc$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003_adc$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_adc.c"

Lst/FWlib_asm32s003_adc$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_adc.c
	@echo generating preprocess file of asm32s003_adc.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003_adc$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_adc.c"

$(IntermediateDirectory)/FWlib_asm32s003_ck802$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_ck802.c  
	@echo compiling asm32s003_ck802.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_ck802.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003_ck802$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003_ck802$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003_ck802$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_ck802.c"

Lst/FWlib_asm32s003_ck802$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_ck802.c
	@echo generating preprocess file of asm32s003_ck802.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003_ck802$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_ck802.c"

$(IntermediateDirectory)/FWlib_asm32s003_coret$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_coret.c  
	@echo compiling asm32s003_coret.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_coret.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003_coret$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003_coret$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003_coret$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_coret.c"

Lst/FWlib_asm32s003_coret$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_coret.c
	@echo generating preprocess file of asm32s003_coret.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003_coret$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_coret.c"

$(IntermediateDirectory)/FWlib_asm32s003_counta$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_counta.c  
	@echo compiling asm32s003_counta.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_counta.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003_counta$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003_counta$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003_counta$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_counta.c"

Lst/FWlib_asm32s003_counta$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_counta.c
	@echo generating preprocess file of asm32s003_counta.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003_counta$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_counta.c"

$(IntermediateDirectory)/FWlib_asm32s003_gpio$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_gpio.c  
	@echo compiling asm32s003_gpio.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_gpio.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003_gpio$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003_gpio$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003_gpio$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_gpio.c"

Lst/FWlib_asm32s003_gpio$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_gpio.c
	@echo generating preprocess file of asm32s003_gpio.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003_gpio$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_gpio.c"

$(IntermediateDirectory)/FWlib_asm32s003_gtc$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_gtc.c  
	@echo compiling asm32s003_gtc.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_gtc.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003_gtc$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003_gtc$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003_gtc$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_gtc.c"

Lst/FWlib_asm32s003_gtc$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_gtc.c
	@echo generating preprocess file of asm32s003_gtc.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003_gtc$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_gtc.c"

$(IntermediateDirectory)/FWlib_asm32s003_i2c$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_i2c.c  
	@echo compiling asm32s003_i2c.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_i2c.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003_i2c$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003_i2c$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003_i2c$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_i2c.c"

Lst/FWlib_asm32s003_i2c$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_i2c.c
	@echo generating preprocess file of asm32s003_i2c.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003_i2c$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_i2c.c"

$(IntermediateDirectory)/FWlib_asm32s003_ifc$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_ifc.c  
	@echo compiling asm32s003_ifc.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_ifc.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003_ifc$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003_ifc$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003_ifc$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_ifc.c"

Lst/FWlib_asm32s003_ifc$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_ifc.c
	@echo generating preprocess file of asm32s003_ifc.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003_ifc$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_ifc.c"

$(IntermediateDirectory)/FWlib_asm32s003_led$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_led.c  
	@echo compiling asm32s003_led.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_led.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003_led$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003_led$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003_led$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_led.c"

Lst/FWlib_asm32s003_led$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_led.c
	@echo generating preprocess file of asm32s003_led.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003_led$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_led.c"

$(IntermediateDirectory)/FWlib_asm32s003_syscon$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_syscon.c  
	@echo compiling asm32s003_syscon.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_syscon.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003_syscon$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003_syscon$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003_syscon$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_syscon.c"

Lst/FWlib_asm32s003_syscon$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_syscon.c
	@echo generating preprocess file of asm32s003_syscon.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003_syscon$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_syscon.c"

$(IntermediateDirectory)/FWlib_asm32s003_uart$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_uart.c  
	@echo compiling asm32s003_uart.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_uart.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003_uart$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003_uart$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003_uart$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_uart.c"

Lst/FWlib_asm32s003_uart$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_uart.c
	@echo generating preprocess file of asm32s003_uart.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003_uart$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_uart.c"

$(IntermediateDirectory)/FWlib_asm32s003_pwm$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_pwm.c  
	@echo compiling asm32s003_pwm.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_pwm.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_asm32s003_pwm$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_asm32s003_pwm$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_asm32s003_pwm$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_pwm.c"

Lst/FWlib_asm32s003_pwm$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_pwm.c
	@echo generating preprocess file of asm32s003_pwm.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_asm32s003_pwm$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/asm32s003_pwm.c"

$(IntermediateDirectory)/FWlib_timeout_api$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/timeout_api.c  
	@echo compiling timeout_api.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/timeout_api.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FWlib_timeout_api$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/FWlib_timeout_api$(ObjectSuffix) -MF$(IntermediateDirectory)/FWlib_timeout_api$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/FWlib/timeout_api.c"

Lst/FWlib_timeout_api$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/FWlib/timeout_api.c
	@echo generating preprocess file of timeout_api.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FWlib_timeout_api$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/FWlib/timeout_api.c"

$(IntermediateDirectory)/key_coding_key$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/key/coding_key.c  
	@echo compiling coding_key.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/key/coding_key.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/key_coding_key$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/key_coding_key$(ObjectSuffix) -MF$(IntermediateDirectory)/key_coding_key$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/key/coding_key.c"

Lst/key_coding_key$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/key/coding_key.c
	@echo generating preprocess file of coding_key.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/key_coding_key$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/key/coding_key.c"

$(IntermediateDirectory)/key_gpio_key$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/key/gpio_key.c  
	@echo compiling gpio_key.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/key/gpio_key.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/key_gpio_key$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/key_gpio_key$(ObjectSuffix) -MF$(IntermediateDirectory)/key_gpio_key$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/key/gpio_key.c"

Lst/key_gpio_key$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/key/gpio_key.c
	@echo generating preprocess file of gpio_key.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/key_gpio_key$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/key/gpio_key.c"

$(IntermediateDirectory)/key_key$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/key/key.c  
	@echo compiling key.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/key/key.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/key_key$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/key_key$(ObjectSuffix) -MF$(IntermediateDirectory)/key_key$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/key/key.c"

Lst/key_key$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/key/key.c
	@echo generating preprocess file of key.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/key_key$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/key/key.c"

$(IntermediateDirectory)/key_power_key$(ObjectSuffix): F:/Soft_sdk/ASM32S003/MXQ/key/power_key.c  
	@echo compiling power_key.c...
	@$(CC) $(SourceSwitch) "F:/Soft_sdk/ASM32S003/MXQ/key/power_key.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/key_power_key$(ObjectSuffix) $(IncludeCPath)
	@$(CC) $(CFLAGS) $(IncludeCPath) -MG -MP -MT$(IntermediateDirectory)/key_power_key$(ObjectSuffix) -MF$(IntermediateDirectory)/key_power_key$(DependSuffix) -MM "F:/Soft_sdk/ASM32S003/MXQ/key/power_key.c"

Lst/key_power_key$(PreprocessSuffix): F:/Soft_sdk/ASM32S003/MXQ/key/power_key.c
	@echo generating preprocess file of power_key.c...
	@$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/key_power_key$(PreprocessSuffix) "F:/Soft_sdk/ASM32S003/MXQ/key/power_key.c"


$(IntermediateDirectory)/__rt_entry$(ObjectSuffix): $(IntermediateDirectory)/__rt_entry$(DependSuffix)
	@$(AS) $(SourceSwitch) "$(ProjectPath)/$(IntermediateDirectory)/__rt_entry.S" $(ASFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/__rt_entry$(ObjectSuffix) $(IncludeAPath)
	@rm -f $(IntermediateDirectory)/__rt_entry.S
$(IntermediateDirectory)/__rt_entry$(DependSuffix):
	@$(CC) $(CFLAGS) $(IncludeAPath) -MG -MP -MT$(IntermediateDirectory)/__rt_entry$(ObjectSuffix) -MF$(IntermediateDirectory)/__rt_entry$(DependSuffix) -MM "$(ProjectPath)/$(IntermediateDirectory)/__rt_entry.S"

-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	@echo Cleaning target...
	@rm -rf $(IntermediateDirectory)/ Lst/

clean_internal:
	@rm -rf $(IntermediateDirectory)/*.o $(IntermediateDirectory)/.d $(IntermediateDirectory)/*.d $(IntermediateDirectory)/*.a $(IntermediateDirectory)/*.elf $(IntermediateDirectory)/*.ihex Lst/

