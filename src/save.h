#ifndef __SAVE_H__
#define __SAVE_H__

enum enBlock {
    BLK_CONFIG,
    BLK_GAME
};

/** Initialize every block. */
void setup_blocks();

/**
 * Write the value on a block's slot into a temporary buffer. The
 * value will be ready for reading right after this call.
 * To save this value, be sure to call 'flush_block' after the last
 * 'write_slot'.
 *
 * @param [in]block The block.
 * @param [in]slot The slot.
 * @param [in]val The value.
 */
void write_slot(enum enBlock block, int slot, int val);

/**
 * Write many values on a block's temporary buffer. These values will be ready
 * for reading right after this call. To save the values, be sure to call
 * 'flush_block' afterwards.
 *
 * @param [in]block The block.
 * @param [in]val The values.
 * @param [in]num How many values are in val.
 */
void write_block(enum enBlock block, int *val, int num);

/**
 * Read a block's slot.
 *
 * @param [in]block The block.
 * @param [in]slot The slot.
 */
int read_slot(enum enBlock block, int slot);

/**
 * Read slots from a  block.
 *
 * @param [in]block The block.
 * @param [in]val Read the values into val.
 * @param [in]num How many values should be read.
 */
void read_block(enum enBlock block, int *val, int num);

/**
 * Actually save a given block to its file.
 *
 * @param [in]block The block.
 */
int flush_block(enum enBlock block);

/**
 * Check whether a given block has already had some data written to it.
 *
 * @param [in]block The block.
 */
int block_has_data(enum enBlock block);

#define SAVE_CONF_SLOTS \
    X(int, ctr_pl1) \
    X(int, ctr_pl2) \
    X(int, hint) \
    X(int, zoom) \
    X(int, music) \
    X(int, sfx) \
    X(int, ufps) \
    X(int, dfps) \
    X(int, speedrun) \
    X(int, lang)

enum enSaveConf {
#define X(type, name) \
    sv_ ## name,
    SAVE_CONF_SLOTS
#undef X
};

#endif /* __SAVE_H__ */
