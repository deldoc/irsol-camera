#include "irsol/utils.hpp"

#include <catch2/catch_all.hpp>
#include <set>

TEST_CASE("toInt(protocol_value_t)", "[Utils]")
{
  auto rawValue      = GENERATE(-42, 0, 2, 1000);
  auto protocolValue = irsol::types::protocol_value_t{rawValue};
  CHECK(irsol::utils::toInt(protocolValue) == rawValue);
}

TEST_CASE("toDouble(protocol_value_t)", "[Utils]")
{
  auto rawValue      = GENERATE(-42.6, 0.3, 2.2, 1000.1231231);
  auto protocolValue = irsol::types::protocol_value_t{rawValue};
  CHECK(irsol::utils::toDouble(protocolValue) == Catch::Approx(rawValue));
}

TEST_CASE("toString(protocol_value_t)", "[Utils]")
{
  std::string rawValue      = GENERATE("", "hello", "hello world!");
  auto        protocolValue = irsol::types::protocol_value_t{rawValue};
  CHECK(irsol::utils::toString(protocolValue) == rawValue);
}

TEST_CASE("uuid()", "[Utils]")
{
  std::set<std::string> results;
  for(size_t i = 0; i < 10000; ++i) {
    auto uuid = irsol::utils::uuid();
    CHECK(results.find(uuid) == results.end());
    results.insert(uuid);
  }
}

TEST_CASE("split()", "[Utils]")
{

  std::string inString = "Hello world";
  {
    char                     delimiter = ' ';
    auto                     result    = irsol::utils::split(inString, delimiter);
    std::vector<std::string> expected{"Hello", "world"};
    CHECK(result == expected);
  }
  {
    char                     delimiter = '!';
    auto                     result    = irsol::utils::split(inString, delimiter);
    std::vector<std::string> expected{"Hello world"};
    CHECK(result == expected);
  }
  {
    char                     delimiter = 'l';
    auto                     result    = irsol::utils::split(inString, delimiter);
    std::vector<std::string> expected{"He", "o wor", "d"};
    CHECK(result == expected);
  }
}

TEST_CASE("strip()", "[Utils]")
{
  {
    std::string inString = " \tHello world\n \r";
    {
      auto        result   = irsol::utils::strip(inString);
      std::string expected = "Hello world";
      CHECK(result == expected);
    }

    {
      std::string delimiters = "d";
      auto        result     = irsol::utils::strip(inString, delimiters);
      std::string expected   = inString;
      CHECK(result == expected);
    }
  }
  {
    std::string inString = "Hello world";
    {
      auto        result   = irsol::utils::strip(inString);
      std::string expected = "Hello world";
      CHECK(result == expected);
    }

    {
      std::string delimiters = "d";
      auto        result     = irsol::utils::strip(inString, delimiters);
      std::string expected   = "Hello worl";
      CHECK(result == expected);
    }

    {
      std::string delimiters = "Hd";
      auto        result     = irsol::utils::strip(inString, delimiters);
      std::string expected   = "ello worl";
      CHECK(result == expected);
    }
    {
      std::string delimiters = "Hello ";
      auto        result     = irsol::utils::strip(inString, delimiters);
      std::string expected   = "world";
      CHECK(result == expected);
    }
  }
}

TEST_CASE("stripString()", "[Utils]")
{
  {
    std::string inString = "Hello world";
    {
      std::string strippedString = "";
      auto        result         = irsol::utils::stripString(inString, strippedString);
      std::string expected       = "Hello world";
      CHECK(result == expected);
    }

    {
      std::string strippedString = "d";
      auto        result         = irsol::utils::stripString(inString, strippedString);
      std::string expected       = "Hello worl";
      CHECK(result == expected);
    }

    {
      std::string strippedString = "Hd";
      auto        result         = irsol::utils::stripString(inString, strippedString);
      std::string expected       = "Hello world";
      CHECK(result == expected);
    }
    {
      std::string strippedString = "Hello ";
      auto        result         = irsol::utils::stripString(inString, strippedString);
      std::string expected       = "world";
      CHECK(result == expected);
    }
  }
}
TEST_CASE("durationToString()", "[Utils]")
{
  {
    irsol::types::duration_t duration{};
    CHECK(irsol::utils::durationToString(duration) == "0ns");
  }
  {
    irsol::types::duration_t duration = std::chrono::nanoseconds(4);
    CHECK(irsol::utils::durationToString(duration) == "4ns");
  }
  {
    irsol::types::duration_t duration = std::chrono::nanoseconds(4000);
    CHECK(irsol::utils::durationToString(duration) == "4us");
  }
  {
    irsol::types::duration_t duration = std::chrono::nanoseconds(4030);
    CHECK(irsol::utils::durationToString(duration) == "4us 030ns");
  }
  {
    irsol::types::duration_t duration = std::chrono::microseconds(234);
    CHECK(irsol::utils::durationToString(duration) == "234us");
  }
  {
    irsol::types::duration_t duration = std::chrono::microseconds(23442);
    CHECK(irsol::utils::durationToString(duration) == "23ms 442us");
  }
  {
    irsol::types::duration_t duration = std::chrono::milliseconds(499);
    CHECK(irsol::utils::durationToString(duration) == "499ms");
  }
  {
    irsol::types::duration_t duration = std::chrono::seconds(16);
    CHECK(irsol::utils::durationToString(duration) == "16s");
  }
  {
    irsol::types::duration_t duration = std::chrono::seconds(200);
    CHECK(irsol::utils::durationToString(duration) == "3minutes 20s");
  }
  {
    irsol::types::duration_t duration = std::chrono::minutes(54);
    CHECK(irsol::utils::durationToString(duration) == "54minutes");
  }
  {
    irsol::types::duration_t duration = std::chrono::hours(270);
    CHECK(irsol::utils::durationToString(duration) == "270hours");
  }
}

TEST_CASE("stringToBytes()", "[Utils]")
{
  {
    std::string empty;
    auto        result = irsol::utils::stringToBytes(empty);
    CHECK(result.size() == 0);
  }
  {
    std::string singleChar = "a";
    auto        result     = irsol::utils::stringToBytes(singleChar);
    CHECK(result.size() == 1);
    CHECK(result[0] == irsol::types::byte_t{'a'});
  }
  {
    std::string longerString = GENERATE("hello", "world", "How are you?");
    auto        result       = irsol::utils::stringToBytes(longerString);
    CHECK(result.size() == longerString.size());
    for(size_t i = 0; i < longerString.size(); ++i) {
      CHECK(
        result[i] ==
        static_cast<irsol::types::byte_t>(static_cast<unsigned char>(longerString[i])));
    }
  }
}

TEST_CASE("bytesToString()", "[Utils]")
{
  {
    std::vector<irsol::types::byte_t> bytes;
    auto                              result = irsol::utils::bytesToString(bytes);
    CHECK(result.size() == 0);
  }
  {
    std::vector<irsol::types::byte_t> bytes{irsol::types::byte_t{'a'}};
    auto                              result = irsol::utils::bytesToString(bytes);
    CHECK(result.size() == 1);
    CHECK(result == "a");
  }
  {
    std::vector<irsol::types::byte_t> bytes{irsol::types::byte_t{'H'},
                                            irsol::types::byte_t{'e'},
                                            irsol::types::byte_t{'l'},
                                            irsol::types::byte_t{'l'},
                                            irsol::types::byte_t{'o'},
                                            irsol::types::byte_t{' '},
                                            irsol::types::byte_t{'!'}};
    auto                              result = irsol::utils::bytesToString(bytes);
    CHECK(result == "Hello !");
  }
}