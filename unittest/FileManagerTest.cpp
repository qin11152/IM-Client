#include "module/FileManager/FileManager.h"
#include <gtest/gtest.h>

class FileManagerTest : public testing::Test
{
protected:
    void SetUp() override {
        fileManager = &Base::fileoperate::FileManager::get_mutable_instance();
    }

    void TearDown() override {
    }

    Base::fileoperate::FileManager * fileManager;
};

TEST_F(FileManagerTest, AddTest) 
{
    EXPECT_EQ(fileManager->checkDirExist("G:/"), true);
}
