/* mbed Microcontroller Library
 * Copyright (c) 2006-2012 ARM Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* Introduction
 * ------------
 * SD and MMC cards support a number of interfaces, but common to them all
 * is one based on SPI. This is the one I'm implmenting because it means
 * it is much more portable even though not so performant, and we already
 * have the mbed SPI Interface!
 *
 * The main reference I'm using is Chapter 7, "SPI Mode" of:
 *  http://www.sdcard.org/developers/tech/sdcard/pls/Simplified_Physical_Layer_Spec.pdf
 *
 */
#include "SDFileSystem.h"
#include "mbed_debug.h"
#include "cmsis_os.h"

#include "hal_sdmmc.h"
#include "hal_trace.h"
#include "hal_iomux.h"

struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux____[] = {
    {HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SDMMC_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    {HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SDMMC_CMD, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_SDMMC_DATA0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_SDMMC_DATA1, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_SDMMC_DATA2, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
    {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_SDMMC_DATA3, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_PULLUP_ENALBE},
};


void iomux_sdmmc(void)
{
	hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)pinmux____, sizeof(pinmux____)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
}

SDFileSystem::SDFileSystem(const char* name) :
    FATFileSystem(name), _is_initialized(0) {
}

SDFileSystem::~SDFileSystem(){
	hal_sdmmc_close(HAL_SDMMC_ID_0);
}

int SDFileSystem::disk_initialize() {

    if (_is_initialized)
        return 0;

    iomux_sdmmc();
    hal_sdmmc_set_delay_func((HAL_SDMMC_DELAY_FUNC)osDelay);
    _is_initialized = hal_sdmmc_open(HAL_SDMMC_ID_0)==0?1:0;
    if (_is_initialized == 0) {
        return 1;
    }
    _sectors = _sd_sectors();
    _is_initialized = 1;

    return 0;
}

int SDFileSystem::disk_write(const uint8_t* buffer, uint64_t block_number, uint8_t count) {
    hal_sdmmc_write_blocks(HAL_SDMMC_ID_0, block_number, count, (uint8_t *)buffer);
    return 0;
}

int SDFileSystem::disk_read(uint8_t* buffer, uint64_t block_number, uint8_t count) {
    hal_sdmmc_read_blocks(HAL_SDMMC_ID_0, block_number, count, (uint8_t *)buffer);
    return 0;
}

int SDFileSystem::disk_status() {
    // FATFileSystem::disk_status() returns 0 when initialized
    if (_is_initialized) {
        return 0;
    } else {
        return 1;
    }
}

int SDFileSystem::disk_sync() { return 0; }
uint64_t SDFileSystem::disk_sectors() { return _sectors; }

uint64_t SDFileSystem::_sd_sectors() {
    uint32_t sector_size = 0, sector_count  = 0;
    hal_sdmmc_info(HAL_SDMMC_ID_0, &sector_count, &sector_size);

    return sector_count;
}
