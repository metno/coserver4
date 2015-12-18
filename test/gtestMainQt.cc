
#include <QCoreApplication>

#include <gtest/gtest.h>

//#define DEBUG_MESSAGES

#ifdef DEBUG_MESSAGES
#include <log4cpp/Category.hh>
#endif // DEBUG_MESSAGES

#define MILOGGER_CATEGORY "coserver4.test"
#include <miLogger/miLogging.h>

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  milogger::LoggingConfig log4cpp("-.!!=-:");
#ifdef DEBUG_MESSAGES
  log4cpp::Category::getRoot().setPriority(log4cpp::Priority::DEBUG);
#endif

  QCoreApplication app(argc, argv);
  setlocale(LC_NUMERIC, "C");

  return RUN_ALL_TESTS();
}
