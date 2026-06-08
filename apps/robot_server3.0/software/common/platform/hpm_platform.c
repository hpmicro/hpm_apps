#include "board.h"
#include "hpm_romapi.h"
#include "hpm_ppor_drv.h"
#include "hpm_l1c_drv.h"
#ifdef CONFIG_BGP_PSM
#include "hpm_psm.h"
#endif
#include "hpm_flashmap.h"

#define DISABLE_GLOBAL_INTERRUPT() disable_global_irq(CSR_MSTATUS_MIE_MASK)
#define ENABLE_GLOBAL_INTERRUPT() enable_global_irq(CSR_MSTATUS_MIE_MASK)

#ifdef CONFIG_BGP_FREERTOS
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

#define SECTOR_SIZE (4 * 1024)
#define BLOCK_SIZE (64 * 1024)

static xpi_nor_config_t s_xpi_nor_config;
static uint8_t platform_flash_init = 0;

// addr is relative address, starting from 0 addr
#define FLASH_CHECK_RANGE(addr, len, max_len) \
    {                                         \
        if ((addr + len) > max_len)           \
            return -1;                 \
    }

/*
 *  platform flash init interface.
 *  Return statue.
 */
int hpm_platform_flash_init(void)
{
    uint32_t flash_size = 0;
    int cnt;
    uint32_t back_plic[2][4];
    xpi_nor_config_option_t option;
    if (platform_flash_init)
        return 0;
    option.header.U = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    option.option0.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    option.option1.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;
    for(cnt = 0; cnt < 4; cnt++)
    {
        back_plic[0][cnt] = HPM_PLIC->TARGETINT[0].INTEN[cnt];
        HPM_PLIC->TARGETINT[0].INTEN[cnt] = 0;
        back_plic[1][cnt] = HPM_PLIC->TARGETINT[1].INTEN[cnt];
        HPM_PLIC->TARGETINT[1].INTEN[cnt] = 0;
    }
    DISABLE_GLOBAL_INTERRUPT();
    rom_xpi_nor_auto_config(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, &option);
    rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, xpi_nor_property_total_size,
                             &flash_size);
    fencei();
    ENABLE_GLOBAL_INTERRUPT();
    for(cnt = 0; cnt < 4; cnt++)
    {
        HPM_PLIC->TARGETINT[0].INTEN[cnt] = back_plic[0][cnt];
        HPM_PLIC->TARGETINT[1].INTEN[cnt] = back_plic[1][cnt];
    }
    if (flash_size < FLASH_MAX_SIZE)
    {
        return -1;
    }
    FLASH_MUTEX_CREATE();
    platform_flash_init = 1;

    return 0;
}

/*
 * platform flash init states
 * Return init states: true:init done, false: no init or init fail
 */
bool hpm_platform_get_flash_initdone(void)
{
    return (platform_flash_init == 1) ? true : false;
}

/*
 *  flash read interface.
 *  Return actual read number.
 */
int hpm_flash_read(unsigned int addr, unsigned char *buffer, unsigned int len)
{
    hpm_stat_t status;
    int result = -1;
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

/*
 *  flash write interface
 *  Return actual written number.
 */
int hpm_flash_write(unsigned int addr, const unsigned char *buffer, unsigned int len)
{
    hpm_stat_t status;
    int result = -1;

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
int hpm_flash_erase_sector(unsigned int addr)
{
    hpm_stat_t status;
    int result = -1;

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
int hpm_flash_erase(unsigned int addr, unsigned int len)
{
    hpm_stat_t status;
    int result = -1;

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

/*
 *  Profile entry get interface.
 *  Return physical address.
 */
unsigned long hpm_profile_entry(void)
{
    return (FLASH_ADDR_BASE + FLASH_PROFILE_ADDR);
}

/*
 *  Profile read interface.
 *  Return actual read number.
 */
int hpm_profile_read(unsigned int addr, unsigned char *buffer, unsigned int len)
{
    return hpm_flash_read(FLASH_PROFILE_ADDR + addr, buffer, len);
}

/*
 *  Profile write interface (used for airconfig).
 *  Return actual written number.
 */
int hpm_profile_write(unsigned int addr, const unsigned char *buffer, unsigned int len)
{
    FLASH_CHECK_RANGE(addr, len, FLASH_PROFILE_SIZE);

    return hpm_flash_write(FLASH_PROFILE_ADDR + addr, buffer, len);
}

/*
 *  Profile cleanup interface (used for airconfig).
 *
 *  Return 0 means cleanup success.
 */
int hpm_profile_cleanup(void)
{
    return (hpm_flash_erase(FLASH_PROFILE_ADDR, FLASH_PROFILE_SIZE) == FLASH_PROFILE_SIZE) ? 0 : -1;
}

#ifdef CONFIG_BGP_PSM
/*
 *  Get PSM area space size interface (file or flash).
 *  Return actual space size (bytes).
 */
int hpm_psm_size(unsigned int psm_type)
{
    int size = 0;

    if (psm_type == HPM_PSM_RESET_UNABLE_ERASE)
        size = FLASH_KEYVALUE1_SIZE;
    else if (psm_type == HPM_PSM_RESET_ABLE_ERASE)
        size = FLASH_KEYVALUE2_SIZE;

    return size;
}

/*
 *  PSM area raw write interface (file or flash).
 *  Return actual written number.
 */
int hpm_psm_write(unsigned int psm_type, unsigned int addr,
                  const unsigned char *buffer, unsigned int len)
{
    int result = -1;

    if (psm_type == HPM_PSM_RESET_UNABLE_ERASE)
    {
        FLASH_CHECK_RANGE(addr, len, FLASH_KEYVALUE1_SIZE);
        result = hpm_flash_write(FLASH_KEYVALUE1_ADDR + addr, buffer, len);
    }
    else if (psm_type == HPM_PSM_RESET_ABLE_ERASE)
    {
        FLASH_CHECK_RANGE(addr, len, FLASH_KEYVALUE2_SIZE);
        result = hpm_flash_write(FLASH_KEYVALUE2_ADDR + addr, buffer, len);
    }

    return result;
}

/*
 *  PSM area raw erase interface (file or flash, set 0xFF...0xFF).
 *  Return actual erase number.
 */
int hpm_psm_erase(unsigned int psm_type, unsigned int addr, unsigned int len)
{
    int result = -1;

    if (psm_type == HPM_PSM_RESET_UNABLE_ERASE)
    {
        FLASH_CHECK_RANGE(addr, len, FLASH_KEYVALUE1_SIZE);
        result = hpm_flash_erase(FLASH_KEYVALUE1_ADDR + addr, len);
    }
    else if (psm_type == HPM_PSM_RESET_ABLE_ERASE)
    {
        FLASH_CHECK_RANGE(addr, len, FLASH_KEYVALUE2_SIZE);
        result = hpm_flash_erase(FLASH_KEYVALUE2_ADDR + addr, len);
    }

    return result;
}

/*
 *  PSM area raw read interface (file or flash).
 *  Return actual read number.
 */
int hpm_psm_read(unsigned int psm_type, unsigned int addr,
                 unsigned char *buffer, unsigned int len)
{
    hpm_stat_t status;
    int result = -1;

    if (psm_type == HPM_PSM_RESET_UNABLE_ERASE)
    {
        result = hpm_flash_read(FLASH_KEYVALUE1_ADDR + addr, buffer, len);
    }
    else if (psm_type == HPM_PSM_RESET_ABLE_ERASE)
    {
        result = hpm_flash_read(FLASH_KEYVALUE2_ADDR + addr, buffer, len);
    }

    return result;
}
#endif
/*
 *  file system read interface.
 *  Return actual read number.
 */
int hpm_filesystem_read(unsigned int addr, unsigned char *buffer, unsigned int len)
{
    memcpy(buffer, (void *)(FLASH_ADDR_BASE + FLASH_FILESYSTEM_ADDR + addr), len);
    return len;
    //return hpm_flash_read(FLASH_FILESYSTEM_ADDR + addr, buffer, len);
}

/*
 *  file system write interface
 *  Return actual written number.
 */
int hpm_filesystem_write(unsigned int addr, const unsigned char *buffer, unsigned int len)
{
    FLASH_CHECK_RANGE(addr, len, FLASH_FILESYSTEM_SIZE);

    return hpm_flash_write(FLASH_FILESYSTEM_ADDR + addr, buffer, len);
}

/*
 *  file system auto write interface, auto erase and write
 *  Return actual written number.
 */
int hpm_filesystem_auto_write(unsigned int addr, const unsigned char *buffer, unsigned int len)
{
    hpm_stat_t status = status_success;
    uint32_t start_addr;
    uint32_t write_addr;
    uint32_t sec_count;
    uint32_t end_addr;
    uint32_t sec_off;
    uint32_t i;
    static unsigned char sec_data[FLASH_SECTOR_SIZE];
    int result = -1;

    FLASH_CHECK_RANGE(addr, len, FLASH_FILESYSTEM_SIZE);

    write_addr = FLASH_FILESYSTEM_ADDR + addr;
    start_addr = write_addr % FLASH_SECTOR_SIZE;

    if (start_addr == 0)
    {
        sec_count = len / FLASH_SECTOR_SIZE;
        end_addr = (write_addr + len) % FLASH_SECTOR_SIZE;
    }
    else
    {
        if (len < FLASH_SECTOR_SIZE)
        {
            sec_count = 0;
            end_addr = (write_addr + len) % FLASH_SECTOR_SIZE;
        }
        else
        {
            sec_count = (len - (FLASH_SECTOR_SIZE - start_addr)) / FLASH_SECTOR_SIZE;
            end_addr = (write_addr + len) % FLASH_SECTOR_SIZE;
        }
    }

    // FLASH_MUTEX_GET();
    if (start_addr != 0)
    {
        result = hpm_flash_read(write_addr / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE, sec_data, FLASH_SECTOR_SIZE);

        sec_off = write_addr % FLASH_SECTOR_SIZE;
        for (i = 0; i < len; i++)
        {
            if (sec_data[sec_off + i] != 0XFF)
                break;
        }
        if (i < len)
        {
            result = hpm_flash_erase_sector(write_addr / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE);
            result = hpm_flash_write((write_addr - start_addr), sec_data, start_addr);
            result = hpm_flash_write(write_addr, buffer, len);

            if ((start_addr + len) < FLASH_SECTOR_SIZE)
            {
                result = hpm_flash_write(write_addr + len, &sec_data[start_addr + len], (FLASH_SECTOR_SIZE - start_addr - len));
            }
        }
        else
        {
            result = hpm_flash_write(write_addr, buffer, len);
        }
        write_addr = write_addr + (FLASH_SECTOR_SIZE - start_addr);
        buffer = buffer + (FLASH_SECTOR_SIZE - start_addr);
    }

    while (sec_count--)
    {
        result = hpm_flash_erase_sector(write_addr / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE);
        result = hpm_flash_write(write_addr, buffer, FLASH_SECTOR_SIZE);
        write_addr += FLASH_SECTOR_SIZE;
        buffer += FLASH_SECTOR_SIZE;
    }

    if (end_addr != 0)
    {
        result = hpm_flash_read(write_addr / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE, sec_data, FLASH_SECTOR_SIZE);

        sec_off = write_addr % FLASH_SECTOR_SIZE;
        for (i = 0; i < len; i++)
        {
            if (sec_data[sec_off + i] != 0XFF)
                break;
        }
        if (i < len)
        {
            result = hpm_flash_erase_sector(write_addr / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE);
            result = hpm_flash_write(write_addr, buffer, end_addr);
            result = hpm_flash_write((write_addr + end_addr), &sec_data[end_addr], (FLASH_SECTOR_SIZE - end_addr));
        }
        else
        {
            result = hpm_flash_write(write_addr, buffer, end_addr);
        }
    }
    // FLASH_MUTEX_PUT();
    if (status == status_success)
    {
        result = len;
    }
    return result;
}

/*
 *  filesystem erase interface (file or flash, set 0xFF...0xFF).
 *  Return actual erase number.
 */
int hpm_filesystem_erase(unsigned int addr, unsigned int len)
{
    FLASH_CHECK_RANGE(addr, len, FLASH_FILESYSTEM_SIZE);

    return hpm_flash_erase(FLASH_FILESYSTEM_ADDR + addr, len);
}

/*
 *  file system cleanup interface
 *
 *  Return 0 means cleanup success.
 */
int hpm_filesystem_cleanup(void)
{
    return hpm_flash_erase(FLASH_FILESYSTEM_ADDR, FLASH_FILESYSTEM_SIZE);
}

/*
 *  usercommon read interface.
 *  Return actual read number.
 */
int hpm_usercommon_read(unsigned int addr, unsigned char *buffer, unsigned int len)
{
    return hpm_flash_read(FLASH_USER_COMMON_ADDR + addr, buffer, len);
}

/*
 *  usercommon write interface
 *  Return actual written number.
 */
int hpm_usercommon_write(unsigned int addr, const unsigned char *buffer, unsigned int len)
{
    FLASH_CHECK_RANGE(addr, len, FLASH_USER_COMMON_SIZE);

    return hpm_flash_write(FLASH_USER_COMMON_ADDR + addr, buffer, len);
}

/*
 *  usercommon erase interface (file or flash, set 0xFF...0xFF).
 *  Return actual erase number.
 */
int hpm_usercommon_erase(unsigned int addr, unsigned int len)
{
    FLASH_CHECK_RANGE(addr, len, FLASH_USER_COMMON_SIZE);

    return hpm_flash_erase(FLASH_USER_COMMON_ADDR + addr, len);
}

/*
 *  usercommon cleanup interface
 *
 *  Return 0 means cleanup success.
 */
int hpm_usercommon_cleanup(void)
{
    return hpm_flash_erase(FLASH_USER_COMMON_ADDR, FLASH_USER_COMMON_SIZE);
}
