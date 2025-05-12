#include "board.h"
#include "hpm_romapi.h"
#include "hpm_ppor_drv.h"
#include "hpm_l1c_drv.h"
#include "ota_kconfig.h"
#include "hpm_flash.h"

/**
 * @brief default in ram, avoid flash crash
 */
#ifndef OTA_PLACE_FLASH
#define OTA_ATTR ATTR_RAMFUNC
#else
#define OTA_ATTR
#endif

#if defined(CONFIG_USE_RAM_TYPE) && CONFIG_USE_RAM_TYPE
#define DISABLE_GLOBAL_INTERRUPT()
#define ENABLE_GLOBAL_INTERRUPT()
#else
#define DISABLE_GLOBAL_INTERRUPT() disable_global_irq(CSR_MSTATUS_MIE_MASK)
#define ENABLE_GLOBAL_INTERRUPT() enable_global_irq(CSR_MSTATUS_MIE_MASK)
#endif

#ifdef CONFIG_FREERTOS
static hpm_mutex_handle_t flash_mutex_handle = NULL;
#define FLASH_MUTEX_CREATE()                                  \
    if (flash_mutex_handle == NULL)                           \
    {                                                         \
        flash_mutex_handle = hpm_mutex_create("flash_mutex"); \
    }
#define FLASH_MUTEX_GET() hpm_mutex_get(flash_mutex_handle, HPM_WAIT_FOREVER);
#define FLASH_MUTEX_PUT() hpm_mutex_put(flash_mutex_handle);
#else
#define FLASH_MUTEX_CREATE()
#define FLASH_MUTEX_GET()
#define FLASH_MUTEX_PUT()
#endif

#define FLASH_SECTOR_COUNT   (FLASH_MAX_SIZE/FLASH_SECTOR_SIZE)

static xpi_nor_config_t s_xpi_nor_config;
static uint8_t flash_init_done = 0;

static uint8_t flash_erase_mask[FLASH_SECTOR_COUNT/8 + 1] = {0};
/*
 *  platform flash init interface.
 *  Return statue.
 */
OTA_ATTR
int hpm_flash_init(void)
{
    uint32_t flash_size = 0;
    xpi_nor_config_option_t option;
    if (flash_init_done)
        return 0;
    option.header.U = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    option.option0.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    option.option1.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;

    DISABLE_GLOBAL_INTERRUPT();
    rom_xpi_nor_auto_config(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, &option);
    rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, xpi_nor_property_total_size,
                             &flash_size);
    fencei();
    ENABLE_GLOBAL_INTERRUPT();

    if (flash_size < FLASH_MAX_SIZE)
    {
        return -1;
    }
    FLASH_MUTEX_CREATE();
    flash_init_done = 1;
    memset(flash_erase_mask, 0, sizeof(flash_erase_mask));

    return 0;
}

/*
 * platform flash init states
 * Return init states: true:init done, false: no init or init fail
 */
OTA_ATTR
bool hpm_get_flash_initdone(void)
{
    return (flash_init_done == 1) ? true : false;
}

/*
 *  flash read interface.
 *  Return actual read number.
 */
OTA_ATTR
int hpm_flash_read(unsigned int addr, unsigned char *buffer, unsigned int len)
{
    hpm_stat_t status;
    int result = -1;
    addr > FLASH_ADDR_BASE ? addr -= FLASH_ADDR_BASE : addr;
    FLASH_MUTEX_GET();
    DISABLE_GLOBAL_INTERRUPT();
    status = rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,
                              (uint32_t *)buffer, addr, len);
    fencei();
    ENABLE_GLOBAL_INTERRUPT();
    FLASH_MUTEX_PUT();
    if (status_success == status)
    {
        result = len;
    }
    // result = len;
    return result;
}

/**
 * @brief hpm localmem read flash interface
 * Return
 */
OTA_ATTR
int hpm_flash_localmem_read(unsigned int addr, unsigned char* buffer, unsigned int len)
{
    addr < FLASH_ADDR_BASE ? addr += FLASH_ADDR_BASE : addr;
    uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(addr);
    uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(addr + len);
    uint32_t aligned_size = aligned_end - aligned_start;

    l1c_dc_invalidate(aligned_start, aligned_size);

    memcpy(buffer, (void *)addr, len);
    return (int)len;
}

/*
 *  flash write interface
 *  Return actual written number.
 */
OTA_ATTR
int hpm_flash_write(unsigned int addr, const unsigned char *buffer, unsigned int len)
{
    hpm_stat_t status;
    int result = -1;

    addr > FLASH_ADDR_BASE ? addr -= FLASH_ADDR_BASE : addr;
    FLASH_MUTEX_GET();
    DISABLE_GLOBAL_INTERRUPT();
    status = rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,
                                 (uint32_t *)buffer, addr, len);
    fencei();
    ENABLE_GLOBAL_INTERRUPT();
    FLASH_MUTEX_PUT();
    if (status == status_success)
    {
        result = len;
    }
    else
        printf("write failed:%d\r\n", status);
    return result;
}

/*
 *  flash erase interface (file or flash, set 0xFF...0xFF). must sector erase
 *  Return actual erase number.
 */
OTA_ATTR
int hpm_flash_erase_sector(unsigned int addr)
{
    hpm_stat_t status;
    int result = -1;
    addr > FLASH_ADDR_BASE ? addr -= FLASH_ADDR_BASE : addr;
    if (addr % FLASH_SECTOR_SIZE)
        return result;

    FLASH_MUTEX_GET();
    DISABLE_GLOBAL_INTERRUPT();
    status = rom_xpi_nor_erase_sector(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, addr);
    fencei();
    ENABLE_GLOBAL_INTERRUPT();
    FLASH_MUTEX_PUT();

    if (status == status_success)
        result = FLASH_SECTOR_SIZE;

    return result;
}

/*
 *  flash erase interface (file or flash, set 0xFF...0xFF). must sector erase
 *  Return actual erase number.
 */
OTA_ATTR
int hpm_flash_erase(unsigned int addr, unsigned int len)
{
    hpm_stat_t status;
    int result = -1;

    addr > FLASH_ADDR_BASE ? addr -= FLASH_ADDR_BASE : addr;
    if (addr % FLASH_SECTOR_SIZE || len % FLASH_SECTOR_SIZE)
        return result;

    FLASH_MUTEX_GET();
    DISABLE_GLOBAL_INTERRUPT();
    status = rom_xpi_nor_erase(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, addr, len);
    fencei();
    ENABLE_GLOBAL_INTERRUPT();
    FLASH_MUTEX_PUT();

    if (status == status_success)
        result = len;

    return result;
}

OTA_ATTR
bool hpm_flash_is_erased(unsigned int addr, unsigned int len)
{
    addr < FLASH_ADDR_BASE ? addr += FLASH_ADDR_BASE: addr;

    uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(addr);
    uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(addr + len);
    uint32_t aligned_size = aligned_end - aligned_start;

    l1c_dc_invalidate(aligned_start, aligned_size);
    for(unsigned int i = 0; i < len; i++)
    {
        if(*(uint8_t*)(addr+i) != 0xFF)
        {
            return false;
        }
    }
    return true;
}

//warning: no support remap enable use
OTA_ATTR
int hpm_flash_erase_and_rewrite(unsigned int addr, unsigned int len)
{
    int result = -1;
    unsigned int start_sector_addr, end_sector_addr;
    unsigned char buffer[FLASH_SECTOR_SIZE] = {0};
    addr > FLASH_ADDR_BASE ? addr -= FLASH_ADDR_BASE : addr;

    start_sector_addr = addr / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE;
    end_sector_addr = (addr + len + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE;

    for(unsigned int start_addr = start_sector_addr; start_addr < end_sector_addr; start_addr += FLASH_SECTOR_SIZE)
    {
        if((start_addr < addr) || (start_addr + FLASH_SECTOR_SIZE > addr + len))
        {
            result = hpm_flash_localmem_read(start_addr, buffer, FLASH_SECTOR_SIZE);
            if(result != 0)
                break;
        }
        result = hpm_flash_erase_sector(start_addr);
        if(result < 0)
            break;
        if(start_addr < addr)
        {
            result = hpm_flash_write(start_addr, &buffer[0], addr - start_addr);
            if(result < 0)
                break;
        }

        if(start_addr + FLASH_SECTOR_SIZE > addr + len)
        {
            result = hpm_flash_write(addr + len, &buffer[addr + len - start_addr], start_addr + FLASH_SECTOR_SIZE - (addr + len));
            if(result < 0)
                break;
        }
    }
    return result < 0 ? result : (int)len;
}

OTA_ATTR
void hpm_flash_empty_erase_mask(void)
{
    memset(flash_erase_mask, 0, sizeof(flash_erase_mask));
}

OTA_ATTR
int hpm_flash_erase_write_of_mask(unsigned int addr, const unsigned char *buffer, unsigned int len)
{
    unsigned int start_sector, end_sector, mask, pos;
    addr > FLASH_ADDR_BASE ? addr -= FLASH_ADDR_BASE : addr;
    start_sector = addr / FLASH_SECTOR_SIZE;
    end_sector = (addr + len + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE;
    for(unsigned int sector = start_sector; sector < end_sector; sector++)
    {
        mask = 1 << (sector % 8);
        pos = sector / 8;
        if(flash_erase_mask[pos] & mask)
        {
            continue;
        }
        else
        {
            if(hpm_flash_erase_sector(sector*FLASH_SECTOR_SIZE) < 0)
                return -1;
            flash_erase_mask[pos] |= mask;
        }
    }
    return hpm_flash_write(addr, buffer, len);
}