
#include <cmath>
#include <cstring>
#include <stdexcept>

#include "common.hpp"

class TestHandler100 : public osmium::handler::Handler {

public:

    TestHandler100() :
        osmium::handler::Handler() {
    }

    void node(const osmium::Node& node) {
        constexpr const double epsilon = 0.00000001;
        if (node.id() == 100000) {
            REQUIRE(node.version() == 1);
            REQUIRE(node.timestamp() == osmium::Timestamp{"2014-01-01T00:00:00Z"});
            REQUIRE(node.uid() == 1);
            REQUIRE(!std::strcmp(node.user(), "test"));
            REQUIRE(node.changeset() == 1);
            REQUIRE(std::abs(node.location().lon() - 1.02) < epsilon);
            REQUIRE(std::abs(node.location().lat() - 1.02) < epsilon);
        } else {
            throw std::runtime_error{"Unknown ID"};
        }
    }

}; // class TestHandler100

TEST_CASE("100") {

    SECTION("test 100") {
        osmium::io::Reader reader{dirname + "/1/100/data.osm"};

        CheckBasicsHandler check_basics_handler{100, 1, 0, 0};
        CheckWKTHandler check_wkt_handler{dirname, 100};
        TestHandler100 test_handler;

        osmium::apply(reader, check_basics_handler, check_wkt_handler, test_handler);
    }

}

