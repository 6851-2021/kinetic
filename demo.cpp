//#include <bits/stdc++.h>
#include "heap.h"
//#include <algorithm>

using namespace std;

int main() {

	vector<MovingObject<int>> heapItems;
	heapItems.push_back(MovingObject<int>(0, 0, 0));
	heapItems.push_back(MovingObject<int>(1, 2, 1));
	heapItems.push_back(MovingObject<int>(5, -1, 2));
	KineticHeap<int> heap(heapItems);

	for (int i = 0; i < 8; i++) {
		heap.fastforward(1);
		cout << heap.min().value().value << endl;
	}

	int time = 0;
	vector<MovingObject<int>> v;

	v.push_back(MovingObject<int>(0, 0, &time, 0));

	v.push_back(MovingObject<int>(1, 2, &time, 1));

	v.push_back(MovingObject<int>(5, -1, &time, 2));

	KineticSuccessor<int> succ(v, &time);
	for (int i = 0; i < 8; i++) {
		succ.fastforward(1);
		for (auto j : succ.items) {
			cout << j.value << ' ';
		}
		cout << endl;
	}
	
}
