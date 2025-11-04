/*
    Album art helper implementation
*/

#include <fstream>
#include <system_error>

#include <djinterop/album_art.hpp>

namespace djinterop
{

std::string album_art::mime_type() const noexcept
{
    if (image_data.size() >= 2)
    {
        // JPEG
        if (image_data[0] == 0xFF && image_data[1] == 0xD8)
            return "image/jpeg";
    }

    if (image_data.size() >= 8)
    {
        // PNG: 89 50 4E 47 0D 0A 1A 0A
        if (image_data[0] == 0x89 && image_data[1] == 0x50 && image_data[2] == 0x4E && image_data[3] == 0x47)
            return "image/png";
    }

    if (image_data.size() >= 6)
    {
        // GIF: GIF87a or GIF89a
        if (image_data[0] == 'G' && image_data[1] == 'I' && image_data[2] == 'F')
            return "image/gif";
    }

    return {};
}

std::string album_art::suggested_extension() const noexcept
{
    auto mt = mime_type();
    if (mt == "image/jpeg")
        return "jpg";
    if (mt == "image/png")
        return "png";
    if (mt == "image/gif")
        return "gif";
    return {};
}

void album_art::write_to_file(const std::filesystem::path& path) const
{
    if (empty())
        return;

    std::ofstream ofs(path, std::ios::binary);
    if (!ofs)
        throw std::system_error(errno, std::generic_category(), "Failed to open file for writing");

    ofs.write(reinterpret_cast<const char*>(image_data.data()), static_cast<std::streamsize>(image_data.size()));
    if (!ofs)
        throw std::system_error(errno, std::generic_category(), "Failed to write album art to file");
}

}  // namespace djinterop
