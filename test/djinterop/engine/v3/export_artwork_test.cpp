/*
    Unit test for export_album_artwork()
*/

#define BOOST_TEST_MODULE engine_v3_export_artwork_test
#include <boost/test/included/unit_test.hpp>

#include <boost/filesystem.hpp>

#include <djinterop/engine/v3/engine_library.hpp>
#include <djinterop/engine/v3/export_artwork.hpp>

#include <sqlite_modern_cpp.h>

namespace ev3 = djinterop::engine::v3;
namespace fs = boost::filesystem;

BOOST_AUTO_TEST_CASE(export_writes_file)
{
    // Create a temporary directory for the Engine library on disk so we can
    // open the m.db directly to insert AlbumArt rows.
    auto tmp = fs::temp_directory_path() / fs::unique_path("libdjinterop-%%%%-%%%%");
    fs::create_directories(tmp);

    // Create a real on-disk Engine library
    auto library = ev3::engine_library::create(tmp.string(), djinterop::engine::engine_schema::schema_3_1_0);

    // Open the database file directly
    auto db_path = tmp / "Database2" / "m.db";
    sqlite::database db(db_path.string());

    // Prepare a tiny fake JPEG header as image data
    std::vector<unsigned char> img{{0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10}};

    // Insert AlbumArt row
    db << "INSERT INTO AlbumArt (hash, albumArt) VALUES (?, ?)" << std::string("deadbeef") << img;
    auto album_id = db.last_insert_rowid();

    // Add a track referencing the album art
    ev3::track_row row{0};
    row.length = 1;
    row.path = "some/path";
    row.filename = "file.mp3";
    row.album_art_id = album_id;
    auto id = library.track().add(row);

    // Export to an output directory
    auto out = tmp / "out";
    auto count = ev3::export_album_artwork(library, out.string());

    BOOST_CHECK_EQUAL(count, 1);

    // Check file exists (named by hash + extension .jpg)
    auto expected = out / (std::string("deadbeef") + ".jpg");
    BOOST_CHECK(fs::exists(expected));
}
