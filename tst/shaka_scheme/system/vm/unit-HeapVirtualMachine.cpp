//
// Created by Billy Wooton on 9/22/17.
//

#include <gmock/gmock.h>

TEST(HeapVirtualMachineUnitTests, const) {
  // Given: An assembly expression of the form (const "hello" (halt))

  // When: We evaluate this assembly instruction by one step

  // Then: The accumulator register contains the string "hello"
  // Then: The next expression register contains the (halt) instruction

}

