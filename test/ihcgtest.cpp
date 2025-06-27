// Header for GoogleTest and Intel HEX Class for testing
#include <gtest/gtest.h>
#include <iostream>
#include "../intelhex_class/intelhexclass.h"

/******************************************************************************/
// Tests for segment address methods
/*****************************************************************************/
namespace {
    intelhex classTest;
    unsigned short ipRegister = 0x00;
    unsigned short csRegister = 0x00;
    unsigned long  eipRegister = 0x00;
    unsigned long  memoryAddress = 0x00;
    unsigned char  memoryData = 0x00;
    
    TEST(SegmentAddress, NoSegmentAddress) {
        EXPECT_EQ(false, classTest.getStartSegmentAddress(&ipRegister, &csRegister));
        EXPECT_EQ(0x00, ipRegister);
        EXPECT_EQ(0x00, csRegister);
    }

    TEST(SegmentAddress, SetAndGetSegmentAddress) {
        classTest.setStartSegmentAddress(0x1234, 0xABCD);
        EXPECT_EQ(true, classTest.getStartSegmentAddress(&ipRegister, &csRegister));
        EXPECT_EQ(0x1234, ipRegister);
        EXPECT_EQ(0xABCD, csRegister);
    }

    TEST(LinearAddress, NoLinearAddress) {
        EXPECT_EQ(false, classTest.getStartLinearAddress(&eipRegister));
        EXPECT_EQ(0x00, eipRegister);
    }

    TEST(LinearAddress, SetAndGetLinearAddress) {
        classTest.setStartLinearAddress(0x12345678);
        EXPECT_EQ(true, classTest.getStartLinearAddress(&eipRegister));
        EXPECT_EQ(0x12345678, eipRegister);
    }
    
    TEST(MemorySpace, CheckMemoryEmpty01) {
        EXPECT_EQ(true, classTest.empty());
    }
    
    TEST(MemorySpace, CheckMemoryEmpty02) { 
        EXPECT_EQ(0, classTest.size());
    }

    TEST(MemorySpace, CheckMemoryEmpty03) {
        EXPECT_EQ(false, classTest.jumpTo(0x00));
    }

    TEST(MemorySpace, CheckMemoryEmpty04) {
        EXPECT_EQ(false, classTest.incrementAddress());
    }

    TEST(MemorySpace, CheckMemoryEmpty05) {
        EXPECT_EQ(false, classTest.decrementAddress());
    }

    TEST(MemorySpace, CheckMemoryEmpty06) {
//        memoryAddress = 0x00;
// TODO: The method currentAddress() should really return false if 
// the memory is empty and an address when it has one.
//        EXPECT_EQ(false, classTest.currentAddress(&memoryAddress));
        EXPECT_EQ(1, classTest.currentAddress());
//        EXPECT_EQ(0, memoryAddress);
    }

    TEST(MemorySpace, CheckMemoryEmpty07) {
        memoryAddress = 0x00;
        EXPECT_EQ(false, classTest.startAddress(&memoryAddress));
        EXPECT_EQ(0, memoryAddress);
    }

    TEST(MemorySpace, CheckMemoryEmpty08) {
        memoryAddress = 0x00;
        EXPECT_EQ(false, classTest.endAddress(&memoryAddress));
        EXPECT_EQ(0, memoryAddress);
    }

    TEST(MemorySpace, CheckMemoryEmpty09) {
        memoryData = 0x00;
        EXPECT_EQ(false, classTest.getData(&memoryData));
        EXPECT_EQ(0, memoryData);
    }
    TEST(MemorySpace, CheckMemoryEmpty10) {
        memoryAddress = 0x00;
        memoryData = 0x00;
        EXPECT_EQ(false, classTest.getData(&memoryData, memoryAddress));
        EXPECT_EQ(0, memoryData);
        EXPECT_EQ(0, memoryAddress);    
    }
} // namespace - segment address

/*****************************************************************************/
// Main function for testing
/*****************************************************************************/
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

//    std::cout << "Testing" << std::endl;
    
    return RUN_ALL_TESTS();
}

