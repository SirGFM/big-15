/**
 * GFM (proprietary) tiled plugin
 * 
 * Based on the "CSV Tiled Plugin"
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gfmplugin.h"

#include "map.h"
#include "mapobject.h"
#include "objectgroup.h"
#include "tile.h"
#include "tilelayer.h"

#include <QFile>

#if QT_VERSION >= 0x050100
#define HAS_QSAVEFILE_SUPPORT
#endif

#ifdef HAS_QSAVEFILE_SUPPORT
#include <QSaveFile>
#endif

using namespace Tiled;
using namespace Gfm;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} gfm_offset;

static void getTilemapBounds(gfm_offset *pOff, const TileLayer *tileLayer);
#ifdef HAS_QSAVEFILE_SUPPORT
static void writeTilemap(QSaveFile &file, const TileLayer *tileLayer,
    gfm_offset *pOff);
static void writeObject(QSaveFile &file, const MapObject *obj,
    gfm_offset *pOff);
static void writeEvent(QSaveFile &file, const MapObject *ev,
    gfm_offset *pOff);
static void writeMob(QSaveFile &file, const MapObject *obj,
    gfm_offset *pOff);
#else
static void writeTilemap(QFile &file, const TileLayer *tileLayer,
    gfm_offset *pOff);
static void writeObject(QFile &file, const MapObject *obj,
    gfm_offset *pOff);
static void writeEvent(QFile &file, const MapObject *ev,
    gfm_offset *pOff);
static void writeMob(QFile &file, const MapObject *obj,
    gfm_offset *pOff);
#endif

GfmPlugin::GfmPlugin()
{
}

bool GfmPlugin::write(const Map *map, const QString &fileName)
{
    gfm_offset off;
    int foundTileLayer;
#ifdef HAS_QSAVEFILE_SUPPORT
    QSaveFile file(fileName);
#else
    QFile file(fileName);
#endif
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        mError = tr("Could not open file for writing.");
        return false;
    }
    
    foundTileLayer = 0;
    
    // Write every layer
    foreach (const Layer *layer, map->layers()) {
        if (!layer->isVisible())
            continue;
        
        // Write a tilemap
        if (layer->layerType() == Layer::TileLayerType) {
            const TileLayer *tileLayer;
            
            if (foundTileLayer == 1) {
                mError = tr("Found more than one tilemap!");
                return false;
            }
            
            foundTileLayer = 1;
            
            tileLayer = static_cast<const TileLayer*>(layer);
            
            getTilemapBounds(&off, tileLayer);
            writeTilemap(file, tileLayer, &off);
        }
        else if (layer->layerType() == Layer::ObjectGroupType) {
            const ObjectGroup *objectGroup;
            
            if (foundTileLayer == 0) {
                mError = tr("First layer must be a tilemap!");
                return false;
            }
            
            objectGroup = static_cast<const ObjectGroup*>(layer);
            
            foreach (const MapObject *obj, objectGroup->objects()) {
                if (!obj->isVisible())
                    continue;
                
                if (obj->type() == "obj")
                    writeObject(file, obj, &off);
                else if (obj->type() == "event")
                    writeEvent(file, obj, &off);
                else if (obj->type() == "mob")
                    writeMob(file, obj, &off);
                else {
                    mError = "Invalid object type!";
                    return false;
                }
            }
        }
    }

    if (file.error() != QFile::NoError) {
        mError = file.errorString();
        return false;
    }

#ifdef HAS_QSAVEFILE_SUPPORT
    if (!file.commit()) {
        mError = file.errorString();
        return false;
    }
#endif

    return true;
}

QString GfmPlugin::nameFilter() const
{
    return tr("GFM files (*.gfm)");
}

QString GfmPlugin::errorString() const
{
    return mError;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Gfm, GfmPlugin)
#endif

static void getTilemapBounds(gfm_offset *pOff, const TileLayer *tileLayer) {
    int j, x, y, w, h;
    
    x = -1;
    y = -1;
    w = -1;
    h = -1;
    j = 0;
    while (j < tileLayer->height()) {
        // Search for the first tile
        if (x == -1 && y == -1) {
            int i;
            
            i = 0;
            while (i < tileLayer->width()) {
                const Tile *tile;
                
                tile = tileLayer->cellAt(i, j).tile;
                if (tile && tile->id() != -1) {
                    x = i;
                    y = j;
                    break;
                }
                i++;
            }
        }
        // Now, calculate the width
        if (x != -1 && y != -1 && w == -1) {
            int i;
            
            i = x;
            while (i < tileLayer->width()) {
                const Tile *tile;
                
                tile = tileLayer->cellAt(i, j).tile;
                if (!tile || tile->id() == -1) {
                    w = i - x;
                    break;
                }
                i++;
            }
        }
        // Finally, search for the height
        if (x != -1 && y != -1 && w != -1 && h == -1) {
            j = y;
            while (j < tileLayer->height()) {
                const Tile *tile;
                
                tile = tileLayer->cellAt(x, j).tile;
                if (!tile || tile->id() == -1) {
                    h = j - y;
                    break;
                }
                j++;
            }
            if (h == -1)
                h = j - y;
            break;
        }
        // Shouldn't happen, but just in case...
        if (x != -1 && y != -1 && w != -1 && h != -1)
            break;
        j++;
    }
    
    pOff->x = x;
    pOff->y = y;
    pOff->w = w;
    pOff->h = h;
}

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeTilemap(QSaveFile &file, const TileLayer *tileLayer,
    gfm_offset *pOff) {
#else
static void writeTilemap(QFile &file, const TileLayer *tileLayer,
    gfm_offset *pOff) {
#endif
    // Write out the tilemap, by ID
/*
    file.write("teste: {", 8);
    file.write(QByteArray::number(pOff->x));
    file.write(", ", 2);
    file.write(QByteArray::number(pOff->y));
    file.write(", ", 2);
    file.write(QByteArray::number(pOff->w));
    file.write(", ", 2);
    file.write(QByteArray::number(pOff->h));
    file.write(", ", 2);
    file.write("}\n", 2);
*/
    file.write("tm:[\n", 5);
//    for (int y = 0; y < tileLayer->height(); ++y) {
//        for (int x = 0; x < tileLayer->width(); ++x) {
    for (int y = 0; y < pOff->h; y++) {
        for (int x = 0; x < pOff->w; x++) {
            const Cell &cell = tileLayer->cellAt(pOff->x + x, pOff->y + y);
            const Tile *tile = cell.tile;
            const int id = tile ? tile->id() : -1;
            file.write(QByteArray::number(id));
            file.write(",", 1);
        }
        
        file.write("\n", 1);
    }
    file.write("]\n", 2);
}

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeObjectBounds(QSaveFile &file, const MapObject *obj, 
    gfm_offset *pOff) {
#else
static void writeObjectBounds(QFile &file, const MapObject *obj, 
    gfm_offset *pOff) {
#endif
    int h, w, x, y;
    
    x = ((int)obj->x()) / 8 - pOff->x;
    y = ((int)obj->y()) / 8 - pOff->y;
    w = ((int)obj->width()) / 8;
    h = ((int)obj->height()) / 8;
    
    file.write(" x:", 3);
    file.write(QByteArray::number(x));
    file.write(" y:", 3);
    file.write(QByteArray::number(y));
    file.write(" w:", 3);
    file.write(QByteArray::number(w));
    file.write(" h:", 3);
    file.write(QByteArray::number(h));
}

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeString(QSaveFile &file, const QString &str) {
#else
static void writeString(QFile &file, const QString &str) {
#endif
    int i;
    QStringList list = str.split("|");
    
    i = 0;
    while (1) {
        file.write("\"", 1);
        file.write(list.at(i).toUtf8());
        file.write("\"", 1);
        
        i++;
        if (i >= list.count())
            break;
        file.write("|", 1);
    }
}

#define isVar(it)\
    (it.key() == "var" || it.key() == "var0" || it.key() == "var1" \
        || it.key() == "var2" || it.key() == "var3" )
#define isInt(it)\
    (it.key() == "int" || it.key() == "int0" || it.key() == "int1" \
        || it.key() == "int2" || it.key() == "int3" )

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeObject(QSaveFile &file, const MapObject *obj,
    gfm_offset *pOff) {
#else
static void writeObject(QFile &file, const MapObject *obj,
    gfm_offset *pOff) {
#endif
    file.write("obj: {", 6);
    
    writeObjectBounds(file, obj, pOff);
    
    for (QMap<QString, QString>::const_iterator it = obj->properties().begin();
        it != obj->properties().end(); it++) {
        file.write(" ", 1);
        if (isVar(it))
            file.write("var", 3);
        else
            file.write(it.key().toUtf8());
        
        file.write(":", 1);
        
        if (!isInt(it))
            writeString(file, it.value());
        else
            file.write(it.value().toUtf8());
    }
    
    file.write(" }\n", 3);
}

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeEvent(QSaveFile &file, const MapObject *ev,
    gfm_offset *pOff) {
#else
static void writeEvent(QFile &file, const MapObject *ev,
    gfm_offset *pOff) {
#endif
    file.write("ev: {", 5);
    
    writeObjectBounds(file, ev, pOff);
    
    for (QMap<QString, QString>::const_iterator it = ev->properties().begin();
        it != ev->properties().end(); it++) {
        file.write(" ", 1);
        
        if (isVar(it)) {
            if (it.value() == "gv_max")
                continue;
            file.write("var", 3);
        }
        else if (isInt(it))
            file.write("int", 3);
        else
            file.write(it.key().toUtf8());
        
        file.write(":", 1);
        if (!isInt(it))
            writeString(file, it.value());
        else
            file.write(it.value().toUtf8());
    }
    
    file.write(" }\n", 3);
}

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeMob(QSaveFile &file, const MapObject *obj,
    gfm_offset *pOff) {
#else
static void writeMob(QFile &file, const MapObject *obj,
    gfm_offset *pOff) {
#endif
    file.write("mob: {", 6);
    
    // Write the mob to its exact position
    file.write(" x:", 3);
    file.write(QByteArray::number(((int)obj->x()) - pOff->x * 8));
    file.write(" y:", 3);
    file.write(QByteArray::number(((int)obj->y()) - pOff->y * 8));
    
    for (QMap<QString, QString>::const_iterator it = obj->properties().begin();
        it != obj->properties().end(); it++) {
        file.write(" ", 1);
        if (isVar(it))
            file.write("var", 3);
        else
            file.write(it.key().toUtf8());
        
        file.write(":", 1);
        
        if (!isInt(it))
            writeString(file, it.value());
        else
            file.write(it.value().toUtf8());
    }
    
    file.write(" }\n", 3);
}

