#if (CHIP_FLASH_CTRL_VER == 2)

#include "plat_types.h"
#include "plat_addr_map.h"
#include "reg_norflaship_v2.h"
#include "hal_norflaship.h"
#include "task_schedule.h"

//====================================================================================
// Flash IP Operations
//====================================================================================

/*
 Supported Command List (Based on GD25Q32C):

 parameter WRSR1     = 8'h01 ;
 parameter PP        = 8'h02 ;
 parameter READ      = 8'h03 ;
 parameter WRDI      = 8'h04 ;
 parameter RDSR1     = 8'h05 ;
 parameter WREN      = 8'h06 ;
 parameter FREAD     = 8'h0B ;
 parameter WRSR3     = 8'h11 ;
 parameter RDSR3     = 8'h15 ;
 parameter SE        = 8'h20 ;
 parameter WRSR2     = 8'h31 ;
 parameter QPP       = 8'h32 ;
 parameter RDSR2     = 8'h35 ;
 parameter DOR       = 8'h3B ;
 parameter PSR       = 8'h42 ;
 parameter ESR       = 8'h44 ;
 parameter RSR       = 8'h48 ;
 parameter WEVSR     = 8'h50 ;
 parameter BE32      = 8'h52 ;
 parameter RSFDP     = 8'h5A ;
 parameter CE        = 8'h60 ;
 parameter RSTE      = 8'h66 ;
 parameter QOR       = 8'h6B ;
 parameter PES       = 8'h75 ;
 parameter SBW       = 8'h77 ;
 parameter PER       = 8'h7A ;
 parameter REMS      = 8'h90 ;
 parameter DREAD_ID  = 8'h92 ;
 parameter QREAD_ID  = 8'h94 ;
 parameter RST       = 8'h99 ;
 parameter RDID      = 8'h9F ;
 parameter HPM       = 8'hA3 ;
 parameter RDI       = 8'hAB ;
 parameter DP        = 8'hB9 ;
 parameter DIOR      = 8'hBB ;
 parameter BE64      = 8'hD8 ;
 parameter QIOWR     = 8'hE7 ;
 parameter QIOR      = 8'hEB ;
 parameter FPP       = 8'hF2 ;
*/

static struct NORFLASH_CTRL_T * const norflash = (struct NORFLASH_CTRL_T *)FLASH_CTRL_BASE;

#if 0
static const uint8_t line_mode[8] = {
    NEW_CMD_LINE_1X, NEW_CMD_LINE_1X, NEW_CMD_LINE_2X, NEW_CMD_LINE_1X,
    NEW_CMD_LINE_4X, NEW_CMD_LINE_1X, NEW_CMD_LINE_1X, NEW_CMD_LINE_1X,
};
#endif

uint8_t norflaship_continuous_read_mode_bit(uint8_t mode_bit)
{
    norflaship_busy_wait();
    norflash->REG_004 = SET_BITFIELD(norflash->REG_004, REG_004_MODEBIT, mode_bit);
    return 0;
}
uint8_t norflaship_continuous_read_off(void)
{
    norflaship_busy_wait();
    norflash->REG_004 &= ~REG_004_CONTINUOUS_MODE;
    return 0;
}
uint8_t norflaship_continuous_read_on(void)
{
    norflaship_busy_wait();
    norflash->REG_004 |= REG_004_CONTINUOUS_MODE;
    return 0;
}
uint8_t norflaship_read_txfifo_empty_count(void)
{
    return GET_BITFIELD(norflash->REG_00C, REG_00C_TXFIFO_EMPCNT);
}
uint32_t norflaship_write_txfifo(const uint8_t *val, uint32_t len)
{
    uint32_t avail = norflaship_read_txfifo_empty_count();
    while (len > 0 && avail > 0) {
#if 0
        if (len >= 4 && ((uint32_t)val & 3) == 0) {
            norflash->REG_008.TXWORD = *(uint32_t *)val;
            val += 4;
            len -= 4;
        } else if (len >= 2 && ((uint32_t)val & 1) == 0) {
            norflash->REG_008.TXHALFWORD = *(uint16_t *)val;
            val += 2;
            len -= 2;
        } else
#endif
        {
            norflash->REG_008.TXBYTE = *(uint8_t *)val;
            val += 1;
            len -= 1;
        }
        avail -= 1;
    }
    return len;
}
uint8_t norflaship_read_rxfifo_count(void)
{
    return GET_BITFIELD(norflash->REG_00C, REG_00C_RXFIFO_COUNT);
}
uint8_t norflaship_read_rxfifo(void)
{
    return norflash->REG_010;
}
void norflaship_blksize(uint32_t blksize)
{
    norflash->REG_004 = SET_BITFIELD(norflash->REG_004, REG_004_BLOCK_SIZE, blksize);
}
void norflaship_cmd_addr(uint8_t cmd, uint32_t address)
{
    norflaship_busy_wait();
    norflash->REG_004 &= ~REG_004_NEW_CMD_EN;
    norflash->REG_000 = REG_000_CMD(cmd) | REG_000_ADDR(address);
}
void norflaship_ext_tx_cmd(uint8_t cmd, uint32_t tx_len)
{
    norflaship_busy_wait();
    norflash->REG_004 = SET_BITFIELD(norflash->REG_004, REG_004_BLOCK_SIZE, tx_len) | REG_004_NEW_CMD_EN;
    norflash->REG_000 = REG_000_CMD(cmd);
}
void norflaship_ext_rx_cmd(uint8_t cmd, uint32_t tx_len, uint32_t rx_len)
{
    norflaship_busy_wait();
    norflash->REG_004 = SET_BITFIELD(norflash->REG_004, REG_004_BLOCK_SIZE, tx_len) | REG_004_NEW_CMD_EN;
    norflash->REG_000 = REG_000_CMD(cmd) | REG_000_NEW_CMD_RX_EN | REG_000_NEW_CMD_RX_LEN(rx_len);
}
void norflaship_cmd_done(void)
{
    norflaship_busy_wait();
    norflash->REG_004 &= ~REG_004_NEW_CMD_EN;
}
void norflaship_rxfifo_count_wait(uint8_t cnt)
{
    while ((norflaship_read_rxfifo_count() < cnt) && TASK_SCHEDULE);
}
void norflaship_rxfifo_empty_wait(void)
{
    while ((norflash->REG_00C & REG_00C_RXFIFO_EMPTY) && TASK_SCHEDULE);
}
int norflaship_txfifo_is_full(void)
{
    return !!(norflash->REG_00C & REG_00C_TXFIFO_FULL);
}
void norflaship_busy_wait(void)
{
    while ((norflash->REG_00C & REG_00C_BUSY) && TASK_SCHEDULE);
}
int norflaship_is_busy(void)
{
    return !!(norflash->REG_00C & REG_00C_BUSY);
}
void norflaship_clear_fifos(void)
{
    norflaship_busy_wait();
    norflash->REG_018 = REG_018_TXFIFOCLR | REG_018_RXFIFOCLR;
    norflaship_busy_wait();
}
void norflaship_clear_rxfifo(void)
{
    norflaship_busy_wait();
    norflash->REG_018 = REG_018_RXFIFOCLR;
    norflaship_busy_wait();
}
void norflaship_clear_txfifo(void)
{
    norflaship_busy_wait();
    norflash->REG_018 = REG_018_TXFIFOCLR;
    norflaship_busy_wait();
}
void norflaship_div(uint32_t div)
{
    norflaship_busy_wait();
    norflash->REG_014 = SET_BITFIELD(norflash->REG_014, REG_014_CLKDIV, div);
}
uint32_t norflaship_get_div(void)
{
    return GET_BITFIELD(norflash->REG_014, REG_014_CLKDIV);
}
void norflaship_cmdquad(uint32_t v)
{
    norflaship_busy_wait();
    if (v) {
        norflash->REG_014 |= REG_014_CMDQUAD;
    } else {
        norflash->REG_014 &= ~REG_014_CMDQUAD;
    }
}
#if 0
void norflaship_pos_neg(uint32_t v)
{
}
void norflaship_neg_phase(uint32_t v)
{
}
#endif
void norflaship_samdly(uint32_t v)
{
    norflaship_busy_wait();
    norflash->REG_014 = SET_BITFIELD(norflash->REG_014, REG_014_SAMPLESEL, v);
}
void norflaship_dual_mode(uint32_t v)
{
    norflaship_busy_wait();
    if (v) {
        norflash->REG_014 |= REG_014_RAM_DUALMODE;
    } else {
        norflash->REG_014 &= ~REG_014_RAM_DUALMODE;
    }
}
void norflaship_hold_pin(uint32_t v)
{
    if (v) {
        norflash->REG_014 |= REG_014_HOLDPIN;
    } else {
        norflash->REG_014 &= ~REG_014_HOLDPIN;
    }
}
void norflaship_wpr_pin(uint32_t v)
{
    if (v) {
        norflash->REG_014 |= REG_014_WPROPIN;
    } else {
        norflash->REG_014 &= ~REG_014_WPROPIN;
    }
}
void norflaship_quad_mode(uint32_t v)
{
    norflaship_busy_wait();
    if (v) {
        norflash->REG_014 |= REG_014_RAM_QUADMODE;
    } else {
        norflash->REG_014 &= ~REG_014_RAM_QUADMODE;
    }
}
void norflaship_dummyclc(uint32_t v)
{
    norflash->REG_014 = SET_BITFIELD(norflash->REG_014, REG_014_DUMMY_CYCLE, v);
}
void norflaship_dummyclcen(uint32_t v)
{
    if (v) {
        norflash->REG_014 |= REG_014_DUMMY_CYCLE_EN;
    } else {
        norflash->REG_014 &= ~REG_014_DUMMY_CYCLE_EN;
    }
}
void norflaship_addrbyte4(uint32_t v)
{
    if (v) {
        norflash->REG_014 |= REG_014_FOUR_BYTE_ADDR_EN;
    } else {
        norflash->REG_014 &= ~REG_014_FOUR_BYTE_ADDR_EN;
    }
}
void norflaship_ruen(uint32_t v)
{
    norflash->REG_034 = SET_BITFIELD(norflash->REG_034, REG_034_SPI_RUEN, v);
}
void norflaship_rden(uint32_t v)
{
    norflash->REG_034 = SET_BITFIELD(norflash->REG_034, REG_034_SPI_RDEN, v);
}
void norflaship_dualiocmd(uint32_t v)
{
    norflash->REG_020 = SET_BITFIELD(norflash->REG_020, REG_020_DUALCMD, v);
}
void norflaship_rdcmd(uint32_t v)
{
    norflash->REG_020 = SET_BITFIELD(norflash->REG_020, REG_020_READCMD, v);
}
void norflaship_frdcmd(uint32_t v)
{
    norflash->REG_020 = SET_BITFIELD(norflash->REG_020, REG_020_FREADCMD, v);
}
void norflaship_qrdcmd(uint32_t v)
{
    norflash->REG_020 = SET_BITFIELD(norflash->REG_020, REG_020_QUADCMD, v);
}

#endif
