#include <gtest/gtest.h>
#include <utils/file_info.h>

using talko::utils::FileInfo;

TEST(FileInfo, IsAbsoulte) {
    EXPECT_TRUE(FileInfo("/home/leaos/Downloads/googletest/build/cmake_install.cmake").isAbsoulte());
    EXPECT_FALSE(FileInfo("../build/").isAbsoulte());
    EXPECT_FALSE(FileInfo("../build/.cache").isAbsoulte());
    EXPECT_FALSE(FileInfo("../build/.cache/").isAbsoulte());
    EXPECT_FALSE(FileInfo("../.gitignore").isAbsoulte());
    EXPECT_FALSE(FileInfo("../CMakeList.txt").isAbsoulte());
}

TEST(FileInfo, IsRelative) {
    EXPECT_FALSE(FileInfo("/home/leaos/Downloads/googletest/build/cmake_install.cmake").isRelative());
    EXPECT_TRUE(FileInfo("../build/").isRelative());
    EXPECT_TRUE(FileInfo("../build/.cache").isRelative());
    EXPECT_TRUE(FileInfo("../build/.cache/").isRelative());
    EXPECT_TRUE(FileInfo("../.gitignore").isRelative());
    EXPECT_TRUE(FileInfo("../CMakeList.txt").isRelative());
}

TEST(FileInfo, IsDirectory) {
    EXPECT_FALSE(FileInfo("/home/leaos/Downloads/googletest/build/cmake_install.cmake").isDirectory());
    EXPECT_TRUE(FileInfo("/usr/bin").isDirectory());
    EXPECT_TRUE(FileInfo("/usr/bin/").isDirectory());
    EXPECT_TRUE(FileInfo("/home/leaos/.cache").isDirectory());
    EXPECT_TRUE(FileInfo("/home/leaos/.cache/").isDirectory());
    EXPECT_FALSE(FileInfo("/home/leaos/.zshrc").isDirectory());
    EXPECT_FALSE(FileInfo("../.clang-format").isDirectory());
    EXPECT_TRUE(FileInfo("../lib").isDirectory());
    EXPECT_TRUE(FileInfo("../lib/").isDirectory());
}

TEST(FileInfo, IsFile) {
    EXPECT_TRUE(FileInfo("/home/leaos/Downloads/googletest/build/cmake_install.cmake").isFile());
    EXPECT_FALSE(FileInfo("/usr/bin").isFile());
    EXPECT_FALSE(FileInfo("/usr/bin/").isFile());
    EXPECT_FALSE(FileInfo("/home/leaos/.cache").isFile());
    EXPECT_FALSE(FileInfo("/home/leaos/.cache/").isFile());
    EXPECT_TRUE(FileInfo("/home/leaos/.zshrc").isFile());
    EXPECT_TRUE(FileInfo("../.clang-format").isFile());
    EXPECT_FALSE(FileInfo("../lib").isFile());
    EXPECT_FALSE(FileInfo("../lib/").isFile());
}

TEST(FileInfo, IsHidden) {
    EXPECT_FALSE(FileInfo("/home/leaos/Downloads/googletest/build/cmake_install.cmake").isHidden());
    EXPECT_FALSE(FileInfo("/usr/bin").isHidden());
    EXPECT_FALSE(FileInfo("/usr/bin/").isHidden());
    EXPECT_TRUE(FileInfo("/home/leaos/.cache").isHidden());
    EXPECT_TRUE(FileInfo("/home/leaos/.cache/").isHidden());
    EXPECT_TRUE(FileInfo("/home/leaos/.zshrc").isHidden());
    EXPECT_TRUE(FileInfo("../.clang-format").isHidden());
    EXPECT_FALSE(FileInfo("../lib").isHidden());
    EXPECT_FALSE(FileInfo("../lib/").isHidden());
}

TEST(FileInfo, Exists) {
    EXPECT_TRUE(FileInfo("/home/leaos/Downloads/googletest/build/cmake_install.cmake").exists());
    EXPECT_FALSE(FileInfo("/home/leaos/Downloads/googletest/build1/cmake_install.cmake").exists());
    EXPECT_TRUE(FileInfo("/usr/bin").exists());
    EXPECT_FALSE(FileInfo("/usr1/bin").exists());
    EXPECT_TRUE(FileInfo("/usr/bin/").exists());
    EXPECT_FALSE(FileInfo("/usr1/bin/").exists());
    EXPECT_TRUE(FileInfo("/home/leaos/.cache").exists());
    EXPECT_FALSE(FileInfo("/home2/leaos/.cache").exists());
    EXPECT_TRUE(FileInfo("/home/leaos/.cache/").exists());
    EXPECT_FALSE(FileInfo("/home/leaos2/.cache/").exists());
    EXPECT_TRUE(FileInfo("/home/leaos/.zshrc").exists());
    EXPECT_FALSE(FileInfo("/home2/leaos/.zshrc").exists());
    EXPECT_TRUE(FileInfo("../.clang-format").exists());
    EXPECT_FALSE(FileInfo("../.clang2-format").exists());
    EXPECT_TRUE(FileInfo("../lib").exists());
    EXPECT_FALSE(FileInfo("../lib2").exists());
    EXPECT_TRUE(FileInfo("../lib/").exists());
    EXPECT_FALSE(FileInfo("../lib2/").exists());
}

TEST(FileInfo, Suffix) {
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir").suffix().c_str(), "");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/").suffix().c_str(), "");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir").suffix().c_str(), "");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir/").suffix().c_str(), "");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.test.tar.gz").suffix().c_str(), "gz");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/test.tar.gz").suffix().c_str(), "gz");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.logtxt").suffix().c_str(), "");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt").suffix().c_str(), "");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt.a").suffix().c_str(), "a");
    ASSERT_STREQ(FileInfo("./.test_dir").suffix().c_str(), "");
    ASSERT_STREQ(FileInfo("./.test_dir/").suffix().c_str(), "");
    ASSERT_STREQ(FileInfo("./.test_dir/dir").suffix().c_str(), "");
    ASSERT_STREQ(FileInfo("./.test_dir/dir/").suffix().c_str(), "");
    ASSERT_STREQ(FileInfo("./.test_dir/.test.tar.gz").suffix().c_str(), "gz");
    ASSERT_STREQ(FileInfo("./.test_dir/test.tar.gz").suffix().c_str(), "gz");
    ASSERT_STREQ(FileInfo("./.test_dir/.logtxt").suffix().c_str(), "");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt").suffix().c_str(), "");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt.a").suffix().c_str(), "a");
}

TEST(FileInfo, CompleteSuffix) {
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir").completeSuffix().c_str(), "");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/").completeSuffix().c_str(), "");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir").completeSuffix().c_str(), "");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir/").completeSuffix().c_str(), "");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.test.tar.gz").completeSuffix().c_str(), "tar.gz");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/test.tar.gz").completeSuffix().c_str(), "tar.gz");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.logtxt").completeSuffix().c_str(), "");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt").completeSuffix().c_str(), "");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt.a").completeSuffix().c_str(), "a");
    ASSERT_STREQ(FileInfo("./.test_dir").completeSuffix().c_str(), "");
    ASSERT_STREQ(FileInfo("./.test_dir/").completeSuffix().c_str(), "");
    ASSERT_STREQ(FileInfo("./.test_dir/dir").completeSuffix().c_str(), "");
    ASSERT_STREQ(FileInfo("./.test_dir/dir/").completeSuffix().c_str(), "");
    ASSERT_STREQ(FileInfo("./.test_dir/.test.tar.gz").completeSuffix().c_str(), "tar.gz");
    ASSERT_STREQ(FileInfo("./.test_dir/test.tar.gz").completeSuffix().c_str(), "tar.gz");
    ASSERT_STREQ(FileInfo("./.test_dir/.logtxt").completeSuffix().c_str(), "");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt").completeSuffix().c_str(), "");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt.a").completeSuffix().c_str(), "a");
}

TEST(FileInfo, FileName) {
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir").fileName().c_str(), ".test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/").fileName().c_str(), ".test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir").fileName().c_str(), "dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir/").fileName().c_str(), "dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.test.tar.gz").fileName().c_str(), ".test.tar.gz");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/test.tar.gz").fileName().c_str(), "test.tar.gz");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.logtxt").fileName().c_str(), ".logtxt");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt").fileName().c_str(), "logtxt");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt.a").fileName().c_str(), "logtxt.a");
    ASSERT_STREQ(FileInfo("./.test_dir").fileName().c_str(), ".test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/").fileName().c_str(), ".test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/dir").fileName().c_str(), "dir");
    ASSERT_STREQ(FileInfo("./.test_dir/dir/").fileName().c_str(), "dir");
    ASSERT_STREQ(FileInfo("./.test_dir/.test.tar.gz").fileName().c_str(), ".test.tar.gz");
    ASSERT_STREQ(FileInfo("./.test_dir/test.tar.gz").fileName().c_str(), "test.tar.gz");
    ASSERT_STREQ(FileInfo("./.test_dir/.logtxt").fileName().c_str(), ".logtxt");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt").fileName().c_str(), "logtxt");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt.a").fileName().c_str(), "logtxt.a");
}

TEST(FileInfo, BaseName) {
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir").baseName().c_str(), ".test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/").baseName().c_str(), ".test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir").baseName().c_str(), "dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir/").baseName().c_str(), "dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.test.tar.gz").baseName().c_str(), ".test");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/test.tar.gz").baseName().c_str(), "test");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.logtxt").baseName().c_str(), ".logtxt");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt").baseName().c_str(), "logtxt");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt.a").baseName().c_str(), "logtxt");
    ASSERT_STREQ(FileInfo("./.test_dir").baseName().c_str(), ".test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/").baseName().c_str(), ".test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/dir").baseName().c_str(), "dir");
    ASSERT_STREQ(FileInfo("./.test_dir/dir/").baseName().c_str(), "dir");
    ASSERT_STREQ(FileInfo("./.test_dir/.test.tar.gz").baseName().c_str(), ".test");
    ASSERT_STREQ(FileInfo("./.test_dir/test.tar.gz").baseName().c_str(), "test");
    ASSERT_STREQ(FileInfo("./.test_dir/.logtxt").baseName().c_str(), ".logtxt");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt").baseName().c_str(), "logtxt");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt.a").baseName().c_str(), "logtxt");
}

TEST(FileInfo, CompleteBaseName) {
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir").completeBaseName().c_str(), ".test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/").completeBaseName().c_str(), ".test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir").completeBaseName().c_str(), "dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir/").completeBaseName().c_str(), "dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.test.tar.gz").completeBaseName().c_str(), ".test.tar");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/test.tar.gz").completeBaseName().c_str(), "test.tar");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.logtxt").completeBaseName().c_str(), ".logtxt");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt").completeBaseName().c_str(), "logtxt");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt.a").completeBaseName().c_str(), "logtxt");
    ASSERT_STREQ(FileInfo("./.test_dir").completeBaseName().c_str(), ".test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/").completeBaseName().c_str(), ".test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/dir").completeBaseName().c_str(), "dir");
    ASSERT_STREQ(FileInfo("./.test_dir/dir/").completeBaseName().c_str(), "dir");
    ASSERT_STREQ(FileInfo("./.test_dir/.test.tar.gz").completeBaseName().c_str(), ".test.tar");
    ASSERT_STREQ(FileInfo("./.test_dir/test.tar.gz").completeBaseName().c_str(), "test.tar");
    ASSERT_STREQ(FileInfo("./.test_dir/.logtxt").completeBaseName().c_str(), ".logtxt");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt").completeBaseName().c_str(), "logtxt");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt.a").completeBaseName().c_str(), "logtxt");
}

TEST(FileInfo, AbsoultePath) {
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir").absoultePath().c_str(), "/home/leaos/Project/talko/bin");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/").absoultePath().c_str(), "/home/leaos/Project/talko/bin");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir/").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.test.tar.gz").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/test.tar.gz").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.logtxt").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt.a").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir").absoultePath().c_str(), "/home/leaos/Project/talko/bin");
    ASSERT_STREQ(FileInfo("./.test_dir/").absoultePath().c_str(), "/home/leaos/Project/talko/bin");
    ASSERT_STREQ(FileInfo("./.test_dir/dir").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/dir/").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/.test.tar.gz").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/test.tar.gz").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/.logtxt").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt.a").absoultePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
}

TEST(FileInfo, AbsoulteFilePath) {
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir/").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.test.tar.gz").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/.test.tar.gz");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/test.tar.gz").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/test.tar.gz");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.logtxt").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/.logtxt");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/logtxt");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt.a").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/logtxt.a");
    ASSERT_STREQ(FileInfo("./.test_dir").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/dir").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/dir");
    ASSERT_STREQ(FileInfo("./.test_dir/dir/").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/dir");
    ASSERT_STREQ(FileInfo("./.test_dir/.test.tar.gz").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/.test.tar.gz");
    ASSERT_STREQ(FileInfo("./.test_dir/test.tar.gz").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/test.tar.gz");
    ASSERT_STREQ(FileInfo("./.test_dir/.logtxt").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/.logtxt");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/logtxt");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt.a").absoulteFilePath().c_str(), "/home/leaos/Project/talko/bin/.test_dir/logtxt.a");
}

TEST(FileInfo, Path) {
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir").path().c_str(), "/home/leaos/Project/talko/bin");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/").path().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir").path().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/dir/").path().c_str(), "/home/leaos/Project/talko/bin/.test_dir/dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.test.tar.gz").path().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/test.tar.gz").path().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/.logtxt").path().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt").path().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("/home/leaos/Project/talko/bin/.test_dir/logtxt.a").path().c_str(), "/home/leaos/Project/talko/bin/.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir").path().c_str(), ".");
    ASSERT_STREQ(FileInfo("./.test_dir/").path().c_str(), "./.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/dir").path().c_str(), "./.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/dir/").path().c_str(), "./.test_dir/dir");
    ASSERT_STREQ(FileInfo("./.test_dir/.test.tar.gz").path().c_str(), "./.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/test.tar.gz").path().c_str(), "./.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/.logtxt").path().c_str(), "./.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt").path().c_str(), "./.test_dir");
    ASSERT_STREQ(FileInfo("./.test_dir/logtxt.a").path().c_str(), "./.test_dir");
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
};