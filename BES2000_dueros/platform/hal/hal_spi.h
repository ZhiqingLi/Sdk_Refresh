#ifndef __HAL_SPI_H__
#define __HAL_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

struct HAL_SPI_CTRL_T {
    uint32_t sspcr0_tx;
    uint32_t sspcr0_rx;
    uint16_t sspcr1;
    uint16_t sspcpsr;
    uint16_t sspdmacr;
    uint16_t ssprxcr_tx;
    uint16_t ssprxcr_rx;
};

struct HAL_SPI_CFG_T {
    uint32_t rate;
    bool clk_delay_half :1;
    bool clk_polarity :1;
    bool slave :1;
    bool dma_rx :1;
    bool dma_tx :1;
    bool rx_sep_line :1;
    uint8_t cs;
    uint8_t tx_bits;
    uint8_t rx_bits;
    uint8_t rx_frame_bits;
};

typedef void (*HAL_SPI_DMA_HANDLER_T)(int error);

//------------------------------------------------------------
// SPI ROM functions
//------------------------------------------------------------

int hal_ispi_rom_open(const struct HAL_SPI_CFG_T *cfg);

void hal_ispi_rom_activate_cs(uint32_t cs);

int hal_ispi_rom_busy(void);

int hal_ispi_rom_send(const void *data, uint32_t len);

int hal_ispi_rom_recv(const void *cmd, void *data, uint32_t len);

int hal_spiphy_rom_open(const struct HAL_SPI_CFG_T *cfg);

void hal_spiphy_rom_close(void);

int hal_spiphy_rom_busy(void);

int hal_spiphy_rom_send(const void *data, uint32_t len);

int hal_spiphy_rom_recv(const void *cmd, void *data, uint32_t len);

//------------------------------------------------------------
// ISPI functions
//------------------------------------------------------------

int hal_ispi_open(const struct HAL_SPI_CFG_T *cfg);

void hal_ispi_activate_cs(uint32_t cs);

int hal_ispi_busy(void);

int hal_ispi_send(const void *data, uint32_t len);

int hal_ispi_recv(const void *cmd, void *data, uint32_t len);

int hal_ispi_dma_send(const void *data, uint32_t len, HAL_SPI_DMA_HANDLER_T handler);

int hal_ispi_dma_recv(const void *cmd, void *data, uint32_t len, HAL_SPI_DMA_HANDLER_T handler);

void hal_ispi_stop_dma_send(void);

void hal_ispi_stop_dma_recv(void);

//------------------------------------------------------------
// SPI peripheral functions
//------------------------------------------------------------

int hal_spi_open(const struct HAL_SPI_CFG_T *cfg);

void hal_spi_close(uint32_t cs);

int hal_spi_activate_cs(uint32_t cs);

int hal_spi_busy(void);

int hal_spi_send(const void *data, uint32_t len);

int hal_spi_recv(const void *cmd, void *data, uint32_t len);

int hal_spi_dma_send(const void *data, uint32_t len, HAL_SPI_DMA_HANDLER_T handler);

int hal_spi_dma_recv(const void *cmd, void *data, uint32_t len, HAL_SPI_DMA_HANDLER_T handler);

void hal_spi_stop_dma_send(void);

void hal_spi_stop_dma_recv(void);

int hal_spi_enable_and_send(const struct HAL_SPI_CTRL_T *ctrl, const void *data, uint32_t len);

int hal_spi_enable_and_recv(const struct HAL_SPI_CTRL_T *ctrl, const void *cmd, void *data, uint32_t len);

//------------------------------------------------------------
// SPI LCD functions
//------------------------------------------------------------

int hal_spilcd_open(const struct HAL_SPI_CFG_T *cfg);

void hal_spilcd_close(uint32_t cs);

int hal_spilcd_busy(void);

int hal_spilcd_send(const void *data, uint32_t len);

int hal_spilcd_recv(const void *cmd, void *data, uint32_t len);

int hal_spilcd_dma_send(const void *data, uint32_t len, HAL_SPI_DMA_HANDLER_T handler);

int hal_spilcd_dma_recv(const void *cmd, void *data, uint32_t len, HAL_SPI_DMA_HANDLER_T handler);

void hal_spilcd_stop_dma_send(void);

void hal_spilcd_stop_dma_recv(void);

int hal_spilcd_set_data_mode(void);

int hal_spilcd_set_cmd_mode(void);

//------------------------------------------------------------
// SPI PHY functions
//------------------------------------------------------------

int hal_spiphy_open(const struct HAL_SPI_CFG_T *cfg);

void hal_spiphy_close(void);

int hal_spiphy_busy(void);

int hal_spiphy_send(const void *data, uint32_t len);

int hal_spiphy_recv(const void *cmd, void *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif

