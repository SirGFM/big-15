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

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeTilemap(QSaveFile &file, const TileLayer *tileLayer);
static void writeObject(QSaveFile &file, const MapObject *obj);
static void writeEvent(QSaveFile &file, const MapObject *ev);
#else
static void writeTilemap(QFile &file, const TileLayer *tileLayer);
static void writeObject(QFile &file, const MapObject *obj);
static void writeEvent(QFile &file, const MapObject *ev);
#endif

GfmPlugin::GfmPlugin()
{
}

bool GfmPlugin::write(const Map *map, const QString &fileName)
{
#ifdef HAS_QSAVEFILE_SUPPORT
    QSaveFile file(fileName);
#else
    QFile file(fileName);
#endif
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        mError = tr("Could not open file for writing.");
        return false;
    }


    // Write every layer
    foreach (const Layer *layer, map->layers()) {
        if (!layer->isVisible())
            continue;
        
        // Write a tilemap
        if (layer->layerType() == Layer::TileLayerType) {
            const TileLayer *tileLayer;
            
            tileLayer = static_cast<const TileLayer*>(layer);
            
            writeTilemap(file, tileLayer);
        }
        else if (layer->layerType() == Layer::ObjectGroupType) {
            const ObjectGroup *objectGroup;
            
            objectGroup = static_cast<const ObjectGroup*>(layer);
            
            foreach (const MapObject *obj, objectGroup->objects()) {
                if (!obj->isVisible())
                    continue;
                
                if (obj->type() == "obj")
                    writeObject(file, obj);
                else if (obj->type() == "event")
                    writeEvent(file, obj);
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

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeTilemap(QSaveFile &file, const TileLayer *tileLayer) {
#else
static void writeTilemap(QFile &file, const TileLayer *tileLayer) {
#endif
    // Write out the tilemap, by ID
    file.write("tm:[\n", 5);
    for (int y = 0; y < tileLayer->height(); ++y) {
        for (int x = 0; x < tileLayer->width(); ++x) {
            const Cell &cell = tileLayer->cellAt(x, y);
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
static void writeObjectBounds(QSaveFile &file, const MapObject *obj) {
#else
static void writeObjectBounds(QFile &file, const MapObject *obj) {
#endif
    int h, w, x, y;
    
    x = ((int)obj->x()) / 8;
    y = ((int)obj->y()) / 8;
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
static void writeObject(QSaveFile &file, const MapObject *obj) {
#else
static void writeObject(QFile &file, const MapObject *obj) {
#endif
    file.write("obj: {", 6);
    
    writeObjectBounds(file, obj);
    
    for (QMap<QString, QString>::const_iterator it = obj->properties().begin();
        it != obj->properties().end(); it++) {
        file.write(" ", 1);
        file.write(it.key().toUtf8());
        file.write(":", 1);
        file.write(it.value().toUtf8());
    }
    
    file.write(" }\n", 3);
}

#ifdef HAS_QSAVEFILE_SUPPORT
static void writeEvent(QSaveFile &file, const MapObject *ev) {
#else
static void writeEvent(QFile &file, const MapObject *ev) {
#endif
    file.write("ev: {", 5);
    
    writeObjectBounds(file, ev);
    
    for (QMap<QString, QString>::const_iterator it = ev->properties().begin();
        it != ev->properties().end(); it++) {
        file.write(" ", 1);
        if (it.key() == "var0" || it.key() == "var1" || it.key() == "var2"
          || it.key() == "var3" ) {
            if (it.value() == "gv_max")
                continue;
            file.write("var", 3);
        }
        else if (it.key() == "int0" || it.key() == "int1" || it.key() == "int2"
          || it.key() == "int3")
            file.write("int", 3);
        else
            file.write(it.key().toUtf8());
        file.write(":", 1);
        file.write(it.value().toUtf8());
    }
    
    file.write(" }\n", 3);
}

