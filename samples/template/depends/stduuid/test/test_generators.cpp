#include "uuid.h"
#include "catch.hpp"

#include <set>
#include <unordered_set>
#include <random>
#include <vector>

using namespace uuids;

TEST_CASE("Test multiple default generators", "[gen][rand]")
{
   uuid id1;
   uuid id2;

   {
      std::random_device rd;
      auto seed_data = std::array<int, std::mt19937::state_size> {};
      std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
      std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
      std::mt19937 generator(seq);

      id1 = uuids::uuid_random_generator{ generator }();
      REQUIRE(!id1.is_nil());
      REQUIRE(id1.version() == uuids::uuid_version::random_number_based);
      REQUIRE(id1.variant() == uuids::uuid_variant::rfc);
   }

   {
      std::random_device rd;
      auto seed_data = std::array<int, std::mt19937::state_size> {};
      std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
      std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
      std::mt19937 generator(seq);

      id2 = uuids::uuid_random_generator{ generator }();
      REQUIRE(!id2.is_nil());
      REQUIRE(id2.version() == uuids::uuid_version::random_number_based);
      REQUIRE(id2.variant() == uuids::uuid_variant::rfc);
   }

   REQUIRE(id1 != id2);
}

TEST_CASE("Test default generator", "[gen][rand]")
{
   std::random_device rd;
   auto seed_data = std::array<int, std::mt19937::state_size> {};
   std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
   std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
   std::mt19937 generator(seq);

   uuid const guid = uuids::uuid_random_generator{generator}();
   REQUIRE(!guid.is_nil());
   REQUIRE(guid.version() == uuids::uuid_version::random_number_based);
   REQUIRE(guid.variant() == uuids::uuid_variant::rfc);
}

TEST_CASE("Test random generator (conversion ctor w/ smart ptr)", "[gen][rand]")
{
   std::random_device rd;
   auto seed_data = std::array<int, std::mt19937::state_size> {};
   std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
   std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
   std::mt19937 generator(seq);

   uuids::uuid_random_generator dgen(&generator);
   auto id1 = dgen();
   REQUIRE(!id1.is_nil());
   REQUIRE(id1.version() == uuids::uuid_version::random_number_based);
   REQUIRE(id1.variant() == uuids::uuid_variant::rfc);

   auto id2 = dgen();
   REQUIRE(!id2.is_nil());
   REQUIRE(id2.version() == uuids::uuid_version::random_number_based);
   REQUIRE(id2.variant() == uuids::uuid_variant::rfc);

   REQUIRE(id1 != id2);
}

TEST_CASE("Test random generator (conversion ctor w/ ptr)", "[gen][rand]")
{
   std::random_device rd;
   auto seed_data = std::array<int, std::mt19937::state_size> {};
   std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
   std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
   auto generator = std::make_unique<std::mt19937>(seq);

   uuids::uuid_random_generator dgen(generator.get());
   auto id1 = dgen();
   REQUIRE(!id1.is_nil());
   REQUIRE(id1.version() == uuids::uuid_version::random_number_based);
   REQUIRE(id1.variant() == uuids::uuid_variant::rfc);

   auto id2 = dgen();
   REQUIRE(!id1.is_nil());
   REQUIRE(id2.version() == uuids::uuid_version::random_number_based);
   REQUIRE(id2.variant() == uuids::uuid_variant::rfc);

   REQUIRE(id1 != id2);
}

TEST_CASE("Test random generator (conversion ctor w/ ref)", "[gen][rand]")
{
   std::random_device rd;
   auto seed_data = std::array<int, std::mt19937::state_size> {};
   std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
   std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
   std::mt19937 generator(seq);

   uuids::uuid_random_generator dgen(generator);
   auto id1 = dgen();
   REQUIRE(!id1.is_nil());
   REQUIRE(id1.version() == uuids::uuid_version::random_number_based);
   REQUIRE(id1.variant() == uuids::uuid_variant::rfc);

   auto id2 = dgen();
   REQUIRE(!id2.is_nil());
   REQUIRE(id2.version() == uuids::uuid_version::random_number_based);
   REQUIRE(id2.variant() == uuids::uuid_variant::rfc);

   REQUIRE(id1 != id2);
}

TEST_CASE("Test basic random generator (conversion ctor w/ ptr) w/ ranlux48_base", "[gen][rand]")
{
   std::random_device rd;
   auto seed_data = std::array<int, 6> {};
   std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
   std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
   std::ranlux48_base generator(seq);

   uuids::basic_uuid_random_generator<std::ranlux48_base> dgen(&generator);
   auto id1 = dgen();
   REQUIRE(!id1.is_nil());
   REQUIRE(id1.version() == uuids::uuid_version::random_number_based);
   REQUIRE(id1.variant() == uuids::uuid_variant::rfc);

   auto id2 = dgen();
   REQUIRE(!id2.is_nil());
   REQUIRE(id2.version() == uuids::uuid_version::random_number_based);
   REQUIRE(id2.variant() == uuids::uuid_variant::rfc);

   REQUIRE(id1 != id2);
}

TEST_CASE("Test basic random generator (conversion ctor w/ smart ptr) w/ ranlux48_base", "[gen][rand]")
{
   std::random_device rd;
   auto seed_data = std::array<int, 6> {};
   std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
   std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
   auto generator = std::make_unique<std::ranlux48_base>(seq);

   uuids::basic_uuid_random_generator<std::ranlux48_base> dgen(generator.get());
   auto id1 = dgen();
   REQUIRE(!id1.is_nil());
   REQUIRE(id1.version() == uuids::uuid_version::random_number_based);
   REQUIRE(id1.variant() == uuids::uuid_variant::rfc);

   auto id2 = dgen();
   REQUIRE(!id2.is_nil());
   REQUIRE(id2.version() == uuids::uuid_version::random_number_based);
   REQUIRE(id2.variant() == uuids::uuid_variant::rfc);

   REQUIRE(id1 != id2);
}

TEST_CASE("Test basic random generator (conversion ctor w/ ref) w/ ranlux48_base", "[gen][rand]")
{
   std::random_device rd;
   auto seed_data = std::array<int, 6> {};
   std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
   std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
   std::ranlux48_base generator(seq);

   uuids::basic_uuid_random_generator<std::ranlux48_base> dgen(generator);
   auto id1 = dgen();
   REQUIRE(!id1.is_nil());
   REQUIRE(id1.version() == uuids::uuid_version::random_number_based);
   REQUIRE(id1.variant() == uuids::uuid_variant::rfc);

   auto id2 = dgen();
   REQUIRE(!id2.is_nil());
   REQUIRE(id2.version() == uuids::uuid_version::random_number_based);
   REQUIRE(id2.variant() == uuids::uuid_variant::rfc);

   REQUIRE(id1 != id2);
}

TEST_CASE("Test namespaces", "[gen][name]")
{
   REQUIRE(uuid_namespace_dns == uuids::uuid::from_string("6ba7b810-9dad-11d1-80b4-00c04fd430c8"));
   REQUIRE(uuid_namespace_url == uuids::uuid::from_string("6ba7b811-9dad-11d1-80b4-00c04fd430c8"));
   REQUIRE(uuid_namespace_oid == uuids::uuid::from_string("6ba7b812-9dad-11d1-80b4-00c04fd430c8"));
   REQUIRE(uuid_namespace_x500 == uuids::uuid::from_string("6ba7b814-9dad-11d1-80b4-00c04fd430c8"));
}

TEST_CASE("Test name generator (char*)", "[gen][name]")
{
   uuids::uuid_name_generator dgen(uuids::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43").value());
   auto id1 = dgen("john");
   REQUIRE(!id1.is_nil());
   REQUIRE(id1.version() == uuids::uuid_version::name_based_sha1);
   REQUIRE(id1.variant() == uuids::uuid_variant::rfc);

   auto id2 = dgen("jane");
   REQUIRE(!id2.is_nil());
   REQUIRE(id2.version() == uuids::uuid_version::name_based_sha1);
   REQUIRE(id2.variant() == uuids::uuid_variant::rfc);

   auto id3 = dgen("jane");
   REQUIRE(!id3.is_nil());
   REQUIRE(id3.version() == uuids::uuid_version::name_based_sha1);
   REQUIRE(id3.variant() == uuids::uuid_variant::rfc);

   auto id4 = dgen(L"jane");
   REQUIRE(!id4.is_nil());
   REQUIRE(id4.version() == uuids::uuid_version::name_based_sha1);
   REQUIRE(id4.variant() == uuids::uuid_variant::rfc);

   REQUIRE(id1 != id2);
   REQUIRE(id2 == id3);
   REQUIRE(id3 != id4);
}

TEST_CASE("Test name generator (std::string)", "[gen][name]")
{
   using namespace std::string_literals;

   uuids::uuid_name_generator dgen(uuids::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43").value());
   auto id1 = dgen("john"s);
   REQUIRE(!id1.is_nil());
   REQUIRE(id1.version() == uuids::uuid_version::name_based_sha1);
   REQUIRE(id1.variant() == uuids::uuid_variant::rfc);

   auto id2 = dgen("jane"s);
   REQUIRE(!id2.is_nil());
   REQUIRE(id2.version() == uuids::uuid_version::name_based_sha1);
   REQUIRE(id2.variant() == uuids::uuid_variant::rfc);

   auto id3 = dgen("jane"s);
   REQUIRE(!id3.is_nil());
   REQUIRE(id3.version() == uuids::uuid_version::name_based_sha1);
   REQUIRE(id3.variant() == uuids::uuid_variant::rfc);

   auto id4 = dgen(L"jane"s);
   REQUIRE(!id4.is_nil());
   REQUIRE(id4.version() == uuids::uuid_version::name_based_sha1);
   REQUIRE(id4.variant() == uuids::uuid_variant::rfc);

   REQUIRE(id1 != id2);
   REQUIRE(id2 == id3);
   REQUIRE(id3 != id4);
}

TEST_CASE("Test name generator (std::string_view)", "[gen][name]")
{
   using namespace std::string_view_literals;

   uuids::uuid_name_generator dgen(uuids::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43").value());
   auto id1 = dgen("john"sv);
   REQUIRE(!id1.is_nil());
   REQUIRE(id1.version() == uuids::uuid_version::name_based_sha1);
   REQUIRE(id1.variant() == uuids::uuid_variant::rfc);

   auto id2 = dgen("jane"sv);
   REQUIRE(!id2.is_nil());
   REQUIRE(id2.version() == uuids::uuid_version::name_based_sha1);
   REQUIRE(id2.variant() == uuids::uuid_variant::rfc);

   auto id3 = dgen("jane"sv);
   REQUIRE(!id3.is_nil());
   REQUIRE(id3.version() == uuids::uuid_version::name_based_sha1);
   REQUIRE(id3.variant() == uuids::uuid_variant::rfc);

   auto id4 = dgen(L"jane"sv);
   REQUIRE(!id4.is_nil());
   REQUIRE(id4.version() == uuids::uuid_version::name_based_sha1);
   REQUIRE(id4.variant() == uuids::uuid_variant::rfc);

   REQUIRE(id1 != id2);
   REQUIRE(id2 == id3);
   REQUIRE(id3 != id4);
}

TEST_CASE("Test name generator equality (char const*, std::string, std::string_view)", "[gen][name]")
{
   using namespace std::literals;

   uuids::uuid_name_generator dgen(uuids::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43").value());
   auto id1 = dgen("john");
   auto id2 = dgen("john"s);
   auto id3 = dgen("john"sv);

   REQUIRE(id1 == id2);
   REQUIRE(id2 == id3);
}

#ifdef UUID_TIME_GENERATOR
TEST_CASE("Test time generator", "[gen][time]")
{
   uuid_time_generator gen;
   auto id1 = gen();
   auto id2 = gen();
   REQUIRE(!id1.is_nil());
   REQUIRE(id1.variant() == uuids::uuid_variant::rfc);
   REQUIRE(id1.version() == uuids::uuid_version::time_based);

   REQUIRE(!id2.is_nil());
   REQUIRE(id2.variant() == uuids::uuid_variant::rfc);
   REQUIRE(id2.version() == uuids::uuid_version::time_based);

   REQUIRE(id1 != id2);

   std::set<uuids::uuid> ids;
   for (int i = 0; i < 100; ++i)
      ids.insert(gen());

   REQUIRE(ids.size() == 100);
}
#endif