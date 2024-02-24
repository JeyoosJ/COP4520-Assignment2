#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_set>
#include <vector>

#define NUM_GUESTS 5

using namespace std;

unordered_set<int> guestsVisited{}; // keeps track of which guests already visited vase
mutex threadsMutex;
bool roomAvailable = true;

void vase(int index) {
    while (guestsVisited.size() < NUM_GUESTS) {
        lock_guard<mutex> lock(threadsMutex);

        if (roomAvailable && guestsVisited.find(index) == guestsVisited.end()) {
            roomAvailable = false;
            cout << "Guest " << index << " is viewing the vase " << endl;
            this_thread::sleep_for(chrono::milliseconds(200));
            roomAvailable = true;
            guestsVisited.insert(index);
        }
    }
}

int main() {
    vector<thread> threads;

    auto startTime = chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_GUESTS; i++) {
        threads.emplace_back(vase, i);
    }

    for (auto &thread : threads) {
        thread.join();
    }

    auto endTime = chrono::high_resolution_clock::now();
    auto executionTime = chrono::duration_cast<chrono::seconds>(endTime - startTime);
    cout << "All guests have seen the crystal vase." << endl;
    cout << "Time elapsed: " << executionTime.count() << " seconds" << endl;

    return 0;
}