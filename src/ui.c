/**
 * @file src/ui.c
 */
#include "global.h"
#include "globalVar.h"
#include "types.h"
#include "ui.h"

#define BOX        93
#define RECT_L    189
#define RECT_C    190
#define RECT_R    191
#define HEART_ON  277
#define HEART_OFF 278
#define HJBOOT    279
#define TELEP     280
#define SIGNL     281
#define P_ONE     284
#define P_TWO     285

/** Global timer used to animate ui graphics */
static int _ui_globalTime;

/** Simple structure to help passing parameters */
struct stHeartArray {
    int i;      /** Current number of drawn hearts                     */
    int l;      /** How many hearts should be drawn                    */
    int tile;   /** Current tile (either HEART_ON or HEART_OFF         */
    int x;      /** Current horizontal position                        */
    int y;      /** Current vertical position                          */
    int horInc; /** Next heart's horizontal position, relative to this */
    int horMax; /** How many hearts fit in a row                       */
    int horIni; /** Initial horizontal position                        */
    int verInc; /** Next heart's vertical position, relative to this   */
};

/**
 * Draw a few hearts to the screen
 * 
 * @param pData Current rendering state
 */
static void ui_drawHearts(struct stHeartArray *pData);

/**
 * Draw an item inside its box
 * 
 * @param item The item to be rendered
 * @param x The box's horizontal position
 * @param y The box's vertical position
 */
static void ui_drawItemBox(int item, int x, int y);

/**
 * Draw an item inside its box
 * 
 * @param item The item to be rendered
 * @param x The box's horizontal position
 * @param y The box's vertical position
 */
static void ui_drawItem(int item, int x, int y);

/**
 * Initialize the ui
 * 
 * @return GFraMe error code
 */
GFraMe_ret ui_init() {
    GFraMe_ret rv;
    
    _ui_globalTime = 0;
    
    rv = GFraMe_ret_ok;
//__ret:
    return rv;
}

/**
 * Clean up the ui
 */
void ui_clean() {
    
}

/**
 * Updates the ui
 * 
 * @param ms Time, in milliseconds, elapsed from last frame
 */
void ui_update(int ms) {
    _ui_globalTime += ms;
}

/**
 * Draw the ui to the screen
 */
void ui_draw() {
    int i, x, y, items, pl1Item, pl2Item;
    struct stHeartArray data;
    
    // Setup rendering of player 1 lives
    data.i = 0;
    data.l = gv_getValue(PL1_HP);
    data.tile = HEART_ON;
    data.x = 24;
    data.y = 4;
    data.horInc = 8;
    data.horMax = 5;
    data.horIni = 24;
    data.verInc = 8;
    // Draw current lives
    ui_drawHearts(&data);
    data.l = gv_getValue(PL1_MAXHP);
    // Draw current max lives
    data.tile = HEART_OFF;
    ui_drawHearts(&data);
    
    // Setup rendering of player 2 lives
    data.i = 0;
    data.l = gv_getValue(PL2_HP);
    data.tile = HEART_ON;
    data.x = 288;
    data.y = 4;
    data.horInc = -8;
    data.horMax = 5;
    data.horIni = 288;
    data.verInc = 8;
    // Draw current lives
    ui_drawHearts(&data);
    data.l = gv_getValue(PL2_MAXHP);
    // Draw current max lives
    data.tile = HEART_OFF;
    ui_drawHearts(&data);
    
    // Render which item player 1 is holding
    ui_drawItemBox(gv_getValue(PL1_ITEM), 4/*x*/, 4/*y*/);
    // Render which item player 2 is holding
    ui_drawItemBox(gv_getValue(PL2_ITEM), 300/*x*/, 4/*y*/);
    
    // Render every gotten itens
    x = 84;
    y = 4;
    GFraMe_spriteset_draw(gl_sset8x16, RECT_L, x, y, 0);
    x += 8;
    i = 0;
    //while (i < 22) {
    while (i < 4) {
        GFraMe_spriteset_draw(gl_sset8x16, RECT_C, x, y, 0);
        i++;
        x += 8;
    }
    GFraMe_spriteset_draw(gl_sset8x16, RECT_R, x, y, 0);
    
    // Get every found item
    items = gv_getValue(ITEMS);
    // Removes both players items from the pool
    pl1Item = gv_getValue(PL1_ITEM);
    pl2Item = gv_getValue(PL2_ITEM);
    // Draw every enabled item
    x = 96;
    y = 7;
    while (items) {
        int item;
        #define SET_ITEM(ITEM_ID) if (!item && (items & ITEM_ID) == ITEM_ID) item = ITEM_ID
        
        item = 0;
        SET_ITEM(ID_HIGHJUMP);
        SET_ITEM(ID_TELEPORT);
        SET_ITEM(ID_SIGNALER);
        // TODO check for more items
        if (item == 0) {
            // Shouldn't happen, but just in case...
            GFraMe_log("No more items to be drawn!");
            break;
        }
        if (item == pl1Item)
            ui_drawItem(ID_PL1ITEM, x, y);
        else if (item == pl2Item)
            ui_drawItem(ID_PL2ITEM, x, y);
        else
            ui_drawItem(item, x, y);
        // Draw the current item
        // Clean up this item flag
        items ^= item;
        // Set the next item position
        x += 8;
        
        #undef SET_ITEM
    }
}

/**
 * Draw a few hearts to the screen
 * 
 * @param pData Current rendering state
 */
static void ui_drawHearts(struct stHeartArray *pData) {
    while (pData->i < pData->l) {
        GFraMe_spriteset_draw(gl_sset8x8, pData->tile, pData->x, pData->y, 0);
        pData->i++;
        pData->x += pData->horInc;
        if (pData->i % pData->horMax == 0) {
            pData->x = pData->horIni;
            pData->y += pData->verInc;
        }
    }
}

/**
 * Draw an item inside its box
 * 
 * @param item The item to be rendered
 * @param x The box's horizontal position
 * @param y The box's vertical position
 */
static void ui_drawItemBox(int item, int x, int y) {
    ui_drawItem(item, x + 4, y + 3);
    GFraMe_spriteset_draw(gl_sset16x16, BOX/*tile*/, x, y, 0/*flip*/);
}

/**
 * Draw an item inside
 * 
 * @param item The item to be rendered
 * @param x The box's horizontal position
 * @param y The box's vertical position
 */
static void ui_drawItem(int item, int x, int y) {
    int tile;
    
    switch (item) {
        case ID_HIGHJUMP: tile = HJBOOT; break;
        case ID_TELEPORT: tile = TELEP; break;
        case ID_SIGNALER: tile = SIGNL; break;
        case ID_PL1ITEM: tile = P_ONE; break;
        case ID_PL2ITEM: tile = P_TWO; break;
        // TODO Render item
        default: return;
    }
    GFraMe_spriteset_draw(gl_sset8x8, tile, x, y, 0/*flip*/);
}

