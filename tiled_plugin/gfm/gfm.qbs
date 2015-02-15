import qbs 1.0

TiledPlugin {
    cpp.defines: ["GFM_LIBRARY"]

    files: [
        "gfm_global.h",
        "gfmplugin.cpp",
        "gfmplugin.h",
    ]
}
