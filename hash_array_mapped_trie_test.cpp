#include <queue>
#include <vector>

#include <gtest/gtest.h>

#define GTEST
#define HAMT_IMPLEMENTATION
#include "hash_array_mapped_trie.h"

using foc::HashArrayMappedTrie;

using HAMT = HashArrayMappedTrie<int64_t, int64_t>;

class HashArrayMappedTrieTest : public testing::Test {
 protected:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

TEST_F(HashArrayMappedTrieTest, LogicalToPhysicalIndexTranslationTest) {
  HAMT::BitmapTrie trie;

  // All items will be stored at index=0 if the bitmap is empty
  trie._bitmap = 0; // 0000
  for (int i = 0; i < 32; i++) {
    EXPECT_EQ(trie.physicalIndex(i), 0);
  }
  // For any bitmap, the logical index 0 will map to physical index 0.
  // (we test all bitmaps that contain a single bit)
  for (int i = 0; i < 32; i++) {
    trie._bitmap = 0x1 << i;
    EXPECT_EQ(trie.physicalIndex(0), 0);
  }

  trie._bitmap = 1; // 0001
  EXPECT_EQ(trie.physicalIndex(1), 1);
  EXPECT_EQ(trie.physicalIndex(2), 1);
  EXPECT_EQ(trie.physicalIndex(3), 1);
  EXPECT_EQ(trie.physicalIndex(4), 1);
  EXPECT_EQ(trie.physicalIndex(5), 1);
  EXPECT_EQ(trie.physicalIndex(31), 1);
  trie._bitmap = 2; // 0010
  EXPECT_EQ(trie.physicalIndex(1), 0);
  EXPECT_EQ(trie.physicalIndex(2), 1);
  EXPECT_EQ(trie.physicalIndex(3), 1);
  EXPECT_EQ(trie.physicalIndex(4), 1);
  EXPECT_EQ(trie.physicalIndex(5), 1);
  EXPECT_EQ(trie.physicalIndex(31), 1);
  trie._bitmap = 3; // 0011
  EXPECT_EQ(trie.physicalIndex(1), 1);
  EXPECT_EQ(trie.physicalIndex(2), 2);
  EXPECT_EQ(trie.physicalIndex(3), 2);
  EXPECT_EQ(trie.physicalIndex(4), 2);
  EXPECT_EQ(trie.physicalIndex(5), 2);
  EXPECT_EQ(trie.physicalIndex(31), 2);
  trie._bitmap = 4; // 0100
  EXPECT_EQ(trie.physicalIndex(1), 0);
  EXPECT_EQ(trie.physicalIndex(2), 0);
  EXPECT_EQ(trie.physicalIndex(3), 1);
  EXPECT_EQ(trie.physicalIndex(4), 1);
  EXPECT_EQ(trie.physicalIndex(5), 1);
  EXPECT_EQ(trie.physicalIndex(31), 1);
  trie._bitmap = 5; // 0101
  EXPECT_EQ(trie.physicalIndex(1), 1);
  EXPECT_EQ(trie.physicalIndex(2), 1);
  EXPECT_EQ(trie.physicalIndex(3), 2);
  EXPECT_EQ(trie.physicalIndex(4), 2);
  EXPECT_EQ(trie.physicalIndex(5), 2);
  EXPECT_EQ(trie.physicalIndex(31), 2);
  trie._bitmap = 6; // 0110
  EXPECT_EQ(trie.physicalIndex(1), 0);
  EXPECT_EQ(trie.physicalIndex(2), 1);
  EXPECT_EQ(trie.physicalIndex(3), 2);
  EXPECT_EQ(trie.physicalIndex(4), 2);
  EXPECT_EQ(trie.physicalIndex(5), 2);
  EXPECT_EQ(trie.physicalIndex(31), 2);
  trie._bitmap = 7; // 0111
  EXPECT_EQ(trie.physicalIndex(1), 1);
  EXPECT_EQ(trie.physicalIndex(2), 2);
  EXPECT_EQ(trie.physicalIndex(3), 3);
  EXPECT_EQ(trie.physicalIndex(4), 3);
  EXPECT_EQ(trie.physicalIndex(5), 3);
  EXPECT_EQ(trie.physicalIndex(31), 3);
}

TEST_F(HashArrayMappedTrieTest, BitmapIndexedNodeInsertionTest) {
  HAMT hamt;
  HAMT::BitmapTrie trie;
  trie.allocate(hamt._allocator, 1);

  auto e = std::make_pair(40LL, 4LL);
  trie.insert(hamt._allocator, 4, e, nullptr, 2, 0);
  EXPECT_EQ(trie._bitmap, 16); // 010000
  EXPECT_EQ(trie.size(), 1);
  EXPECT_EQ(trie.physicalGet(0).asEntry().first, 40);
  EXPECT_EQ(trie.physicalGet(0).asEntry().second, 4);

  e = std::make_pair(20L, 2L);
  trie.insert(hamt._allocator, 2, e, nullptr, 2, 0);
  EXPECT_EQ(trie._bitmap, 20); // 010100
  EXPECT_EQ(trie.size(), 2);
  EXPECT_EQ(trie.physicalGet(0).asEntry().first, 20);
  EXPECT_EQ(trie.physicalGet(0).asEntry().second, 2);
  EXPECT_EQ(trie.physicalGet(1).asEntry().first, 40);
  EXPECT_EQ(trie.physicalGet(1).asEntry().second, 4);

  e = std::make_pair(30L, 3L);
  trie.insert(hamt._allocator, 3, e, nullptr, 2, 0);
  EXPECT_EQ(trie._bitmap, 28); // 011100
  EXPECT_EQ(trie.size(), 3);
  EXPECT_EQ(trie.physicalGet(0).asEntry().first, 20);
  EXPECT_EQ(trie.physicalGet(0).asEntry().second, 2);
  EXPECT_EQ(trie.physicalGet(1).asEntry().first, 30);
  EXPECT_EQ(trie.physicalGet(1).asEntry().second, 3);
  EXPECT_EQ(trie.physicalGet(2).asEntry().first, 40);
  EXPECT_EQ(trie.physicalGet(2).asEntry().second, 4);

  e = std::make_pair(0LL, 0LL);
  trie.insert(hamt._allocator, 0, e, nullptr, 2, 0);
  EXPECT_EQ(trie._bitmap, 29); // 011101
  EXPECT_EQ(trie.size(), 4);
  EXPECT_EQ(trie.physicalGet(0).asEntry().first, 0);
  EXPECT_EQ(trie.physicalGet(0).asEntry().second, 0);
  EXPECT_EQ(trie.physicalGet(1).asEntry().first, 20);
  EXPECT_EQ(trie.physicalGet(1).asEntry().second, 2);
  EXPECT_EQ(trie.physicalGet(2).asEntry().first, 30);
  EXPECT_EQ(trie.physicalGet(2).asEntry().second, 3);
  EXPECT_EQ(trie.physicalGet(3).asEntry().first, 40);
  EXPECT_EQ(trie.physicalGet(3).asEntry().second, 4);

  e = std::make_pair(50LL, 5LL);
  trie.insert(hamt._allocator, 5, e, nullptr, 2, 0);
  EXPECT_EQ(trie._bitmap, 61); // 111101
  EXPECT_EQ(trie.size(), 5);
  EXPECT_EQ(trie.physicalGet(0).asEntry().first, 0);
  EXPECT_EQ(trie.physicalGet(0).asEntry().second, 0);
  EXPECT_EQ(trie.physicalGet(1).asEntry().first, 20);
  EXPECT_EQ(trie.physicalGet(1).asEntry().second, 2);
  EXPECT_EQ(trie.physicalGet(2).asEntry().first, 30);
  EXPECT_EQ(trie.physicalGet(2).asEntry().second, 3);
  EXPECT_EQ(trie.physicalGet(3).asEntry().first, 40);
  EXPECT_EQ(trie.physicalGet(3).asEntry().second, 4);
  EXPECT_EQ(trie.physicalGet(4).asEntry().first, 50);
  EXPECT_EQ(trie.physicalGet(4).asEntry().second, 5);

  e = std::make_pair(10LL, 1LL);
  trie.insert(hamt._allocator, 1, e, nullptr, 2, 0);
  EXPECT_EQ(trie._bitmap, 63); // 111111
  EXPECT_EQ(trie.size(), 6);
  EXPECT_EQ(trie.physicalGet(0).asEntry().first, 0);
  EXPECT_EQ(trie.physicalGet(0).asEntry().second, 0);
  EXPECT_EQ(trie.physicalGet(1).asEntry().first, 10);
  EXPECT_EQ(trie.physicalGet(1).asEntry().second, 1);
  EXPECT_EQ(trie.physicalGet(2).asEntry().first, 20);
  EXPECT_EQ(trie.physicalGet(2).asEntry().second, 2);
  EXPECT_EQ(trie.physicalGet(3).asEntry().first, 30);
  EXPECT_EQ(trie.physicalGet(3).asEntry().second, 3);
  EXPECT_EQ(trie.physicalGet(4).asEntry().first, 40);
  EXPECT_EQ(trie.physicalGet(4).asEntry().second, 4);
  EXPECT_EQ(trie.physicalGet(5).asEntry().first, 50);
  EXPECT_EQ(trie.physicalGet(5).asEntry().second, 5);

  e = std::make_pair(310LL, 31L);
  trie.insert(hamt._allocator, 31, e, nullptr, 2, 0);
  EXPECT_EQ(trie._bitmap, 63 | (0x1 << 31));
  EXPECT_EQ(trie.size(), 7);
  EXPECT_EQ(trie.physicalGet(6).asEntry().first, 310);
  EXPECT_EQ(trie.physicalGet(6).asEntry().second, 31);
}

void print_bitmap_indexed_node(
    HAMT::BitmapTrie &trie,
    std::string indent) {
  std::vector<HAMT::BitmapTrie *> tries;

  printf("%3d/%-3d: %s", trie.size(), trie.capacity(), indent.c_str());
  for (int i = 0; i < 32; i++) {
    if (trie.logicalPositionTaken(i)) {
      auto& node = trie.logicalGet(i);
      if (node.isEntry()) {
        printf("%3lld ", node.asEntry().second);
      } else {
        printf("[ ] ");
        tries.push_back(&node.asTrie());
      }
    } else {
      printf("--- ");
    }
  }
  putchar('\n');

  for (auto trie : tries) {
    print_bitmap_indexed_node(*trie, indent + "    ");
  }
}

void print_hamt(HAMT &hamt) {
  print_bitmap_indexed_node(hamt.root(), "");
  putchar('\n');
}

void print_stats(HAMT &hamt) {
  std::queue<HAMT::BitmapTrie *> q;

  int stats[33];
  memset(stats, 0, sizeof(stats));

  q.push(&hamt.root());
  while (!q.empty()) {
    auto trie = q.front();
    q.pop();

    stats[trie->size()]++;

    for (int i = 0; i < 32; i++) {
      if (trie->logicalPositionTaken(i)) {
        auto& node = trie->logicalGet(i);
        if (!node.isEntry()) {
          q.push(&node.asTrie());
        }
      }
    }
  }

  int total = 0;
  for (int i = 1; i <= 32; i++) {
    printf("%6d ", stats[i]);
    total += stats[i];
  }
  putchar('\n');
  for (int i = 1; i <= 32; i++) {
    printf("%6.3lf ", (double)stats[i] / total);
  }
  putchar('\n');
  for (int i = 1; i <= 32; i++) {
    double a = (double)stats[i] / total;
    printf("%6d ", (int)(a * 100));
  }
  putchar('\n');
}

TEST_F(HashArrayMappedTrieTest, TopLevelInsertionTest) {
  /* for (int max = 1; max <= 1048576; max *= 2) { */
  for (int64_t max = 65536; max <= 65536; max *= 2) {
    HAMT hamt;
    for (int64_t i = 1; i <= max; i++) {
      /* printf("%d:\n", i * 10); */
      hamt.insert(i * 10, i);
      if (__builtin_popcount(i) == 1) {
        hamt.print();
      }
    }
    /* print_hamt(hamt); */
    /* print_stats(hamt); */

    /* int64_t count_not_found = 0; */
    int64_t last_not_found = -1;
    for (int64_t i = 1; i <= max; i++) {
      auto found = hamt.find(i * 10);
      EXPECT_TRUE(found != nullptr);
      /*
      if (found == nullptr) {
        count_not_found++;
        last_not_found = i * 10;
        continue;
      }
      */
      EXPECT_EQ(*found, i);
    }
    /* printf("count_not_found %d\n", count_not_found); */
    EXPECT_EQ(last_not_found, -1);
  }
}
