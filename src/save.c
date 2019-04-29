#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_save.h>

#include "global.h"
#include "globalVar.h"
#include "save.h"

#define X(type, name) \
    type name;
struct conf {
    SAVE_CONF_SLOTS
};
#undef X

static struct conf _conf;
static int _gameSlot[GV_MAX];
static int _gameHasSave;

static void setup_conf() {
    GFraMe_save sv, *pSv;
    int rv;

    pSv = 0;
    rv = GFraMe_save_bind(&sv, CONFFILE);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to open conf file", __ret);
    pSv = &sv;

#define X(type, name) \
    rv = GFraMe_save_read_ ## type(&sv, #name, &_conf.name); \
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to read " # name, __ret);
    SAVE_CONF_SLOTS
#undef X

__ret:
    if (pSv)
        GFraMe_save_close(pSv);
}

static void setup_game() {
    GFraMe_save sv, *pSv;
    int gv, rv;
    char varname[sizeof("var000")];

    pSv = 0;
    rv = GFraMe_save_bind(&sv, SAVEFILE);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to open save file", __ret);
    pSv = &sv;

    memset(varname, 0x0, sizeof(varname));
    memcpy(varname, "var", 3);
    for (gv = 0; gv < GV_MAX; gv++) {
        varname[3] = '0' + ((gv / 100) % 10);
        varname[4] = '0' + ((gv / 10) % 10);
        varname[5] = '0' + (gv % 10);

        rv = GFraMe_save_read_int(pSv, varname, _gameSlot + gv);
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Error writing variable", __ret);
    }

    rv = GFraMe_ret_ok;
__ret:
    if (pSv)
        GFraMe_save_close(pSv);
}

void setup_blocks() {
    /* Initialize every configuration to -1 */
    #define X(type, name) \
        _conf.name = -1;
    SAVE_CONF_SLOTS
    #undef X
    /* Initialize the global variables */
    memset(_gameSlot, 0x0, sizeof(_gameSlot));
    _gameSlot[PL1_MAXHP] = 3;
    _gameSlot[PL1_HP] = 3;
    _gameSlot[PL2_MAXHP] = 3;
    _gameSlot[PL2_HP] = 3;
    _gameSlot[SIGL_X] = -1;
    _gameSlot[SIGL_Y] = -1;

    /* Try to initialize both from their files */
#if !defined(EMCC)
    setup_conf();
    setup_game();
#endif
}

void write_slot(enum enBlock block, int slot, int val) {
    switch (block) {
    case BLK_CONFIG:
        switch (slot) {
        #define X(type, name) \
            case sv_ ## name: \
                _conf.name = val; \
                break;
        SAVE_CONF_SLOTS
        #undef X
        }
        break;
    case BLK_GAME:
        break;
    }
}

void write_block(enum enBlock block, int *val, int num) {
    switch (block) {
    case BLK_CONFIG:
        break;
    case BLK_GAME:
        memset(_gameSlot, 0x0, sizeof(_gameSlot));
        if (num > sizeof(_gameSlot) / sizeof(_gameSlot[0]))
            num = sizeof(_gameSlot) / sizeof(_gameSlot[0]);
        memcpy(_gameSlot, val, num * sizeof(int));
    }
}

int read_slot(enum enBlock block, int slot) {
    switch (block) {
    case BLK_CONFIG:
        switch (slot) {
        #define X(type, name) \
            case sv_ ## name: \
                return _conf.name;
            SAVE_CONF_SLOTS
        #undef X
        default:
            return -1;
        }
    case BLK_GAME:
    default:
        return -1;
    }
}

void read_block(enum enBlock block, int *val, int num) {
    switch (block) {
    case BLK_CONFIG:
        break;
    case BLK_GAME:
        memset(val, 0x0, num * sizeof(int));
        if (num > GV_MAX)
            num = GV_MAX;
        memcpy(val, _gameSlot, num * sizeof(int));
        break;
    }
}

static int flush_conf() {
#if defined(EMCC)
    return 0;
#else
    GFraMe_save sv, *pSv;
    int rv;

    pSv = 0;
    rv = GFraMe_save_bind(&sv, CONFFILE);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to open conf file", __ret);
    pSv = &sv;

#define X(type, name) \
    rv = GFraMe_save_write_ ## type(&sv, #name, _conf.name); \
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to write " # name, __ret);
    SAVE_CONF_SLOTS
#undef X

    rv = GFraMe_ret_ok;
__ret:
    if (pSv)
        GFraMe_save_close(pSv);

    return rv;
#endif
}

static int flush_game() {
#if defined(EMCC)
    return 0;
#else
    GFraMe_save sv, *pSv;
    int gv, rv;
    char varname[sizeof("var000")];

    pSv = 0;
    rv = GFraMe_save_bind(&sv, SAVEFILE);
    GFraMe_assertRet(rv == GFraMe_ret_ok, "Failed to open save file", __ret);
    pSv = &sv;

    memset(varname, 0x0, sizeof(varname));
    memcpy(varname, "var", 3);
    for (gv = 0; gv < GV_MAX; gv++) {
        varname[3] = '0' + ((gv / 100) % 10);
        varname[4] = '0' + ((gv / 10) % 10);
        varname[5] = '0' + (gv % 10);

        rv = GFraMe_save_write_int(pSv, varname, _gameSlot[gv]);
        GFraMe_assertRet(rv == GFraMe_ret_ok, "Error writing variable", __ret);
    }

    rv = GFraMe_ret_ok;
__ret:
    if (pSv)
        GFraMe_save_close(pSv);

    return rv;
#endif
}

int flush_block(enum enBlock block) {
    switch (block) {
    case BLK_CONFIG:
        return flush_conf();
    case BLK_GAME:
        _gameHasSave = 1;
        return flush_game();
    default:
        return -1;
    }
}

int block_has_data(enum enBlock block) {
#if defined(EMCC)
    return (block == BLK_GAME && _gameHasSave);
#else
    GFraMe_save sv;
    int rv;

    switch (block) {
    case BLK_CONFIG:
        rv = GFraMe_save_bind(&sv, CONFFILE);
        break;
    case BLK_GAME:
        if (_gameHasSave)
            return 1;
        rv = GFraMe_save_bind(&sv, SAVEFILE);
        break;
    default:
        return 0;
    }

    if (rv != GFraMe_ret_ok)
        return 0;

    /* Check if anything other than the version is written */
    rv = (sv.size > 50);
    GFraMe_save_close(&sv);

    return rv;
#endif
}
