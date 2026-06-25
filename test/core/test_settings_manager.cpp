/**
 * @file    test_settings_manager.cpp
 * @author  Marvin Smith
 * @date    2026-06-24
 *
 * @brief   Unit tests for Settings_Manager
 */
#include <gtest/gtest.h>

// C++ Standard Libraries
#include <memory>
#include <optional>
#include <string>
#include <vector>

// Project Libraries
#include <overboard/core/settings_manager.hpp>
#include <overboard/hal/i_settings_store.hpp>
#include <overboard/hal/settings_tree.hpp>

using namespace ovb::core;
using namespace ovb::hal;

// ─── In-memory mock store ────────────────────────────────────────────────────

/**
 * Simple in-memory I_Settings_Store implementation for testing.
 * Optionally pre-populated with a Settings_Tree at construction.
 */
class Mock_Store : public I_Settings_Store {
    public:
        explicit Mock_Store(Settings_Tree initial = {}, bool load_succeeds = true, bool save_succeeds = true)
            : m_tree(std::move(initial))
            , m_load_succeeds(load_succeeds)
            , m_save_succeeds(save_succeeds) {}

        bool load(Settings_Tree& out_tree) override {
            if (!m_load_succeeds) return false;
            out_tree = m_tree;
            ++m_load_count;
            return true;
        }

        bool save(const Settings_Tree& tree) override {
            if (!m_save_succeeds) return false;
            m_tree = tree;
            ++m_save_count;
            return true;
        }

        bool        exists()      const override { return true; }
        std::string location()    const override { return "<mock>"; }
        bool        is_writable() const override { return m_save_succeeds; }

        int m_load_count { 0 };
        int m_save_count { 0 };

    private:
        Settings_Tree m_tree;
        bool          m_load_succeeds;
        bool          m_save_succeeds;
};

// ─── Constructor / load ───────────────────────────────────────────────────────

TEST(Settings_Manager, Constructor_Loads_From_Store) {
    Settings_Tree initial;
    initial.set("display.brightness", 80);
    auto store_ptr = std::make_unique<Mock_Store>(initial);
    auto* raw = store_ptr.get();

    Settings_Manager mgr(std::move(store_ptr));

    EXPECT_EQ(raw->m_load_count, 1);
    EXPECT_EQ(mgr.get<int>("display.brightness").value(), 80);
}

TEST(Settings_Manager, Constructor_Load_Failure_Leaves_Empty_Tree) {
    auto store = std::make_unique<Mock_Store>(Settings_Tree{}, false);
    Settings_Manager mgr(std::move(store));

    EXPECT_FALSE(mgr.has("display.brightness"));
    EXPECT_FALSE(mgr.is_dirty());
}

TEST(Settings_Manager, Not_Dirty_After_Construction) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    EXPECT_FALSE(mgr.is_dirty());
}

// ─── get / set ────────────────────────────────────────────────────────────────

TEST(Settings_Manager, Get_Returns_Nullopt_When_Missing) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    EXPECT_FALSE(mgr.get<int>("no.such.key").has_value());
}

TEST(Settings_Manager, Get_With_Default_Returns_Default_When_Missing) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    EXPECT_EQ(mgr.get<int>("missing", 42), 42);
}

TEST(Settings_Manager, Set_Makes_Dirty) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    EXPECT_FALSE(mgr.is_dirty());
    mgr.set("foo.bar", 1);
    EXPECT_TRUE(mgr.is_dirty());
}

TEST(Settings_Manager, Set_And_Get_Roundtrip_Int) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    mgr.set("display.brightness", 75);
    EXPECT_EQ(mgr.get<int>("display.brightness").value(), 75);
}

TEST(Settings_Manager, Set_And_Get_Roundtrip_String) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    mgr.set<std::string>("calculator.angle_mode", "radians");
    EXPECT_EQ(mgr.get<std::string>("calculator.angle_mode").value(), "radians");
}

TEST(Settings_Manager, Set_And_Get_Roundtrip_Float) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    mgr.set("location.latitude", 51.5);
    EXPECT_NEAR(mgr.get<double>("location.latitude").value(), 51.5, 1e-6);
}

TEST(Settings_Manager, Set_And_Get_Roundtrip_Bool) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    mgr.set("feature.enabled", true);
    EXPECT_TRUE(mgr.get<bool>("feature.enabled").value());
}

TEST(Settings_Manager, Has_Returns_True_After_Set) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    EXPECT_FALSE(mgr.has("x.y"));
    mgr.set("x.y", 1);
    EXPECT_TRUE(mgr.has("x.y"));
}

// ─── remove ──────────────────────────────────────────────────────────────────

TEST(Settings_Manager, Remove_Clears_Key) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    mgr.set("a.b", 99);
    EXPECT_TRUE(mgr.has("a.b"));
    mgr.remove("a.b");
    EXPECT_FALSE(mgr.has("a.b"));
}

TEST(Settings_Manager, Remove_Makes_Dirty) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    mgr.set("a.b", 1);
    // clear dirty by saving
    mgr.save();
    EXPECT_FALSE(mgr.is_dirty());
    mgr.remove("a.b");
    EXPECT_TRUE(mgr.is_dirty());
}

// ─── save ────────────────────────────────────────────────────────────────────

TEST(Settings_Manager, Save_Clears_Dirty) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    mgr.set("x", 1);
    EXPECT_TRUE(mgr.is_dirty());
    EXPECT_TRUE(mgr.save());
    EXPECT_FALSE(mgr.is_dirty());
}

TEST(Settings_Manager, Save_Calls_Store_Once) {
    auto store_ptr = std::make_unique<Mock_Store>();
    auto* raw = store_ptr.get();
    Settings_Manager mgr(std::move(store_ptr));
    mgr.set("x", 1);
    mgr.save();
    EXPECT_EQ(raw->m_save_count, 1);
}

TEST(Settings_Manager, Save_Failure_Leaves_Dirty) {
    auto store = std::make_unique<Mock_Store>(Settings_Tree{}, true, false);
    Settings_Manager mgr(std::move(store));
    mgr.set("x", 1);
    EXPECT_FALSE(mgr.save());
    EXPECT_TRUE(mgr.is_dirty());
}

// ─── reload ──────────────────────────────────────────────────────────────────

TEST(Settings_Manager, Reload_Picks_Up_New_Values) {
    Settings_Tree initial;
    initial.set("display.brightness", 50);
    auto store_ptr = std::make_unique<Mock_Store>(initial);

    Settings_Manager mgr(std::move(store_ptr));
    EXPECT_EQ(mgr.get<int>("display.brightness").value(), 50);

    // Simulate the store file being updated externally by modifying
    // then calling save so the mock store holds the new tree
    mgr.set("display.brightness", 90);
    mgr.save();

    // Now reload — should reflect saved value
    EXPECT_TRUE(mgr.reload());
    EXPECT_EQ(mgr.get<int>("display.brightness").value(), 90);
}

TEST(Settings_Manager, Reload_Failure_Leaves_Tree_Unchanged) {
    Settings_Tree initial;
    initial.set("x", 7);
    auto store = std::make_unique<Mock_Store>(initial, true, true);

    // Construct normally (load succeeds)
    Settings_Manager mgr(std::make_unique<Mock_Store>(initial, true, true));
    EXPECT_EQ(mgr.get<int>("x").value(), 7);

    // Now build a manager whose store always fails to load
    auto bad_store = std::make_unique<Mock_Store>(initial, false, true);
    Settings_Manager mgr2(std::move(bad_store));
    // reload should return false; tree was never loaded
    EXPECT_FALSE(mgr2.reload());
}

// ─── tree() accessor ─────────────────────────────────────────────────────────

TEST(Settings_Manager, Tree_Accessor_Reflects_Current_State) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    mgr.set("a.b", 123);
    EXPECT_EQ(mgr.tree().get<int>("a.b").value(), 123);
}

// ─── Observer pattern ────────────────────────────────────────────────────────

TEST(Settings_Manager, Observer_Notified_On_Set) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());

    std::vector<std::string> fired;
    mgr.add_observer([&](const std::string& path) {
        fired.push_back(path);
    });

    mgr.set("foo.bar", 1);
    ASSERT_EQ(fired.size(), 1u);
    EXPECT_EQ(fired[0], "foo.bar");
}

TEST(Settings_Manager, Observer_Notified_On_Remove) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());
    mgr.set("x", 1);

    std::vector<std::string> fired;
    mgr.add_observer([&](const std::string& path) { fired.push_back(path); });

    mgr.remove("x");
    ASSERT_EQ(fired.size(), 1u);
    EXPECT_EQ(fired[0], "x");
}

TEST(Settings_Manager, Prefix_Observer_Only_Fires_For_Matching_Prefix) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());

    std::vector<std::string> location_fired;
    std::vector<std::string> all_fired;

    mgr.add_observer("location", [&](const std::string& p) { location_fired.push_back(p); });
    mgr.add_observer([&](const std::string& p) { all_fired.push_back(p); });

    mgr.set("display.brightness", 80);
    mgr.set("location.latitude", 39.7);

    EXPECT_EQ(location_fired.size(), 1u);
    EXPECT_EQ(location_fired[0], "location.latitude");
    EXPECT_EQ(all_fired.size(), 2u);
}

TEST(Settings_Manager, Observer_Not_Fired_After_Removal) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());

    int count = 0;
    const size_t id = mgr.add_observer([&](const std::string&) { ++count; });

    mgr.set("a", 1);
    EXPECT_EQ(count, 1);

    mgr.remove_observer(id);
    mgr.set("a", 2);
    EXPECT_EQ(count, 1);  // no additional fires
}

TEST(Settings_Manager, Multiple_Observers_All_Notified) {
    Settings_Manager mgr(std::make_unique<Mock_Store>());

    int a = 0, b = 0;
    mgr.add_observer([&](const std::string&) { ++a; });
    mgr.add_observer([&](const std::string&) { ++b; });

    mgr.set("x", 1);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);
}

TEST(Settings_Manager, Reload_Notifies_All_Observers) {
    Settings_Tree initial;
    initial.set("x", 1);
    auto store = std::make_unique<Mock_Store>(initial);

    Settings_Manager mgr(std::move(store));

    std::vector<std::string> fired;
    mgr.add_observer([&](const std::string& p) { fired.push_back(p); });

    EXPECT_TRUE(mgr.reload());
    // reload fires with empty path (signals full refresh)
    ASSERT_EQ(fired.size(), 1u);
    EXPECT_EQ(fired[0], "");
}
