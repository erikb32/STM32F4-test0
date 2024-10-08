#include <string.h>
#include "conf.h"

#include "flash.h"
#include "utils.h"

#define CONF_DATA_VERSION    0u

conf_v0_t app_conf;

static bool _is_valid = false;

static inline const uint8_t* _get_conf_start(uint32_t adr)
{
    uint8_t* mem_start = (uint8_t*)FLASH_CONF_AREA_ADR;
    return &mem_start[adr];
}

static inline uint32_t _next_conf_adr(uint32_t adr)
{
    return adr + sizeof(conf_v0_t);
}

static inline bool _is_free(uint32_t adr)
{
    return (0xFFFFFFFF == *(uint32_t*)_get_conf_start(adr));
}

static uint32_t _find_conf(void)
{
    uint32_t adr;
    uint32_t next_adr = 0;;

    do {
        adr = next_adr;
        next_adr = _next_conf_adr(adr);
    } while ((next_adr < FLASH_AREA_MAX_LEN) && (_is_free(next_adr) == false));

    return adr;
}

static void _load_conf(void)
{
    uint32_t adr = _find_conf();

    if (_is_free(adr) == false) {
        memcpy(&app_conf, _get_conf_start(adr), sizeof(app_conf));
        _is_valid = true;
    }
}

// -- Public Interface --
void conf_init(void)
{
    _is_valid = false;
    _load_conf();
}

bool conf_is_valid(void)
{
    return _is_valid;
}

// Note: no automatic cleaning when area is full
void conf_save(void)
{
    app_conf.c.version = CONF_DATA_VERSION;

    uint32_t adr = _find_conf();
    if (_is_free(adr) == false) {
        if (memcmp(_get_conf_start(adr), (void*)&app_conf, sizeof(conf_v0_t)) == 0) {
            return; // Conf not changed
        }
        adr = _next_conf_adr(adr);
    }
    flash_write_conf_words(adr, app_conf.raw_data, ARRAYSIZE(app_conf.raw_data));
    _is_valid = true;
}
