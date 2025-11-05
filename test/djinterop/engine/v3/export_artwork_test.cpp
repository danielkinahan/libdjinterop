/*
    Unit test for export_album_artwork()
*/

#define BOOST_TEST_MODULE engine_v3_export_artwork_test
#include <boost/test/included/unit_test.hpp>

#include <boost/filesystem.hpp>

#include <djinterop/engine/v3/engine_library.hpp>
#include <djinterop/engine/v3/export_artwork.hpp>
#include <djinterop/track.hpp>

#include <sqlite_modern_cpp.h>
#include <memory>

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

BOOST_AUTO_TEST_CASE(album_art_id_preserved)
{
    // Create a temporary directory for the Engine library
    auto tmp = fs::temp_directory_path() / fs::unique_path("libdjinterop-%%%%-%%%%");
    fs::create_directories(tmp);

    // Create a real on-disk Engine library
    auto library = ev3::engine_library::create(tmp.string(), djinterop::engine::engine_schema::schema_3_1_0);

    // Open the database file directly
    auto db_path = tmp / "Database2" / "m.db";
    sqlite::database db(db_path.string());

    // Insert AlbumArt row
    std::vector<unsigned char> img{{0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10}};
    db << "INSERT INTO AlbumArt (hash, albumArt) VALUES (?, ?)" << std::string("deadbeef") << img;
    auto album_id = db.last_insert_rowid();

    // Add a track referencing the album art
    ev3::track_row row{0};
    row.length = 1;
    row.path = "some/path";
    row.filename = "file.mp3";
    row.album_art_id = album_id;
    auto id = library.track().add(row);

    // Read the track row using the public table API and check album art id
    {
        auto before = library.track().get(id);
        BOOST_REQUIRE(before.has_value());
        BOOST_CHECK_EQUAL(before->album_art_id, album_id);

        // Update title via public API
        library.track().set_title(id, std::make_optional<std::string>("Test Track"));

        // Read back and ensure album_art_id still present
        auto after = library.track().get(id);
        BOOST_REQUIRE(after.has_value());
        BOOST_CHECK_EQUAL(after->album_art_id, album_id);
    }

    // Verify directly in database
    int64_t db_album_art_id = 0;
    db << "SELECT albumArtId FROM Track WHERE id = ?" << id >> db_album_art_id;
    BOOST_CHECK_EQUAL(db_album_art_id, album_id);
}
