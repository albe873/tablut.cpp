#include <gtest/gtest.h>
#include <adversarialSearch/t_table.h>

class TTableTest : public ::testing::Test {
protected:
    int tableSize = 1000;
    int unknownValue = -1000; // Example unknown value
    t_table<int, int> tt;   // Assuming int for score and action

    TTableTest() : tt(tableSize, unknownValue) {}
};

TEST_F(TTableTest, InsertAndProbe) {
    int64_t hash = 12345;
    entry_type type = entry_type::exact;
    int depth = 5;
    int score = 10;

    tt.insert(hash, type, score, depth);

    // Probe for the value we just inserted
    int probedScore = tt.probe(hash, -100, 100, depth);
    ASSERT_EQ(probedScore, score);

    // Probe with a different depth - should still return the score
    int probedScoreDifferentDepth = tt.probe(hash, -100, 100, depth + 1);
    ASSERT_EQ(probedScoreDifferentDepth, unknownValue); // Because depth is not sufficient

    // Probe with wrong hash
    int wrongHash = tt.probe(67890, -100, 100, depth);
    ASSERT_EQ(wrongHash, unknownValue);
}

TEST_F(TTableTest, ClearTable) {
    int64_t hash = 54321;
    tt.insert(hash, entry_type::exact, 20, 3);
    tt.clear();

    // After clearing, probing should return the unknown value
    int probedScore = tt.probe(hash, -100, 100, 3);
    ASSERT_EQ(probedScore, unknownValue);
}

TEST_F(TTableTest, LowerBoundProbe) {
    int64_t hash = 98765;
    int depth = 4;
    int score = 50;

    tt.insert(hash, entry_type::l_bound, score, depth);

    // beta < score, should return unknownValue
    int probedScore1 = tt.probe(hash, 40, 60, depth);  // score < beta
    ASSERT_EQ(probedScore1, unknownValue);

    // beta < score, should return unknownValue
    int probedScore2 = tt.probe(hash, 60, 70, depth);  // score < beta
    ASSERT_EQ(probedScore2, unknownValue);

    // beta > score, should return score
    int probedScore3 = tt.probe(hash, 30, 40, depth);  // score > score
     ASSERT_EQ(probedScore3, 50);
}

TEST_F(TTableTest, UpperBoundProbe) {
    int64_t hash = 45678;
    int depth = 2;
    int score = 30;

    tt.insert(hash, entry_type::u_bound, score, depth);

    // alpha < score, should return unknownValue
    int probedScore1 = tt.probe(hash, 20, 40, depth);  // score > alpha
    ASSERT_EQ(probedScore1, unknownValue);

    // alpha < score, should return unknownValue
    int probedScore2 = tt.probe(hash, 10, 20, depth);  // score > alpha
    ASSERT_EQ(probedScore2, unknownValue);

    // alpha > score, should return score
    int probedScore3 = tt.probe(hash, 40, 50, depth);  // score < alpha
    ASSERT_EQ(probedScore3, 30);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
