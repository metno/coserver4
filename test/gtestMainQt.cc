
#include <QCoreApplication>

#include <gtest/gtest.h>

#define MILOGGER_CATEGORY "coserver4.test"
#include <miLogger/miLogging.h>

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  milogger::LoggingConfig log4cpp("-.!!=-:");

  QCoreApplication app(argc, argv);
  setlocale(LC_NUMERIC, "C");

  return RUN_ALL_TESTS();
}
