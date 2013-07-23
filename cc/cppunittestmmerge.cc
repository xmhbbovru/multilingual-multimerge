// cc/cppunittestmmerge.cc rev. 22 July 2013 by Stuart Ambler.
// Runs testmmerge_main using CppUnit.
// Copyright (c) 2013 Stuart Ambler.
// Distributed under the Boost License in the accompanying file LICENSE.

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include "testmmerge.h"

class CppUnitTestMMerge : public CppUnit::TestCase {
 public:
  CppUnitTestMMerge() : CppUnit::TestCase("CppUnitTestMMerge") {}

  static CppUnit::Test *getSuite() {
    CppUnit::TestSuite *suite
        = new CppUnit::TestSuite("CppUnitTestMMerge Suite");
    suite->addTest(new CppUnit::TestCaller<CppUnitTestMMerge>(
        "testDefaultArgs", &CppUnitTestMMerge::testDefaultArgs));
    return suite;
  }
  
 protected:
  void testDefaultArgs() {
    char *argv[] = { "./testmmerge" };
    CPPUNIT_ASSERT(testmmerge_main(1, argv) == 0);
  }
};

int main(int argc, char *argv[]) {
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(CppUnitTestMMerge::getSuite());
  runner.run();
  return 0;
}
