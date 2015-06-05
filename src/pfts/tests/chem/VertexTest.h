#ifndef VERTEX_TEST_H
#define VERTEX_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <pfts/chem/Block.h>
#include <pfts/chem/Vertex.h>

#include <fstream>

using namespace Pfts;
//using namespace Util;

class VertexTest : public UnitTest 
{

public:

   void setUp()
   {}

   void tearDown()
   {}

  
   void testConstructor()
   {
      printMethod(TEST_FUNC);
      Vertex v;
   } 

   void testAddBlock() {
      printMethod(TEST_FUNC);
      printEndl();

      Block b;
      std::ifstream in;
      openInputFile("in/Block", in);

      in >> b;
      TEST_ASSERT(b.id() == 5);
      TEST_ASSERT(b.monomerId() == 0);
      TEST_ASSERT(b.vertexId(0) == 3);
      TEST_ASSERT(b.vertexId(1) == 4);
      TEST_ASSERT(eq(b.length(), 2.0));
      //std::cout << b << std::endl;

      Vertex v;
      v.setId(3);
      v.addBlock(b);
      TEST_ASSERT(v.size() == 1);
      TEST_ASSERT(v.outSolverId(0)[0] == 5);
      TEST_ASSERT(v.outSolverId(0)[1] == 0);
      TEST_ASSERT(v.inSolverId(0)[0] == 5);
      TEST_ASSERT(v.inSolverId(0)[1] == 1);
      //std::cout << v.inSolverId(0)[0] << "  "
      //          << v.inSolverId(0)[1] << "\n";
      //std::cout << v.outSolverId(0)[0] << "  "
      //          << v.outSolverId(0)[1] << "\n";
      
   }

};

TEST_BEGIN(VertexTest)
TEST_ADD(VertexTest, testConstructor)
TEST_ADD(VertexTest, testAddBlock)
TEST_END(VertexTest)

#endif
