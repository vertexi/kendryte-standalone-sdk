/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "nt35310.h"
#include "sysctl.h"
#include "fpioa.h"
#include "gpiohs.h"
#include "spi.h"
#include "dmac.h"
#include "plic.h"
#include <common.h>

#define __SPI_SYSCTL(x, y)  SYSCTL_##x##_SPI##y
#define _SPI_SYSCTL(x, y)   __SPI_SYSCTL(x, y)
#define SPI_SYSCTL(x)       _SPI_SYSCTL(x, SPI_CHANNEL)
#define __SPI_SS(x, y)      FUNC_SPI##x##_SS##y
#define _SPI_SS(x, y)       __SPI_SS(x, y)
#define SPI_SS              _SPI_SS(SPI_CHANNEL, SPI_SLAVE_SELECT)
#define __SPI(x, y)         FUNC_SPI##x##_##y
#define _SPI(x, y)          __SPI(x, y)
#define SPI(x)              _SPI(SPI_CHANNEL, x)

void  init_dcx(void)
{
    gpiohs_init();
    fpioa_set_function(DCX_IO, FUNC_GPIOHS0 + DCX_GPIONUM);/*dcx*/
    gpiohs_set_drive_mode(DCX_GPIONUM, GPIO_DM_Output);
    gpiohs_set_pin_value(DCX_GPIONUM, GPIO_PV_High);
}

void set_dcx_control(void)
{
    gpiohs_set_pin_value(DCX_GPIONUM, GPIO_PV_Low);
}

void set_dcx_data(void)
{
    gpiohs_set_pin_value(DCX_GPIONUM, GPIO_PV_High);
}

void pin_mux_init(void)
{
    fpioa_set_function(31, SPI_SS);
    fpioa_set_function(32, SPI(SCLK));
    sysctl_spi0_dvp_data_set(1);
}

void tft_hard_init(void)
{
    init_dcx();
    pin_mux_init();
    spi_master_config(SPI_CHANNEL, SPI_MODE_2, SPI_FF_OCTAL, 8);
}

void tft_write_command(uint8_t cmd)
{
    set_dcx_control();
    spi_set_tmod(SPI_CHANNEL, SPI_TMOD_TRANS);
    spi_set_frame_format(SPI_CHANNEL, SPI_FF_OCTAL);
    spi_set_frame_size(SPI_CHANNEL, 8);
    spi_trans_config(SPI_CHANNEL, 8/*instrction length*/, 0/*address length*/, 0/*wait cycles*/, SPI_AITM_AS_FRAME_FORMAT/*spi address trans mode*/);
    spi_normal_send_dma(DMAC_CHANNEL0, SPI_CHANNEL, 1 << SPI_SLAVE_SELECT, (uint8_t *)(&cmd), 1,SPI_TRANS_CHAR);
}

void tft_write_byte(uint8_t *data_buf, uint32_t length)
{
    set_dcx_data();
    spi_set_tmod(SPI_CHANNEL, SPI_TMOD_TRANS);
    spi_set_frame_format(SPI_CHANNEL, SPI_FF_OCTAL);
    spi_set_frame_size(SPI_CHANNEL, 8);
    spi_trans_config(SPI_CHANNEL, 8/*instrction length*/, 0/*address length*/, 0/*wait cycles*/, SPI_AITM_AS_FRAME_FORMAT/*spi address trans mode*/);
    spi_normal_send_dma(DMAC_CHANNEL0, SPI_CHANNEL, 1 << SPI_SLAVE_SELECT, data_buf, length, SPI_TRANS_CHAR);
}

void tft_write_half(uint16_t *data_buf, uint32_t length)
{
    set_dcx_data();
    spi_set_tmod(SPI_CHANNEL, SPI_TMOD_TRANS);
    spi_set_frame_format(SPI_CHANNEL, SPI_FF_OCTAL);
    spi_set_frame_size(SPI_CHANNEL, 16);
    spi_trans_config(SPI_CHANNEL, 16/*instrction length*/, 0/*address length*/, 0/*wait cycles*/, SPI_AITM_AS_FRAME_FORMAT/*spi address trans mode*/);
    spi_normal_send_dma(DMAC_CHANNEL0, SPI_CHANNEL, 1 << SPI_SLAVE_SELECT,data_buf, length, SPI_TRANS_SHORT);
}

void tft_write_word(uint32_t *data_buf, uint32_t length, uint32_t flag)
{
    set_dcx_data();
    spi_set_tmod(SPI_CHANNEL, SPI_TMOD_TRANS);
    spi_set_frame_format(SPI_CHANNEL, SPI_FF_OCTAL);
    spi_set_frame_size(SPI_CHANNEL, 32);
    spi_trans_config(SPI_CHANNEL, 0/*instrction length*/, 32/*address length*/, 0/*wait cycles*/, SPI_AITM_AS_FRAME_FORMAT/*spi address trans mode*/);
    spi_normal_send_dma(DMAC_CHANNEL0, SPI_CHANNEL, 1 << SPI_SLAVE_SELECT,data_buf, length, SPI_TRANS_INT);
}

void tft_fill_data(uint32_t *data_buf, uint32_t length)
{
    set_dcx_data();
    spi_set_tmod(SPI_CHANNEL, SPI_TMOD_TRANS);
    spi_set_frame_format(SPI_CHANNEL, SPI_FF_OCTAL);
    spi_set_frame_size(SPI_CHANNEL, 32);
    spi_trans_config(SPI_CHANNEL, 0/*instrction length*/, 32/*address length*/, 0/*wait cycles*/, SPI_AITM_AS_FRAME_FORMAT/*spi address trans mode*/);
    spi_fill_dma(DMAC_CHANNEL0, SPI_CHANNEL, 1 << SPI_SLAVE_SELECT,data_buf, length);
}

