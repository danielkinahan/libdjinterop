/*
    AlbumArt table wrapper for Engine v3
*/

#pragma once

#include <cstdint>
#include <memory>
#include <optional>

#include <djinterop/config.hpp>

namespace djinterop::engine
{
struct engine_library_context;
}

namespace djinterop
{
class album_art;
}

namespace djinterop::engine::v3
{

class DJINTEROP_PUBLIC album_art_table
{
public:
    explicit album_art_table(std::shared_ptr<engine_library_context> context);

    /// Get album art by id. Returns std::nullopt when not found or when the
    /// blob is empty.
    std::optional<djinterop::album_art> get(int64_t id) const;

private:
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine::v3
