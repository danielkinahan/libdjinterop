/*
    Export implementation for Engine v3 album artwork
*/

#include "djinterop/engine/v3/export_artwork.hpp"

#include <cassert>
#include <cstddef>
#include <fstream>

#include <djinterop/album_art.hpp>
#include <djinterop/engine/v3/engine_library.hpp>
#include <djinterop/engine/v3/track_table.hpp>

namespace djinterop::engine::v3
{
int export_album_artwork(const engine_library& library,
                         const std::filesystem::path& out_dir)
{
    // Ensure output directory exists
    std::error_code ec;
    std::filesystem::create_directories(out_dir, ec);
    if (ec)
    {
        return 0;
    }

    int written = 0;

    for (auto id : library.track().all_ids())
    {
        auto row = library.track().get(id);
        if (!row)
            continue;

        if (row->album_art_id == TRACK_ROW_ID_NONE)
            continue;

        auto art = library.album_art().get(row->album_art_id);
        if (!art)
            continue;

        // Derive filename
        std::string name = art->hash_hex_str;
        if (name.empty())
            name = std::to_string(row->album_art_id);

        auto ext = art->suggested_extension();
        if (ext.empty())
            ext = "bin";

        auto out_path = out_dir / (name + "." + ext);
        art->write_to_file(out_path);
        ++written;
    }

    return written;
}

}  // namespace djinterop::engine::v3
