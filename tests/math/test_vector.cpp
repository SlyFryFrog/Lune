#include <catch.hpp>
import lune;

using namespace lune;

TEST_CASE("Vec2 basic arithmetic", "[Vec2]") {
    Vec2 a{1.0f, 2.0f};
    Vec2 b{3.0f, 4.0f};

    REQUIRE((a + b).x == Catch::Approx(4.0f));
    REQUIRE((a + b).y == Catch::Approx(6.0f));
    REQUIRE((b - a).x == Catch::Approx(2.0f));
    REQUIRE((b - a).y == Catch::Approx(2.0f));
    REQUIRE((a * 2.0f).x == Catch::Approx(2.0f));
    REQUIRE((a * 2.0f).y == Catch::Approx(4.0f));
    REQUIRE((b / 2.0f).x == Catch::Approx(1.5f));
    REQUIRE((b / 2.0f).y == Catch::Approx(2.0f));
}

TEST_CASE("Vec2 length, dot, normalize", "[Vec2]") {
    Vec2 v{3.0f, 4.0f};
    REQUIRE(v.length() == Catch::Approx(5.0f));
    REQUIRE(v.dot(v) == Catch::Approx(25.0f));

    Vec2 n = v.normalize();
    REQUIRE(n.length() == Catch::Approx(1.0f));
    REQUIRE(n.x == Catch::Approx(0.6f));
    REQUIRE(n.y == Catch::Approx(0.8f));

    Vec2 zero{};
    REQUIRE(zero.normalize().length() == Catch::Approx(0.0f));
}

TEST_CASE("Vec2 min, max, clamp, lerp", "[Vec2]") {
    Vec2 a{1.0f, 5.0f};
    Vec2 b{3.0f, 2.0f};

    Vec2 mn = Vec2::min(a, b);
    REQUIRE(mn.x == Catch::Approx(1.0f));
    REQUIRE(mn.y == Catch::Approx(2.0f));

    Vec2 mx = Vec2::max(a, b);
    REQUIRE(mx.x == Catch::Approx(3.0f));
    REQUIRE(mx.y == Catch::Approx(5.0f));

    Vec2 cl = Vec2::clamp(Vec2{2.0f, 6.0f}, Vec2{1.0f, 2.0f}, Vec2{3.0f, 5.0f});
    REQUIRE(cl.x == Catch::Approx(2.0f));
    REQUIRE(cl.y == Catch::Approx(5.0f));

    Vec2 lr = Vec2::lerp(a, b, 0.5f);
    REQUIRE(lr.x == Catch::Approx(2.0f));
    REQUIRE(lr.y == Catch::Approx(3.5f));
}

TEST_CASE("Vec3 basic arithmetic & length", "[Vec3]") {
    Vec3 a{1,2,3};
    Vec3 b{4,5,6};

    REQUIRE((a + b).x == Catch::Approx(5.0f));
    REQUIRE((a + b).y == Catch::Approx(7.0f));
    REQUIRE((a + b).z == Catch::Approx(9.0f));

    REQUIRE((b - a).x == Catch::Approx(3.0f));
    REQUIRE((b - a).y == Catch::Approx(3.0f));
    REQUIRE((b - a).z == Catch::Approx(3.0f));

    REQUIRE(a.dot(b) == Catch::Approx(32.0f));
    Vec3 c = a.cross(b);
    REQUIRE(c.x == Catch::Approx(-3.0f));
    REQUIRE(c.y == Catch::Approx(6.0f));
    REQUIRE(c.z == Catch::Approx(-3.0f));

    REQUIRE(a.length() == Catch::Approx(std::sqrt(14.0f)));
    REQUIRE(a.normalized().length() == Catch::Approx(1.0f));
}

TEST_CASE("Vec3 min, max, clamp, lerp, distance", "[Vec3]") {
    Vec3 a{1,2,3};
    Vec3 b{4,0,6};

    Vec3 mn = Vec3::min(a,b);
    REQUIRE(mn.x == Catch::Approx(1.0f));
    REQUIRE(mn.y == Catch::Approx(0.0f));
    REQUIRE(mn.z == Catch::Approx(3.0f));

    Vec3 mx = Vec3::max(a,b);
    REQUIRE(mx.x == Catch::Approx(4.0f));
    REQUIRE(mx.y == Catch::Approx(2.0f));
    REQUIRE(mx.z == Catch::Approx(6.0f));

    Vec3 cl = Vec3::clamp(Vec3{2,3,5}, Vec3{1,1,1}, Vec3{3,2,6});
    REQUIRE(cl.x == Catch::Approx(2.0f));
    REQUIRE(cl.y == Catch::Approx(2.0f));
    REQUIRE(cl.z == Catch::Approx(5.0f));

    Vec3 lr = Vec3::lerp(a,b,0.5f);
    REQUIRE(lr.x == Catch::Approx(2.5f));
    REQUIRE(lr.y == Catch::Approx(1.0f));
    REQUIRE(lr.z == Catch::Approx(4.5f));

    REQUIRE(a.distance(b) == Catch::Approx(std::sqrt(3*3 + 2*2 + 3*3)));
}

TEST_CASE("Vec4 basic arithmetic & length", "[Vec4]") {
    Vec4 a{1,2,3,4};
    Vec4 b{4,5,6,7};

    REQUIRE((a + b).w == Catch::Approx(11.0f));
    REQUIRE((a - b).x == Catch::Approx(-3.0f));
    REQUIRE((a * 2.0f).z == Catch::Approx(6.0f));
    REQUIRE((b / 2.0f).y == Catch::Approx(2.5f));

    REQUIRE(a.dot(b) == Catch::Approx(1*4 + 2*5 + 3*6 + 4*7));
    REQUIRE(a.length() == Catch::Approx(std::sqrt(30.0f)));
    REQUIRE(a.normalize().length() == Catch::Approx(1.0f));
}
