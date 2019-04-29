############################################################################
#
#             Copyright Mentor Graphics Corporation 2004
#                All Rights Reserved.
#
#    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION
#  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS
#  LICENSORS AND IS SUBJECT TO LICENSE TERMS.
#
# Dependencies for UCD
#
# $Revision: 1.3 $
#############################################################################

###############################################################################
# Dependencies
###############################################################################

PUBLIC_HEADERS = \
    $(MUSB_DIR)/include/mu_arch.h \
    $(MUSB_DIR)/include/mu_cdi.h \
    $(MUSB_DIR)/include/mu_dev.h \
    $(MUSB_DIR)/include/mu_dsi.h \
    $(MUSB_DIR)/include/mu_list.h \
    $(MUSB_DIR)/include/mu_pkoff.h \
    $(MUSB_DIR)/include/mu_pkon.h \
    $(MUSB_DIR)/include/mu_stat.h \
    $(MUSB_DIR)/include/mu_tools.h \
    
CD_HEADERS = \
    $(CD_SRC_DIR)/mu_drcdf.h \
    $(CD_SRC_DIR)/mu_drcpr.h \
    $(CD_SRC_DIR)/mu_fdrdf.h \
    $(CD_SRC_DIR)/mu_fdrpr.h \
    $(CD_SRC_DIR)/mu_funpr.h \
    $(CD_SRC_DIR)/mu_hdrdf.h \
    $(CD_SRC_DIR)/mu_hdrpr.h \
    $(CD_SRC_DIR)/mu_hstpr.h \
    $(CD_SRC_DIR)/mu_mdrdf.h \
    $(CD_SRC_DIR)/mu_mdrpr.h \
    $(CD_SRC_DIR)/mu_pippr.h \

# since just a few files, make conservative assumption
# NOTE: it still doesn't catch everything, e.g. simulated stdint.h
    
$(CD_OBJ_DIR)/mu_cntlr.o: $(PUBLIC_HEADERS) $(CD_HEADERS)

$(CD_OBJ_DIR)/mu_drc.o: $(PUBLIC_HEADERS) $(CD_HEADERS)

$(CD_OBJ_DIR)/mu_fdr.o: $(PUBLIC_HEADERS) $(CD_HEADERS)

$(CD_OBJ_DIR)/mu_fun.o: $(PUBLIC_HEADERS) $(CD_HEADERS)

$(CD_OBJ_DIR)/mu_hdr.o: $(PUBLIC_HEADERS) $(CD_HEADERS)

$(CD_OBJ_DIR)/mu_hst.o: $(PUBLIC_HEADERS) $(CD_HEADERS)

$(CD_OBJ_DIR)/mu_list.o: $(PUBLIC_HEADERS) $(CD_HEADERS)

$(CD_OBJ_DIR)/mu_mdr.o: $(PUBLIC_HEADERS) $(CD_HEADERS)

$(CD_OBJ_DIR)/mu_pip.o: $(PUBLIC_HEADERS) $(CD_HEADERS)

