#define CATCH_CONFIG_RUNNER
#include "args/args.hpp"
#include "irsol/logging.hpp"

#include <catch2/catch_all.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

class LoggingListener : public Catch::EventListenerBase
{
public:
  using Catch::EventListenerBase::EventListenerBase;

  void testRunStarting(Catch::TestRunInfo const& testRunInfo) override
  {
    irsol::setLoggerName("unit-tests");
    irsol::setLoggingFormat(irsol::LoggingFormat::SIMPLE);
    IRSOL_LOG_INFO("Starting unittests.");
  }
  void testRunEnded(Catch::TestRunStats const& testRunStats) override
  {
    irsol::setLoggerName("unit-tests");
    IRSOL_LOG_INFO("Test run ended.");
  }

  void testCaseStarting(Catch::TestCaseInfo const& testInfo) override
  {
    std::string testCaseDescription = "";
    for(const auto& tag : testInfo.tags) {
      testCaseDescription += tag.original + "][";
    }
    if(!testCaseDescription.empty()) {
      testCaseDescription.pop_back();  // remove trailing '['
      testCaseDescription += "[" + testInfo.name;
    } else {
      testCaseDescription = testInfo.name;
    }

    irsol::setLoggerName(testCaseDescription.c_str());
    IRSOL_LOG_INFO("Starting testcase: {}-{}.", testInfo.tagsAsString(), testInfo.name);
  }
  void testCaseEnded(Catch::TestCaseStats const& stats) override
  {
    IRSOL_LOG_INFO("Testcase ended.");
  }
};

CATCH_REGISTER_LISTENER(LoggingListener)

std::vector<std::string>
configureLogging(int argc, char** argv)
{
  std::vector<std::string> filteredArgs;

  std::string logLevelStr    = "";
  bool        shouldSkipNext = false;
  for(int i = 0; i < argc; ++i) {
    if(shouldSkipNext) {
      shouldSkipNext = false;
      continue;
    }
    std::string arg = argv[i];
    if(arg == "--log-level") {
      if(i + 1 >= argc) {
        std::cerr << "Error: --log-level requires an argument\n";
        std::exit(1);
      }
      logLevelStr    = argv[i + 1];
      shouldSkipNext = true;
    } else {
      filteredArgs.emplace_back(argv[i]);
    }
  }

  spdlog::level::level_enum parsedLoglevel;
  if(logLevelStr != "") {
    parsedLoglevel = spdlog::level::from_str(logLevelStr);
    if(parsedLoglevel == spdlog::level::off) {
      std::cerr << "Error: Invalid log level provided: '" << logLevelStr << "'\n";
      std::exit(1);
    }
    irsol::initLogging("log/unit-tests.log", parsedLoglevel);
  } else {
    // Logging disabled
    std::cout << "Logging disabled.\n";
    irsol::initLogging("log/unit-tests.log", spdlog::level::off);
  }

  return filteredArgs;
}

int
main(int argc, char** argv)
{
  auto filteredArgs = configureLogging(argc, argv);
  // Convert to char* array for Catch2
  std::vector<char*> filteredArgsForCatch;
  for(auto& s : filteredArgs) {
    filteredArgsForCatch.push_back(s.data());
  }

  int result = Catch::Session().run(
    static_cast<int>(filteredArgsForCatch.size()), filteredArgsForCatch.data());

  return result;
}
