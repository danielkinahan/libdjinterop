/*
    Engine v3 AlbumArt table implementation
*/

#include <optional>

#include <djinterop/album_art.hpp>
#include <djinterop/engine/v3/album_art_table.hpp>

#include "../../util/convert.hpp"
#include "../engine_library_context.hpp"

namespace djinterop::engine::v3
{
album_art_table::album_art_table(std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
}

std::optional<djinterop::album_art> album_art_table::get(int64_t id) const
{
    std::optional<djinterop::album_art> result;

    // Read hash and blob from AlbumArt
    context_->db << "SELECT hash, albumArt FROM AlbumArt WHERE id = ?" << id >>
        [&](std::optional<std::string> hash, std::optional<std::vector<std::byte>> blob)
    {
        if (!blob || blob->empty())
        {
            result = std::nullopt;
            return;
        }

        djinterop::album_art art;
        art.hash_hex_str = hash.value_or("");
        art.image_data.reserve(blob->size());
        for (auto b : *blob)
            art.image_data.push_back(static_cast<djinterop::album_art::image_data_type>(std::to_integer<uint8_t>(b)));

        result = std::move(art);
    };

    return result;
}

}  // namespace djinterop::engine::v3
