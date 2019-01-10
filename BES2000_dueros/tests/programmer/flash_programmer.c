#include "stdint.h"
#include "stdbool.h"
#include "plat_types.h"
#include "string.h"
#include "stdio.h"
#ifdef _WIN32
#include "crc32.h"
#else
#include "export_fn_rom.h"
#endif
#include "hexdump.h"
#include "tool_msg.h"
#include "sys_api_programmer.h"
#include "flash_programmer.h"

static int send_burn_data_reply(enum ERR_CODE code, unsigned short sec_seq, unsigned char seq);

//======================================================================================================

#define FLASH_PROGRAMMER_VERSION        0x0100

#define SLIDE_WIN_NUM                   2

#define BULK_READ_STEP_SIZE             0x1000

#ifdef _WIN32
#define BURN_BUFFER_LOC
#define CRC32_FUNC(crc, buf, len)       crc32(crc, buf, len)
#else
#define BURN_BUFFER_LOC                 __attribute__((section(".burn_buffer")))
#define CRC32_FUNC(crc, buf, len)       __export_fn_rom.crc32(crc, buf, len)
#endif

enum PROGRAMMER_STATE {
    PROGRAMMER_NONE,
    PROGRAMMER_ERASE_BURN_START,
    PROGRAMMER_BURN_START,
};

enum DATA_BUF_STATE {
    DATA_BUF_FREE,
    DATA_BUF_RECV,
    DATA_BUF_BURN,
    DATA_BUF_DONE,
};

enum BURN_CMD_TYPE {
    BURN_CMD_ERASE_SECTOR = 0x21,
    BURN_CMD_BURN_DATA = 0x22,
    BURN_CMD_ERASE_CHIP = 0x31,
};

static enum PROGRAMMER_STATE programmer_state;

static enum PARSE_STATE parse_state;
static struct message_t recv_msg;
static struct message_t send_msg = { { PREFIX_CHAR, }, };
static unsigned char send_seq = 0;

static unsigned int extra_buf_len;
static unsigned int recv_extra_timeout;

static unsigned int burn_addr;
static unsigned int burn_len;
static unsigned int sector_size;
static unsigned int sector_cnt;
static unsigned int last_sector_len;
static unsigned int cur_sector_seq;

static unsigned char BURN_BUFFER_LOC data_buf[SLIDE_WIN_NUM][(32 * 1024 + BURN_DATA_MSG_OVERHEAD + 63) / 64 * 64];
static enum DATA_BUF_STATE data_buf_state[SLIDE_WIN_NUM];
static enum ERR_CODE data_buf_errcode[SLIDE_WIN_NUM];
static unsigned int cur_data_buf_idx;

static const unsigned int size_mask = SECTOR_SIZE_32K | SECTOR_SIZE_4K;

static void POSSIBLY_UNUSED dump_buffer(unsigned char *buf, size_t len)
{
    print_hex_dump_bytes("", 2, buf, len);
}

static unsigned int count_set_bits(unsigned int i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

static void reset_data_buf_state(void)
{
    unsigned int i;

    cur_data_buf_idx = 0;
    data_buf_state[0] = DATA_BUF_RECV;
    for (i = 1; i < SLIDE_WIN_NUM; i++) {
        data_buf_state[i] = DATA_BUF_FREE;
    }
}

static void burn_data_buf(int index)
{
    data_buf_state[index] = DATA_BUF_BURN;

#ifdef _WIN32
    write_flash_data(&data_buf[index][BURN_DATA_MSG_OVERHEAD],
        data_buf[index][4] | (data_buf[index][5] << 8) |
        (data_buf[index][6] << 16) | (data_buf[index][7] << 24));

    send_burn_data_reply(ERR_BURN_OK,
        data_buf[index][12] | (data_buf[index][13] << 8),
        data_buf[index][2]);
    data_buf_state[index] = DATA_BUF_FREE;
#endif
}

static int get_free_data_buf(void)
{
    unsigned int i;
    unsigned int index;

    while (1) {
        index = cur_data_buf_idx + 1;
        if (index >= SLIDE_WIN_NUM) {
            index = 0;
        }
        for (i = 0; i < SLIDE_WIN_NUM; i++) {
            if (data_buf_state[index] == DATA_BUF_FREE) {
                data_buf_state[index] = DATA_BUF_RECV;
                cur_data_buf_idx = index;
                return 0;
            }

            if (++index >= SLIDE_WIN_NUM) {
                index = 0;
            }
        }

        if (wait_data_buf_finished()) {
            break;
        }
    }

    return 1;
}

#ifndef _WIN32
int get_burn_data_buf(unsigned int *rindex)
{
    unsigned int i;
    unsigned int index;

    index = cur_data_buf_idx + 1;
    if (index >= SLIDE_WIN_NUM) {
        index = 0;
    }
    for (i = 0; i < SLIDE_WIN_NUM; i++) {
        if (data_buf_state[index] == DATA_BUF_BURN) {
            *rindex = index;
            return 0;
        }

        if (++index >= SLIDE_WIN_NUM) {
            index = 0;
        }
    }

    return 1;
}

static void finish_data_buf(unsigned int index, enum ERR_CODE code)
{
    data_buf_errcode[index] = code;
    data_buf_state[index] = DATA_BUF_DONE;
}

int handle_data_buf(unsigned int index)
{
    unsigned int addr;
    unsigned int len;
    unsigned int sec_seq;
    //unsigned int mcrc;
    int ret;

    if (index >= SLIDE_WIN_NUM) {
        return 1;
    }

    TRACE_TIME("### FLASH_TASK: %s index=%u ###", __FUNCTION__, index);

    len = data_buf[index][4] | (data_buf[index][5] << 8) |
        (data_buf[index][6] << 16) | (data_buf[index][7] << 24);
    //mcrc = data_buf[index][8] | (data_buf[index][9] << 8) |
    //    (data_buf[index][10] << 16) | (data_buf[index][11] << 24);
    sec_seq = data_buf[index][12] | (data_buf[index][13] << 8);
    addr = burn_addr + sec_seq * sector_size;

    TRACE("### FLASH_TASK: sec_seq=%u addr=0x%08X len=%u ###", sec_seq, addr, len);

    if (programmer_state == PROGRAMMER_ERASE_BURN_START) {
        ret = erase_sector(addr, len);
        if (ret) {
            TRACE("### FLASH_TASK: ERASE_DATA failed: addr=0x%08X len=%u ###", addr, len);
            finish_data_buf(index, ERR_ERASE_FLSH);
            return 0;
        }
        TRACE_TIME("### FLASH_TASK: Erase done ###");
    }

    ret = burn_data(addr, &data_buf[index][BURN_DATA_MSG_OVERHEAD], len);
    if (ret) {
        TRACE("### FLASH_TASK: BURN_DATA failed: addr=0x%08X len=%u ###", addr, len);
        finish_data_buf(index, ERR_BURN_FLSH);
        return 0;
    }
    TRACE_TIME("### FLASH_TASK: Burn done ###");

    ret = verify_flash_data(addr, &data_buf[index][BURN_DATA_MSG_OVERHEAD], len);
    if (ret) {
        TRACE("### FLASH_TASK: VERIFY_DATA failed");
        finish_data_buf(index, ERR_VERIFY_FLSH);
        return 0;
    }
    TRACE_TIME("### FLASH_TASK: Verify done ###");

    finish_data_buf(index, ERR_BURN_OK);

    return 0;
}

int free_data_buf(void)
{
    unsigned int i;
    unsigned int index;
    unsigned int sec_seq;

    index = cur_data_buf_idx + 1;
    if (index >= SLIDE_WIN_NUM) {
        index = 0;
    }

    // Free one data buffer once
    for (i = 0; i < SLIDE_WIN_NUM; i++) {
        if (data_buf_state[index] == DATA_BUF_DONE) {
            sec_seq = data_buf[index][12] | (data_buf[index][13] << 8);
            send_burn_data_reply(data_buf_errcode[index], sec_seq, data_buf[index][2]);
            data_buf_state[index] = DATA_BUF_FREE;
            return 0;
        }

        if (++index >= SLIDE_WIN_NUM) {
            index = 0;
        }
    }

    return 1;
}

int data_buf_in_use(void)
{
    unsigned int index;

    for (index = 0; index < SLIDE_WIN_NUM; index++) {
        if (data_buf_state[index] != DATA_BUF_FREE) {
            return 1;
        }
    }

    return 0;
}
#endif

static unsigned char check_sum(unsigned char *buf, unsigned char len)
{
    int i;
    unsigned char sum = 0;

    for (i = 0; i < len; i++) {
        sum += buf[i];
    }

    return sum;
}

static int send_sector_size_msg(void)
{
    int ret;

    send_msg.hdr.type = TYPE_SECTOR_SIZE;
    send_msg.hdr.seq = send_seq++;
    send_msg.hdr.len = 6;
    send_msg.data[0] = FLASH_PROGRAMMER_VERSION & 0xFF;
    send_msg.data[1] = (FLASH_PROGRAMMER_VERSION >> 8) & 0xFF;
    send_msg.data[2] = size_mask & 0xFF;
    send_msg.data[3] = (size_mask >> 8) & 0xFF;
    send_msg.data[4] = (size_mask >> 16) & 0xFF;
    send_msg.data[5] = (size_mask >> 24) & 0xFF;
    send_msg.data[6] = ~check_sum((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg) - 1);

    ret = send_data((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg));

    return ret;
}

int send_reply(const unsigned char *payload, unsigned int len)
{
    int ret = 0;

    if (len + 1 > sizeof(send_msg.data)) {
        TRACE("Packet length too long: %u", len);
        return -1;
    }

    send_msg.hdr.type = recv_msg.hdr.type;
    send_msg.hdr.seq = recv_msg.hdr.seq;
    send_msg.hdr.len = len;
    memcpy(&send_msg.data[0], payload, len);
    send_msg.data[len] = ~check_sum((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg) - 1);

    ret = send_data((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg));

    return ret;
}

static int send_burn_data_reply(enum ERR_CODE code, unsigned short sec_seq, unsigned char seq)
{
    int ret = 0;

    if (programmer_state == PROGRAMMER_ERASE_BURN_START) {
        send_msg.hdr.type = TYPE_ERASE_BURN_DATA;
    } else if (programmer_state == PROGRAMMER_BURN_START) {
        send_msg.hdr.type = TYPE_BURN_DATA;
    } else {
        return 1;
    }

    send_msg.hdr.seq = seq;
    send_msg.hdr.len = 3;
    send_msg.data[0] = code;
    send_msg.data[1] = sec_seq & 0xFF;
    send_msg.data[2] = (sec_seq >> 8) & 0xFF;
    send_msg.data[3] = ~check_sum((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg) - 1);

    ret = send_data((unsigned char *)&send_msg, MSG_TOTAL_LEN(&send_msg));

    return ret;
}

static void reset_parse_state(unsigned char **buf, size_t *len)
{
    parse_state = PARSE_HEADER;
    memset(&recv_msg.hdr, 0, sizeof(recv_msg.hdr));

    *buf = (unsigned char *)&recv_msg.hdr;
    *len = sizeof(recv_msg.hdr);
}

static void reset_programmer_state(unsigned char **buf, size_t *len)
{
    programmer_state = PROGRAMMER_NONE;
    reset_parse_state(buf, len);
    reset_data_buf_state();
}

static enum ERR_CODE check_msg_hdr(void)
{
    enum ERR_CODE errcode = ERR_NONE;
    struct CUST_CMD_PARAM param;

    switch (recv_msg.hdr.type) {
        case TYPE_SYS:
            if (recv_msg.hdr.len != 1 && recv_msg.hdr.len != 5) {
                //TRACE("SYS msg length error: %u", recv_msg.hdr.len);
                errcode = ERR_LEN;
            }
            break;
        case TYPE_READ:
            if (recv_msg.hdr.len != 5) {
                //TRACE("READ msg length error: %u", recv_msg.hdr.len);
                errcode = ERR_LEN;
            }
            break;
        case TYPE_WRITE:
            if (recv_msg.hdr.len <= 4 || recv_msg.hdr.len > 20) {
                //TRACE("WRITE msg length error: %u", recv_msg.hdr.len);
                errcode = ERR_LEN;
            }
            break;
        case TYPE_BULK_READ:
            if (recv_msg.hdr.len != 8) {
                //TRACE("BULK_READ msg length error: %u", recv_msg.hdr.len);
                errcode = ERR_LEN;
            }
            break;
        case TYPE_ERASE_BURN_START:
        case TYPE_BURN_START:
            if (recv_msg.hdr.len != 12) {
                //TRACE("BURN_START 0x%x msg length error: %u", recv_msg.hdr.type, recv_msg.hdr.len);
                errcode = ERR_LEN;
            }
            break;
        case TYPE_ERASE_BURN_DATA:
        case TYPE_BURN_DATA:
            // BURN_DATA msgs are sent in extra msgs
            errcode = ERR_BURN_INFO_MISSING;
            break;
        case TYPE_BURN_CMD:
            if (recv_msg.hdr.len != 1 && (recv_msg.hdr.len <= 5 || recv_msg.hdr.len > 21)) {
                //TRACE("BURN_CMD msg length error: %u", recv_msg.hdr.len);
                errcode = ERR_LEN;
            }
            break;
        case TYPE_GET_SECTOR_INFO:
            if (recv_msg.hdr.len != 4) {
                //TRACE("GET_SECTOR_INFO msg length error: %u", recv_msg.hdr.len);
                errcode = ERR_LEN;
            }
            break;
        default:
            param.stage = CUST_CMD_STAGE_HEADER;
            param.msg = &recv_msg;
            errcode = handle_cust_cmd(&param);
            if (errcode == ERR_TYPE_INVALID) {
                TRACE("Invalid message type: 0x%02x", recv_msg.hdr.type);
            }
            break;
    }

    if (errcode == ERR_NONE && recv_msg.hdr.len + 1 > sizeof(recv_msg.data)) {
        errcode = ERR_LEN;
    }

    return errcode;
}

static enum ERR_CODE handle_sys_cmd(enum SYS_CMD_TYPE cmd, unsigned char *param, unsigned int len)
{
    unsigned char cret[5];
    unsigned int bootmode;

    cret[0] = ERR_NONE;

    if (cmd == SYS_CMD_SET_BOOTMODE || cmd == SYS_CMD_CLR_BOOTMODE) {
        if (len != 4) {
            TRACE("Invalid SYS CMD len %u for cmd: 0x%x", len, cmd);
            return ERR_DATA_LEN;
        }
    } else {
        if (len != 0) {
            TRACE("Invalid SYS CMD len %u for cmd: 0x%x", len, cmd);
            return ERR_DATA_LEN;
        }
    }

    switch (cmd) {
        case SYS_CMD_REBOOT: {
            TRACE("--- Reboot---");
            send_reply(cret, 1);
            system_reboot();
            break;
        }
        case SYS_CMD_SHUTDOWN: {
            TRACE("--- Shutdown ---");
            send_reply(cret, 1);
            system_shutdown();
            break;
        }
        case SYS_CMD_FLASH_BOOT: {
            TRACE("--- Flash boot ---");
            send_reply(cret, 1);
            system_flash_boot();
            break;
        }
        case SYS_CMD_SET_BOOTMODE: {
            TRACE("--- Set bootmode ---");
            memcpy(&bootmode, param, 4);
            system_set_bootmode(bootmode);
            send_reply(cret, 1);
            break;
        }
        case SYS_CMD_CLR_BOOTMODE: {
            TRACE("--- Clear bootmode ---");
            memcpy(&bootmode, param, 4);
            system_clear_bootmode(bootmode);
            send_reply(cret, 1);
            break;
        }
        case SYS_CMD_GET_BOOTMODE: {
            TRACE("--- Get bootmode ---");
            bootmode = system_get_bootmode();
            memcpy(&cret[1], &bootmode, 4);
            send_reply(cret, 5);
            break;
        }
        default: {
            TRACE("Invalid command: 0x%x", recv_msg.data[0]);
            return ERR_SYS_CMD;
        }
    }

    return ERR_NONE;
}

static enum ERR_CODE handle_read_cmd(unsigned int addr, unsigned int len)
{
    union {
            unsigned int data[4];
            unsigned char buf[19];
        } d;
    int i;
#ifndef _WIN32
    int cnt;
    unsigned short *p;
#endif

    TRACE("[READ] addr=0x%08X len=%u", addr, len);

    if (debug_read_enabled() == 0) {
        TRACE("[READ] No access right");
        return ERR_ACCESS_RIGHT;
    }
    if (len > 16) {
        TRACE("[READ] Length error: %u", len);
        return ERR_DATA_LEN;
    }

#ifdef _WIN32
    for (i = 0; i < 4; i++) {
        d.data[i] = (0x11 + i) | ((0x22 + i) << 8) | ((0x33 + i) << 16) | ((0x44 + i ) << 24);
    }
#else
    // Handle half-word and word register reading
    if ((len & 0x03) == 0 && (addr & 0x03) == 0) {
        cnt = len / 4;
        for (i = 0; i < cnt; i++) {
            d.data[i] = *((unsigned int *)addr + i);
        }
    } else if ((len & 0x01) == 0 && (addr & 0x01) == 0) {
        cnt = len / 2;
        p = (unsigned short *)&d.data[0];
        for (i = 0; i < cnt; i++) {
            p[i] = *((unsigned short *)addr + i);
        }
    } else {
        memcpy(&d.data[0], (unsigned char *)addr, len);
    }
#endif

    memmove(&d.buf[1], &d.buf[0], len);
    d.buf[0] = ERR_NONE;
    send_reply((unsigned char *)&d.buf[0], 1 + len);

    return ERR_NONE;
}

static enum ERR_CODE handle_write_cmd(unsigned int addr, unsigned int len, unsigned char *wdata)
{
    unsigned int data;
#ifdef _WIN32
    unsigned char d[16];
#else
    int i;
    int cnt;
#endif

    TRACE("[WRITE] addr=0x%08X len=%u", addr, len);

    if (debug_write_enabled() == 0) {
        TRACE("[WRITE] No access right");
        return ERR_ACCESS_RIGHT;
    }
    if (len > 16) {
        TRACE("[WRITE] Length error: %u", len);
        return ERR_DATA_LEN;
    }

#ifdef _WIN32
    memcpy(d, wdata, len);
    dump_buffer(d, len);
#else
    // Handle half-word and word register writing
    if ((len & 0x03) == 0 && (addr & 0x03) == 0) {
        cnt = len / 4;
        for (i = 0; i < cnt; i++) {
            data = wdata[4 * i] | (wdata[4 * i + 1] << 8) | (wdata[4 * i + 2] << 16) | (wdata[4 * i + 3] << 24);
            *((unsigned int *)addr + i) = data;
        }
    } else if ((len & 0x01) == 0 && (addr & 0x01) == 0) {
        cnt = len / 2;
        for (i = 0; i < cnt; i++) {
            data = wdata[2 * i] | (wdata[2 * i + 1] << 8);
            memcpy(&data, wdata + 2 * i, 2);
            *((unsigned short *)addr + i) = (unsigned short)data;
        }
    } else {
        memcpy((unsigned char *)addr, wdata, len);
    }
#endif

    data = ERR_NONE;
    send_reply((unsigned char *)&data, 1);

    return ERR_NONE;
}

static enum ERR_CODE handle_bulk_read_cmd(unsigned int addr, unsigned int len)
{
    int ret;
    unsigned int sent;
    unsigned char cret[1];

    TRACE("[BULK_READ] addr=0x%08X len=%u", addr, len);

    if (debug_read_enabled() == 0) {
        TRACE("[READ] No access right");
        return ERR_ACCESS_RIGHT;
    }

    cret[0] = ERR_NONE;
    send_reply(cret, 1);

    ret = 0;
#ifdef _WIN32
    while (ret == 0 && len > 0) {
        sent = (len > sizeof(data_buf[0])) ? sizeof(data_buf[0]) : len;
        ret = send_data(&data_buf[0][0], sent);
        len -= sent;
    }
#else
    while (ret == 0 && len > 0) {
        sent = (len > BULK_READ_STEP_SIZE) ? BULK_READ_STEP_SIZE : len;
        ret = send_data((unsigned char *)addr, sent);
        addr += sent;
        len -= sent;
    }
#endif

    if (ret) {
        TRACE("[BULK_READ] Failed to send data: %d", ret);
        // Just let the peer timeout
    }

    return ERR_NONE;
}

static enum ERR_CODE handle_burn_cmd(enum BURN_CMD_TYPE cmd, unsigned char *param, unsigned int len)
{
    int ret;
    unsigned char cret = ERR_NONE;

    switch (cmd) {
        case BURN_CMD_ERASE_SECTOR: {
            unsigned int addr;
            unsigned int size;

            TRACE("- BURN_CMD_ERASE_SECTOR -");

            if (len != 8) {
                TRACE("Invalid ERASE_SECTOR cmd param len: %u");
                return ERR_LEN;
            }

            addr = param[0] | (param[1] << 8) | (param[2] << 16) | (param[3] << 24);
            size = param[4] | (param[5] << 8) | (param[6] << 16) | (param[7] << 24);
            TRACE("addr=0x%08X size=%u", addr, size);

            ret = erase_sector(addr, size);
            if (ret) {
                TRACE("ERASE_SECTOR failed: addr=0x%08X size=%u", addr, size);
                return ERR_ERASE_FLSH;
            }
            send_reply(&cret, 1);
            break;
        }
        case BURN_CMD_BURN_DATA: {
            unsigned int addr;

            TRACE("- BURN_CMD_BURN_DATA -");

            if (len <= 4 || len > 20) {
                TRACE("Invalid BURN_DATA cmd param len: %u", len);
                return ERR_LEN;
            }

            addr = param[0] | (param[1] << 8) | (param[2] << 16) | (param[3] << 24);
            TRACE("addr=0x%08X len=%u", addr, len - 4);

            ret = burn_data(addr, &param[4], len - 4);
            if (ret) {
                TRACE("BURN_DATA failed");
                return ERR_BURN_FLSH;
            }

            ret = verify_flash_data(addr, &param[4], len - 4);
            if (ret) {
                TRACE("BURN_DATA verify failed");
                return ERR_VERIFY_FLSH;
            }
            send_reply(&cret, 1);
            break;
        }
        case BURN_CMD_ERASE_CHIP: {
            TRACE("- BURN_CMD_ERASE_CHIP -");

            if (len != 0) {
                TRACE("Invalid ERASE_CHIP cmd param len: %u", len);
                return ERR_LEN;
            }
            ret = erase_chip();
            if (ret) {
                TRACE("ERASE_CHIP failed");
                return ERR_ERASE_FLSH;
            }
            send_reply(&cret, 1);
            break;
        }
        default:
            TRACE("Unsupported burn cmd: 0x%x", cmd);
            return ERR_BURN_CMD;
    }

    return ERR_NONE;
}

static enum ERR_CODE handle_sector_info_cmd(unsigned int addr)
{
    unsigned int sector_addr;
    unsigned int sector_len;
    unsigned char buf[9];
    int ret;

    ret = get_sector_info(addr, &sector_addr, &sector_len);
    if (ret) {
        return ERR_DATA_ADDR;
    }

    TRACE("addr=0x%08X sector_addr=0x%08X sector_len=%u", addr, sector_addr, sector_len);

    buf[0] = ERR_NONE;
    memcpy(&buf[1], &sector_addr, 4);
    memcpy(&buf[5], &sector_len, 4);

    send_reply(buf, 9);

    return ERR_NONE;
}

static enum ERR_CODE handle_data(unsigned char **buf, size_t *len, int *extra)
{
    unsigned char cret = ERR_NONE;
    enum ERR_CODE errcode;
    struct CUST_CMD_PARAM param;

    *extra = 0;

    // Checksum
    if (check_sum((unsigned char *)&recv_msg, MSG_TOTAL_LEN(&recv_msg)) != 0xFF) {
        TRACE("Checksum error");
        return ERR_CHECKSUM;
    }

    switch (recv_msg.hdr.type) {
        case TYPE_SYS: {
            TRACE_TIME("------ SYS CMD ------");
            errcode = handle_sys_cmd((enum SYS_CMD_TYPE)recv_msg.data[0], &recv_msg.data[1], recv_msg.hdr.len - 1);
            if (errcode != ERR_NONE) {
                return errcode;
            }
            break;
        }
        case TYPE_READ: {
            unsigned int addr;
            unsigned int len;

            TRACE_TIME("------ READ CMD ------");

            addr = recv_msg.data[0] | (recv_msg.data[1] << 8) | (recv_msg.data[2] << 16) | (recv_msg.data[3] << 24);
            len = recv_msg.data[4];

            errcode = handle_read_cmd(addr, len);
            if (errcode != ERR_NONE) {
                return errcode;
            }
            break;
        }
        case TYPE_WRITE: {
            unsigned int addr;
            unsigned int len;
            unsigned char *wdata;

            TRACE_TIME("------ WRITE CMD ------");

            addr = recv_msg.data[0] | (recv_msg.data[1] << 8) | (recv_msg.data[2] << 16) | (recv_msg.data[3] << 24);
            len = recv_msg.hdr.len - 4;
            wdata = &recv_msg.data[4];

            errcode = handle_write_cmd(addr, len, wdata);
            if (errcode != ERR_NONE) {
                return errcode;
            }
            break;
        }
        case TYPE_BULK_READ: {
            unsigned int addr;
            unsigned int len;

            TRACE_TIME("------ BULK READ CMD ------");

            addr = recv_msg.data[0] | (recv_msg.data[1] << 8) | (recv_msg.data[2] << 16) | (recv_msg.data[3] << 24);
            len = recv_msg.data[4] | (recv_msg.data[5] << 8) | (recv_msg.data[6] << 16) | (recv_msg.data[7] << 24);

            errcode = handle_bulk_read_cmd(addr, len);
            if (errcode != ERR_NONE) {
                return errcode;
            }
            break;
        }
        case TYPE_ERASE_BURN_START:
        case TYPE_BURN_START: {
            if (recv_msg.hdr.type == TYPE_ERASE_BURN_START) {
                TRACE_TIME("------ ERASE_BURN_START ------");
            } else {
                TRACE_TIME("------ BURN_START ------");
            }

            burn_addr = recv_msg.data[0] | (recv_msg.data[1] << 8) | (recv_msg.data[2] << 16) | (recv_msg.data[3] << 24);
            burn_len = recv_msg.data[4] | (recv_msg.data[5] << 8) | (recv_msg.data[6] << 16) | (recv_msg.data[7] << 24);
            sector_size = recv_msg.data[8] | (recv_msg.data[9] << 8) | (recv_msg.data[10] << 16) | (recv_msg.data[11] << 24);

            TRACE("burn_addr=0x%08X burn_len=%u sector_size=%u", burn_addr, burn_len, sector_size);

            if ((size_mask & sector_size) == 0 || count_set_bits(sector_size) != 1) {
                TRACE("Unsupported sector_size=0x%08X mask=0x%08X", sector_size, size_mask);
                return ERR_SECTOR_SIZE;
            }

            sector_cnt = burn_len / sector_size;
            last_sector_len = burn_len % sector_size;
            if (last_sector_len) {
                sector_cnt++;
            } else {
                last_sector_len = sector_size;
            }

            if (sector_cnt > 0xFFFF) {
                TRACE("Sector seq overflow: %u", sector_cnt);
                return ERR_SECTOR_SEQ_OVERFLOW;
            }

            send_reply(&cret, 1);

            if (burn_len == 0) {
                TRACE("Burn length = 0");
                break;
            }

            if (recv_msg.hdr.type == TYPE_ERASE_BURN_START) {
                programmer_state = PROGRAMMER_ERASE_BURN_START;
            } else {
                programmer_state = PROGRAMMER_BURN_START;
            }

            reset_data_buf_state();

            *extra = 1;
            *buf = &data_buf[0][0];
            *len = BURN_DATA_MSG_OVERHEAD + ((sector_cnt == 1) ? last_sector_len : sector_size);
            extra_buf_len = sizeof(data_buf[0]);
            recv_extra_timeout = default_recv_timeout_short + (*len + 4096 - 1) / 4096 * default_recv_timeout_4k_data;;
            break;
        }
        case TYPE_BURN_CMD: {

            TRACE_TIME("------ BURN CMD ------");

            errcode = handle_burn_cmd((enum BURN_CMD_TYPE)recv_msg.data[0], &recv_msg.data[1], recv_msg.hdr.len - 1);

            TRACE_TIME("BURN CMD ret=%d", errcode);

            if (errcode != ERR_NONE) {
                return errcode;
            }
            break;
        }
        case TYPE_GET_SECTOR_INFO: {
            unsigned int addr;

            TRACE_TIME("------ GET SECTOR INFO ------");

            addr = recv_msg.data[0] | (recv_msg.data[1] << 8) | (recv_msg.data[2] << 16) | (recv_msg.data[3] << 24);

            errcode = handle_sector_info_cmd(addr);
            if (errcode != ERR_NONE) {
                return errcode;
            }
            break;
        }
        default:
            param.stage = CUST_CMD_STAGE_DATA;
            param.msg = &recv_msg;
            param.extra = 0;
            errcode = handle_cust_cmd(&param);
            if (errcode == ERR_NONE && param.extra) {
                *extra = 1;
                *buf = param.buf;
                *len = param.expect;
                extra_buf_len = param.size;
                recv_extra_timeout = param.timeout;
            }
            return errcode;
    }

    return ERR_NONE;
}

static enum ERR_CODE check_extra_burn_data_msg(unsigned char *data)
{
    struct message_t *msg = (struct message_t *)data;
    enum ERR_CODE errcode = ERR_NONE;
    enum MSG_TYPE type;
    const char *str_burn_data;

    if (programmer_state == PROGRAMMER_ERASE_BURN_START) {
        type = TYPE_ERASE_BURN_DATA;
        str_burn_data = "ERASE_BURN_DATA";
    } else if (programmer_state == PROGRAMMER_BURN_START) {
        type = TYPE_BURN_DATA;
        str_burn_data = "BURN_DATA";
    } else {
        return ERR_INTERNAL;
    }

    if (msg->hdr.prefix != PREFIX_CHAR) {
        TRACE("Invalid prefix char: 0x%x", msg->hdr.prefix);
        errcode = ERR_SYNC_WORD;
    } else if (msg->hdr.type != type) {
        TRACE("Invalid %s msg type: 0x%x", str_burn_data, msg->hdr.type);
        errcode = ERR_TYPE_INVALID;
    } else if (msg->hdr.len != 11) {
        TRACE("%s msg length error: %u", str_burn_data, msg->hdr.len);
        errcode = ERR_LEN;
    } else if (check_sum(data, BURN_DATA_MSG_OVERHEAD) != 0xFF) {
        TRACE("%s msg checksum error", str_burn_data);
        errcode = ERR_CHECKSUM;
    }

    return errcode;
}

static enum ERR_CODE handle_extra(unsigned char **buf, size_t *len, int *extra)
{
    enum ERR_CODE errcode = ERR_NONE;
    int ret;
    struct CUST_CMD_PARAM param;

    *extra = 0;

    switch (recv_msg.hdr.type) {
        case TYPE_ERASE_BURN_START:
        case TYPE_BURN_START: {
            unsigned int dlen;
            unsigned int mcrc;
            unsigned int crc;
            const char *str_burn_data;

            if (programmer_state == PROGRAMMER_ERASE_BURN_START) {
                str_burn_data = "ERASE_BURN_DATA";
            } else if (programmer_state == PROGRAMMER_BURN_START) {
                str_burn_data = "BURN_DATA";
            } else {
                TRACE("Invalid programmer state when receiving burn data: %d", programmer_state);
                return ERR_INTERNAL;
            }

            TRACE_TIME("------ %s ------", str_burn_data);

            errcode = check_extra_burn_data_msg((unsigned char *)&data_buf[cur_data_buf_idx][0]);
            if (errcode != ERR_NONE) {
                return errcode;
            }

            dlen = data_buf[cur_data_buf_idx][4] | (data_buf[cur_data_buf_idx][5] << 8) |
                (data_buf[cur_data_buf_idx][6] << 16) | (data_buf[cur_data_buf_idx][7] << 24);
            mcrc = data_buf[cur_data_buf_idx][8] | (data_buf[cur_data_buf_idx][9] << 8) |
                (data_buf[cur_data_buf_idx][10] << 16) | (data_buf[cur_data_buf_idx][11] << 24);
            cur_sector_seq = data_buf[cur_data_buf_idx][12] | (data_buf[cur_data_buf_idx][13] << 8);
            TRACE("%s: sec_seq=%u dlen=%u cur_data_buf_idx=%u", str_burn_data, cur_sector_seq, dlen, cur_data_buf_idx);

            if (cur_sector_seq >= sector_cnt) {
                TRACE("%s: Bad sector seq: sec_seq=%u sector_cnt=%u", str_burn_data, cur_sector_seq, sector_cnt);
                send_burn_data_reply(ERR_SECTOR_SEQ, cur_sector_seq, data_buf[cur_data_buf_idx][2]);
                return ERR_NONE;
            }

            if (((cur_sector_seq + 1) == sector_cnt && dlen != last_sector_len) ||
                    ((cur_sector_seq + 1) != sector_cnt && dlen != sector_size)) {
                TRACE("%s: Bad sector len: sec_seq=%u dlen=%u", str_burn_data, cur_sector_seq, dlen);
                send_burn_data_reply(ERR_SECTOR_DATA_LEN, cur_sector_seq, data_buf[cur_data_buf_idx][2]);
                return ERR_NONE;
            }

            crc = CRC32_FUNC(0, (unsigned char *)&data_buf[cur_data_buf_idx][BURN_DATA_MSG_OVERHEAD], dlen);
            if (crc != mcrc) {
                TRACE("%s: Bad CRC", str_burn_data);
                send_burn_data_reply(ERR_SECTOR_DATA_CRC, cur_sector_seq, data_buf[cur_data_buf_idx][2]);
                return ERR_NONE;
            }

            burn_data_buf(cur_data_buf_idx);

            if (cur_sector_seq + 1 == sector_cnt) {
                ret = wait_all_data_buf_finished();
                if (ret) {
                    TRACE("%s: Waiting all data buffer free failed", str_burn_data);
                    return ERR_INTERNAL;
                }
                // Reset state
                programmer_state = PROGRAMMER_NONE;
            } else {
                ret = get_free_data_buf();
                if (ret) {
                    TRACE("%s: Getting free data buffer failed", str_burn_data);
                    return ERR_INTERNAL;
                }

                TRACE_TIME("%s: Recv next buffer. cur_data_buf_idx=%u", str_burn_data, cur_data_buf_idx);

                *extra = 1;
                *buf = (unsigned char *)&data_buf[cur_data_buf_idx][0];
                *len = BURN_DATA_MSG_OVERHEAD + ((cur_sector_seq + 2 == sector_cnt) ? last_sector_len : sector_size);
                extra_buf_len = sizeof(data_buf[cur_data_buf_idx]);
                recv_extra_timeout = default_recv_timeout_short + (*len + 4096 - 1) / 4096 * default_recv_timeout_4k_data;;
            }
            break;
        }
        default:
            param.stage = CUST_CMD_STAGE_EXTRA;
            param.msg = &recv_msg;
            param.extra = 0;
            errcode = handle_cust_cmd(&param);
            if (errcode == ERR_NONE && param.extra) {
                *extra = 1;
                *buf = param.buf;
                *len = param.expect;
                extra_buf_len = param.size;
                recv_extra_timeout = param.timeout;
            }
            return errcode;
    }
    return ERR_NONE;
}

static int parse_packet(unsigned char **buf, size_t *len)
{
    enum ERR_CODE errcode;
    int rlen = *len;
    unsigned char *data;
    int i;
    int extra;
    unsigned char cret;

    switch (parse_state) {
        case PARSE_HEADER:
            ASSERT(rlen > 0 && rlen <= sizeof(recv_msg.hdr), "Invalid rlen!");

            if (recv_msg.hdr.prefix != PREFIX_CHAR) {
                data = (unsigned char *)&recv_msg.hdr.prefix;
                for (i = 1; i < rlen; i++) {
                    if (data[i] == PREFIX_CHAR) {
                        memmove(&recv_msg.hdr.prefix, &data[i], rlen - i);
                        break;
                    }
                }
                *buf = &data[rlen - i];
                *len = sizeof(recv_msg.hdr) + i - rlen;
            } else {
                errcode = check_msg_hdr();
                if (errcode != ERR_NONE) {
                    goto _err;
                }
                parse_state = PARSE_DATA;
                *buf = &recv_msg.data[0];
                *len = recv_msg.hdr.len + 1;
            }
            break;
        case PARSE_DATA:
        case PARSE_EXTRA:
            if (parse_state == PARSE_DATA) {
                errcode = handle_data(buf, len, &extra);
            } else {
                errcode = handle_extra(buf, len, &extra);
            }
            if (errcode != ERR_NONE) {
                goto _err;
            }
            if (extra) {
                parse_state = PARSE_EXTRA;
            } else {
                // Receive next message
                reset_parse_state(buf, len);
            }
            break;
        default:
            TRACE("Invalid parse_state: %d", parse_state);
            break;
    }

    return 0;

_err:
    cancel_input();
    cret = (unsigned char)errcode;
    send_reply(&cret, 1);

    return 1;
}

void programmer_loop(void)
{
    int ret;
    unsigned char *buf = NULL;
    size_t len = 0;
    size_t buf_len, rlen;

    TRACE("------ Init download transport ------");

    init_download_transport();
    init_cust_cmd();

_sync:
    reset_transport();
    reset_programmer_state(&buf, &len);

    TRACE("------ Send SECTOR SIZE msg ------");

    ret = send_sector_size_msg();
    if (ret) {
        TRACE("Sending SECTOR SIZE msg failed");
        goto _err;
    }

    while (1) {
        rlen = 0;
        if (parse_state == PARSE_EXTRA) {
            set_recv_timeout(recv_extra_timeout);
            buf_len = extra_buf_len;
        } else {
            if (programmer_state == PROGRAMMER_NONE && parse_state == PARSE_HEADER) {
                set_recv_timeout(default_recv_timeout_idle);
            } else {
                set_recv_timeout(default_recv_timeout_short);
            }
            buf_len = 0;
        }
        ret = recv_data_ex(buf, buf_len, len, &rlen);
        if (ret) {
            TRACE("Receiving data failed: %d", ret);
            goto _err;
        }
        //dump_buffer(buf, rlen);
        if (len != rlen) {
            TRACE("Receiving part of the data: expect=%u real=%u", len, rlen);
            goto _err;
        }

        ret = parse_packet(&buf, &len);
        if (ret) {
            TRACE("Parsing packet failed");
            goto _err;
        }
    }

_err:
    ret = handle_error();
    if (ret == 0) {
        TRACE("Start SECTOR SIZE ...");
        goto _sync;
    }

    return;
}

