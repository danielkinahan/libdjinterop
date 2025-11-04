/*
    Export album artwork for Engine v3 libraries
*/

#pragma once

#include <filesystem>
#include <djinterop/config.hpp>

namespace djinterop::engine::v3
{
class engine_library;

/// Export album artwork for all tracks in the given Engine v3 library into
/// the specified output directory.  Files are named using the artwork hash
/// (or the albumArtId if the hash is empty) and the suggested file extension.
///
/// \return The number of artwork files written.
DJINTEROP_PUBLIC
int export_album_artwork(const engine_library& library,
                         const std::filesystem::path& out_dir);

}  // namespace djinterop::engine::v3
