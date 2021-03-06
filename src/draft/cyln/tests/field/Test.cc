/*
* This program runs all unit tests in the pssp/tests/fftw directory.
*/ 

#include <util/global.h>
//#include "RFieldTest.h"
#include "FieldTestComposite.h"

#include <test/TestRunner.h>
#include <test/CompositeTestRunner.h>

int main(int argc, char* argv[])
{
   //TEST_RUNNER(RMeshFieldTest) runner;
   FieldTestComposite runner;

   #if 0
   if (argc > 2) {
      UTIL_THROW("Too many arguments");
   }
   if (argc == 2) {
      runner.addFilePrefix(argv[1]);
   }
   #endif

   runner.run();
}
