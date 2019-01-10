#ifndef __HAL_NORFLASHIP_H__
#define __HAL_NORFLASHIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

uint8_t norflaship_continuous_read_mode_bit(uint8_t mode_bit);

uint8_t norflaship_continuous_read_off(void);

uint8_t norflaship_continuous_read_on(void);

uint32_t norflaship_write_txfifo(const uint8_t *val, uint32_t len);

uint8_t norflaship_read_rxfifo_count(void);

uint8_t norflaship_read_rxfifo(void);

void norflaship_blksize(uint32_t blksize);

void norflaship_cmd_addr(uint8_t cmd, uint32_t address);

void norflaship_ext_tx_cmd(uint8_t cmd, uint32_t tx_len);

void norflaship_ext_rx_cmd(uint8_t cmd, uint32_t tx_len, uint32_t rx_len);

void norflaship_cmd_done(void);

void norflaship_rxfifo_count_wait(uint8_t cnt);

void norflaship_rxfifo_empty_wait(void);

void norflaship_busy_wait(void);

int norflaship_is_busy(void);

void norflaship_clear_fifos(void);

void norflaship_clear_rxfifo(void);

void norflaship_clear_txfifo(void);

void norflaship_div(uint32_t div);

uint32_t norflaship_get_div(void);

void norflaship_cmdquad(uint32_t v);

void norflaship_pos_neg(uint32_t v);

void norflaship_neg_phase(uint32_t v);

void norflaship_samdly(uint32_t v);

void norflaship_dual_mode(uint32_t v);

void norflaship_hold_pin(uint32_t v);

void norflaship_wpr_pin(uint32_t v);

void norflaship_quad_mode(uint32_t v);

void norflaship_dummyclc(uint32_t v);

void norflaship_dummyclcen(uint32_t v);

void norflaship_addrbyte4(uint32_t v);

void norflaship_ruen(uint32_t v);

void norflaship_rden(uint32_t v);

void norflaship_dualiocmd(uint32_t v);

void norflaship_rdcmd(uint32_t v);

void norflaship_frdcmd(uint32_t v);

void norflaship_qrdcmd(uint32_t v);


#ifdef __cplusplus
}
#endif

#endif
