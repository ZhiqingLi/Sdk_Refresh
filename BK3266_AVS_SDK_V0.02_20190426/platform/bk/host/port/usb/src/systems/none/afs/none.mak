############################################################################
#
#             Copyright Mentor Graphics Corporation 2004
#                All Rights Reserved.
#
#    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION
#  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS
#  LICENSORS AND IS SUBJECT TO LICENSE TERMS.
#
# $Revision: 1.6 $
############################################################################

PCI_DIR = $(AFS_DIR)/Source/PCI
UHAL_DIR = $(AFS_DIR)/Source/uHAL

PCI_BIN_DIR = $(AFS_DIR)/Source/PCI/Build/$(AFS_TARGET)/$(AFS_MODE)
UHAL_BIN_DIR  = $(AFS_DIR)/Source/uHAL/Build/$(AFS_TARGET)/$(AFS_MODE)

UCD_INCLUDE  = $(CINCLUDE)../../include $(CINCLUDE)$(OS_SRC_DIR)
PCI_INCLUDE  = $(CINCLUDE)$(PCI_DIR)/Sources
UHAL_INCLUDE = $(CINCLUDE)$(UHAL_DIR)/h $(CINCLUDE)$(UHAL_DIR)/Boards/$(AFS_BOARD) $(CINCLUDE)$(UHAL_DIR)/Processors
ALL_INCLUDE  = $(PCI_INCLUDE) $(UHAL_INCLUDE)
CFLAGS += $(ALL_INCLUDE)

LDFLAGS_TAIL += $(UHAL_BIN_DIR)/uHALlibrary$(LIB) $(PCI_BIN_DIR)/PCIlibrary$(LIB)

