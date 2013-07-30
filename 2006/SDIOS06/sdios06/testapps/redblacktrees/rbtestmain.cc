#include <stdlib.h>
#include <stdio.h>

#include "redblacktree.h"

using namespace std::private_stuff;
typedef RedBlackTree<int, int> TreeType;

int main()
{
	TreeType tree;

	srand(12354);
	for(int i = 0; i < 2000; ++i) {
		int val = rand() % 1000;
		printf("Inserting %d (step %d)\n", val, i);
		tree.insert(val, val);
	}
	for(int i = 0; i < 100; ++i) {
		tree.insert(i, i);
	}

	printf("Vals: ");
	for(TreeType::iterator i = tree.begin(); i != tree.end(); ++i) {
		printf("%d ", *i);
	}
}
