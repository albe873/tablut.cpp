#include <gtest/gtest.h>

#include <adversarialSearch/utilities.h>
#include <tablut/common.h>


TEST(utilTest, ordering) {
    std::multiset<actionUtility<Move, uint8_t>> results;

    results.insert({Move(cord(0, 0), cord(1, 1)), 5});
    results.insert({Move(cord(0, 1), cord(1, 1)), 3});
    results.insert({Move(cord(0, 2), cord(1, 1)), 4});
    results.insert({Move(cord(0, 3), cord(1, 1)), 6});
    results.insert({Move(cord(0, 4), cord(1, 1)), 3});

    for (auto it = results.begin(); it != results.end(); it++) {
        std::cout << "Action: " << std::to_string(it->action.getFrom().y) << ", Utility: " << (int)it->utility << std::endl;
    }
    ASSERT_EQ(results.size(), 5);
    auto it = results.begin();
    ASSERT_EQ(it->action.getFrom().y, 3);
    ASSERT_EQ(it->utility, 6);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}