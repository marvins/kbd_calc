/**
 * @file    test_settings_tree.cpp
 * @author  Marvin Smith
 * @date    2026-06-13
 *
 * @brief   Unit tests for Settings_Tree
 */
#include <gtest/gtest.h>

// C++ Standard Libraries
#include <filesystem>
#include <fstream>
#include <string>

// Project Libraries
#include <overboard/core/enums.hpp>
#include <overboard/hal/settings_tree.hpp>

using namespace ovb::hal;

/***************************/
/*   Basic Get/Set Tests   */
/***************************/
TEST(Settings_Tree, Basic_Bool_GetSet) {
    Settings_Tree tree;

    tree.set("feature.enabled", true);
    EXPECT_TRUE(tree.get<bool>("feature.enabled").value());
    EXPECT_TRUE(tree.get("feature.enabled", false));
}

TEST(Settings_Tree, Basic_Int_GetSet) {
    Settings_Tree tree;

    tree.set("display.brightness", 75);
    EXPECT_EQ(tree.get<int>("display.brightness").value(), 75);
    EXPECT_EQ(tree.get("display.brightness", 50), 75);
}

TEST(Settings_Tree, Basic_Float_GetSet) {
    Settings_Tree tree;

    tree.set("audio.volume", 0.85f);
    EXPECT_FLOAT_EQ(tree.get<float>("audio.volume").value(), 0.85f);
    EXPECT_FLOAT_EQ(tree.get("audio.volume", 0.5f), 0.85f);
}

TEST(Settings_Tree, Basic_String_GetSet) {
    Settings_Tree tree;

    tree.set("general.name", "TestApp");
    EXPECT_EQ(tree.get<std::string>("general.name").value(), "TestApp");
    EXPECT_EQ(tree.get("general.name", std::string("Default")), "TestApp");
}

/***************************/
/*   Path Type Tests       */
/***************************/
TEST(Settings_Tree, Path_GetSet) {
    Settings_Tree tree;

    std::filesystem::path test_path = "/home/user/config.toml";
    tree.set("config.file", test_path);

    auto result = tree.get<std::filesystem::path>("config.file");
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), test_path);
}

TEST(Settings_Tree, Path_Default) {
    Settings_Tree tree;

    std::filesystem::path default_path = "/default/path";
    auto result = tree.get("missing.path", default_path);
    EXPECT_EQ(result, default_path);
}

/***************************/
/*   Nested Path Tests     */
/***************************/
TEST(Settings_Tree, Deeply_Nested_Path) {
    Settings_Tree tree;

    tree.set("a.b.c.d.e", 42);
    EXPECT_EQ(tree.get<int>("a.b.c.d.e").value(), 42);
    EXPECT_TRUE(tree.has("a.b.c.d.e"));
    EXPECT_TRUE(tree.has("a.b.c"));
    EXPECT_TRUE(tree.has("a"));
}

TEST(Settings_Tree, Missing_Path_Returns_Nullopt) {
    Settings_Tree tree;

    EXPECT_FALSE(tree.get<int>("nonexistent").has_value());
    EXPECT_FALSE(tree.has("nonexistent"));
    EXPECT_FALSE(tree.has("also.missing.path"));
}

/***************************/
/*   Default Value Tests   */
/***************************/
TEST(Settings_Tree, Default_Value_When_Missing) {
    Settings_Tree tree;

    EXPECT_EQ(tree.get("missing.int", 100), 100);
    EXPECT_FLOAT_EQ(tree.get("missing.float", 3.14f), 3.14f);
    EXPECT_EQ(tree.get("missing.string", std::string("default")), "default");
    EXPECT_EQ(tree.get("missing.bool", true), true);
}

/***************************/
/*   Remove Tests          */
/***************************/
TEST(Settings_Tree, Remove_Existing_Path) {
    Settings_Tree tree;

    tree.set("temp.value", 123);
    EXPECT_TRUE(tree.has("temp.value"));

    tree.remove("temp.value");
    EXPECT_FALSE(tree.has("temp.value"));
}

TEST(Settings_Tree, Remove_Nested_Path) {
    Settings_Tree tree;

    tree.set("level1.level2.level3", 456);
    EXPECT_TRUE(tree.has("level1.level2.level3"));

    tree.remove("level1.level2.level3");
    EXPECT_FALSE(tree.has("level1.level2.level3"));
    // Parent should still exist
    EXPECT_TRUE(tree.has("level1.level2"));
}

/***************************/
/*   Child Tree Tests      */
/***************************/
TEST(Settings_Tree, Child_Tree_Extract) {
    Settings_Tree tree;

    tree.set("parent.child1", 1);
    tree.set("parent.child2", 2);
    tree.set("other.value", 99);

    auto child = tree.child("parent");
    EXPECT_EQ(child.get<int>("child1").value(), 1);
    EXPECT_EQ(child.get<int>("child2").value(), 2);
    EXPECT_FALSE(child.has("other"));  // Not in child subtree
}

TEST(Settings_Tree, Child_Tree_Missing_Returns_Empty) {
    Settings_Tree tree;

    auto child = tree.child("nonexistent");
    EXPECT_TRUE(child.empty());
}

/***************************/
/*   Merge Tests           */
/***************************/
TEST(Settings_Tree, Merge_Trees) {
    Settings_Tree tree1;
    tree1.set("shared.key", 1);
    tree1.set("tree1.only", "a");

    Settings_Tree tree2;
    tree2.set("shared.key", 2);  // Will overwrite
    tree2.set("tree2.only", "b");

    tree1.merge(tree2);

    EXPECT_EQ(tree1.get<int>("shared.key").value(), 2);  // Overwritten
    EXPECT_EQ(tree1.get<std::string>("tree1.only").value(), "a");
    EXPECT_EQ(tree1.get<std::string>("tree2.only").value(), "b");
}

/***************************/
/*   Empty/Clear Tests     */
/***************************/
TEST(Settings_Tree, Empty_Tree) {
    Settings_Tree tree;
    EXPECT_TRUE(tree.empty());

    tree.set("any.key", 1);
    EXPECT_FALSE(tree.empty());
}

TEST(Settings_Tree, Clear_Tree) {
    Settings_Tree tree;
    tree.set("key1", 1);
    tree.set("key2", 2);
    EXPECT_FALSE(tree.empty());

    tree.clear();
    EXPECT_TRUE(tree.empty());
    EXPECT_FALSE(tree.has("key1"));
    EXPECT_FALSE(tree.has("key2"));
}

/***************************/
/*   TOML Parsing Tests    */
/***************************/
TEST(Settings_Tree, Parse_From_TOML_String) {
    std::string toml_content = R"(
[general]
name = "TestApp"
version = 1

[display]
brightness = 85
theme = "dark"
)";

    std::istringstream iss(toml_content);
    toml::table table = toml::parse(iss);
    Settings_Tree tree(table);

    EXPECT_EQ(tree.get<std::string>("general.name").value(), "TestApp");
    EXPECT_EQ(tree.get<int>("general.version").value(), 1);
    EXPECT_EQ(tree.get<int>("display.brightness").value(), 85);
    EXPECT_EQ(tree.get<std::string>("display.theme").value(), "dark");
}

TEST(Settings_Tree, Parse_Sample_Config_File) {
    // Test with the actual sample config file
    std::filesystem::path config_path = "/Users/marvin/Desktop/Projects/kbd_calc/data/configs/settings.toml";

    // Check if file exists (it should in this test environment)
    if (!std::filesystem::exists(config_path)) {
        GTEST_SKIP() << "Config file not found at " << config_path;
    }

    toml::table table = toml::parse_file(config_path.string());
    Settings_Tree tree(table);

    // Verify expected settings from the template
    EXPECT_EQ(tree.get<std::string>("general.timezone").value_or(""), "auto");
    EXPECT_EQ(tree.get<int>("display.brightness").value_or(0), 80);
    EXPECT_EQ(tree.get<std::string>("display.theme").value_or(""), "dark");

    // Test centralized enum parsing
    auto angle_str = tree.get<std::string>("calculator.angle_mode").value_or("");
    auto angle_mode = ovb::core::angle_mode_from_string(angle_str);
    EXPECT_TRUE(angle_mode.has_value());
    EXPECT_EQ(*angle_mode, ovb::core::Angle_Mode::Degrees);
    EXPECT_EQ(ovb::core::to_string(*angle_mode), "degrees");

    auto format_str = tree.get<std::string>("calculator.number_format").value_or("");
    auto number_format = ovb::core::number_format_from_string(format_str);
    EXPECT_TRUE(number_format.has_value());
    EXPECT_EQ(*number_format, ovb::core::Number_Format::Auto);
    EXPECT_EQ(ovb::core::to_string(*number_format), "auto");

    EXPECT_EQ(tree.get<int>("calculator.decimal_places").value_or(0), 6);
}

/***************************/
/*   Data Access Tests     */
/***************************/
TEST(Settings_Tree, Direct_Data_Access) {
    Settings_Tree tree;
    tree.set("test.value", 42);

    // Access underlying TOML table
    toml::table& data = tree.data();
    EXPECT_TRUE(data.contains("test"));
}
