#include <gtest/gtest.h>
#include "railcore/types.h"
#include "railcore/status.h"
#include "railcore/services.h"
#include "railcore/persistence/rcd_id.h"

using namespace RailCore;

// Basic sanity tests that don't require file I/O

TEST(StatusTest, StatusCodeEnumValues) {
    EXPECT_NE(StatusCode::Ok, StatusCode::NotFound);
    EXPECT_NE(StatusCode::Ok, StatusCode::ValidationError);
    EXPECT_NE(StatusCode::Ok, StatusCode::InternalError);
}

TEST(StatusTest, CreateOkStatus) {
    Status s{StatusCode::Ok, "Success"};
    EXPECT_EQ(s.code, StatusCode::Ok);
    EXPECT_EQ(s.message, "Success");
}

TEST(StatusTest, CreateErrorStatus) {
    Status s{StatusCode::NotFound, "File not found"};
    EXPECT_EQ(s.code, StatusCode::NotFound);
    EXPECT_EQ(s.message, "File not found");
}

TEST(RcdIdTest, CanonicalizeEmptyString) {
    std::string input = "";
    std::string output = CanonicalizeRcdContent(input);
    EXPECT_EQ(output, "");
}

TEST(RcdIdTest, CanonicalizeSimpleString) {
    std::string input = "Hello World";
    std::string output = CanonicalizeRcdContent(input);
    EXPECT_FALSE(output.empty());
}

TEST(RcdIdTest, CanonicalizeTrimsTrailingSpaces) {
    std::string input = "Line1   \nLine2   \n";
    std::string output = CanonicalizeRcdContent(input);
    // Canonicalization should remove trailing spaces
    EXPECT_EQ(output.find("   \n"), std::string::npos);
}

TEST(RcdIdTest, CanonicalizeCRLF) {
    std::string input = "Line1\r\nLine2\r\n";
    std::string output = CanonicalizeRcdContent(input);
    // Should normalize to LF
    EXPECT_EQ(output.find("\r"), std::string::npos);
}

TEST(RcdIdTest, ComputeIdForSameContent) {
    std::string content = "[GENERAL]\nName, Test\n";
    std::string canon = CanonicalizeRcdContent(content);
    std::string id1 = ComputeRcdIdFromContent(canon);
    std::string id2 = ComputeRcdIdFromContent(canon);

    EXPECT_FALSE(id1.empty());
    EXPECT_EQ(id1.size(), 16); // Should be 16-character hex string
    EXPECT_EQ(id1, id2); // Same content should give same ID
}

TEST(RcdIdTest, ComputeIdForDifferentContent) {
    std::string content1 = "[GENERAL]\nName, Test1\n";
    std::string content2 = "[GENERAL]\nName, Test2\n";

    std::string id1 = ComputeRcdIdFromContent(CanonicalizeRcdContent(content1));
    std::string id2 = ComputeRcdIdFromContent(CanonicalizeRcdContent(content2));

    EXPECT_NE(id1, id2); // Different content should give different IDs
}

// Test that types are defined correctly
TEST(TypesTest, SimulationClockCanBeConstructed) {
    SimulationClock clock{100};
    EXPECT_EQ(clock.count(), 100);
}

TEST(TypesTest, LayoutDescriptorDefaults) {
    LayoutDescriptor desc;
    EXPECT_TRUE(desc.name.empty());
    EXPECT_TRUE(desc.sourcePath.empty());
}

TEST(TypesTest, WorldStateDefaults) {
    WorldState ws;
    EXPECT_EQ(ws.tickId, 0);
    EXPECT_TRUE(ws.sections.empty());
    EXPECT_TRUE(ws.routes.empty());
    EXPECT_TRUE(ws.timetable.empty());
    EXPECT_TRUE(ws.locos.empty());
    EXPECT_FALSE(ws.simulationActive);
}

TEST(TypesTest, SectionConstruction) {
    Section s{42, "Test Section"};
    EXPECT_EQ(s.id, 42);
    EXPECT_EQ(s.name, "Test Section");
}

TEST(TypesTest, RouteConstruction) {
    Route r{100, "Main Route"};
    EXPECT_EQ(r.id, 100);
    EXPECT_EQ(r.name, "Main Route");
}

TEST(TypesTest, LocoConstruction) {
    Loco l{7, "Express Locomotive"};
    EXPECT_EQ(l.id, 7);
    EXPECT_EQ(l.name, "Express Locomotive");
}

TEST(TypesTest, TimetableEntryConstruction) {
    TimetableEntry te{5, "Morning Service"};
    EXPECT_EQ(te.id, 5);
    EXPECT_EQ(te.name, "Morning Service");
}

TEST(TypesTest, DelaySettingsDefaults) {
    DelaySettings ds;
    EXPECT_EQ(ds.mode, DelayMode::None);
    EXPECT_EQ(ds.threshold.count(), 0);
    EXPECT_FALSE(ds.maintenanceThrough);
}

TEST(TypesTest, DelayModeEnumValues) {
    EXPECT_NE(DelayMode::None, DelayMode::Randomized);
    EXPECT_NE(DelayMode::None, DelayMode::MaintenanceOnly);
    EXPECT_NE(DelayMode::Randomized, DelayMode::MaintenanceOnly);
}
