/*
    This file is part of libdjinterop.

    libdjinterop is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libdjinterop is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libdjinterop.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef DJINTEROP_ALBUM_ART_HPP
#define DJINTEROP_ALBUM_ART_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>

#include <djinterop/config.hpp>

namespace djinterop
{
// Artwork for tracks
class DJINTEROP_PUBLIC album_art
{
public:
    using image_data_type = uint_least8_t;

    std::string hash_hex_str;
    std::vector<image_data_type> image_data;

    // Return true if no image data is present.
    bool empty() const noexcept { return image_data.empty(); }

    // Try to detect a MIME type from the image bytes. Returns empty string
    // when unknown.
    std::string mime_type() const noexcept;

    // Suggest a filename extension (without dot) based on the data, or
    // return an empty string when unknown.
    std::string suggested_extension() const noexcept;

    // Write the image data to the given path. Throws std::system_error on
    // failure to write.
    void write_to_file(const std::filesystem::path& path) const;
};

}  // namespace djinterop

#endif  // DJINTEROP_ALBUM_ART_HPP
