#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <thread>
#include <vector>

#define NUM_GUESTS 5

using namespace std;

vector<bool> guestsEaten(NUM_GUESTS, false); // keeps track of which guests already ate a cupcake
mutex threadsMutex;
bool isCupcakeAvailable = true;
int currentGuest = 0; // indicates which guest is currently in the labyrinth
int currentTotal = 0; // keeps track of total number of guests who have already eaten a cupcake

void countGuests() {
    while (currentTotal < NUM_GUESTS) {
        lock_guard<mutex> lock(threadsMutex);

        // first guest is picked to enter the labyrinth
        if (currentGuest == 0) {
            // someone else already ate the cupcake
            if (!isCupcakeAvailable) {
                currentTotal++;
                isCupcakeAvailable = true;
            }

            // first guest can eat cupcake
            if (isCupcakeAvailable && !guestsEaten[0]) {
                currentTotal++;
                guestsEaten[0] = true;
                isCupcakeAvailable = true;
                cout << "Guest 0 ate cupcake" << endl;
            }
        }
    }
    this_thread::sleep_for(chrono::milliseconds(100));
}

void labyrinth(int index) {
    while (currentTotal < NUM_GUESTS) {
        lock_guard<mutex> lock(threadsMutex);

        if (!guestsEaten[index] && index == currentGuest && isCupcakeAvailable) {
            isCupcakeAvailable = false; // eat cupcake
            guestsEaten[index] = true;
            currentTotal++;
            cout << "Guest " << index << " ate cupcake" << endl;
            isCupcakeAvailable = true; // replace cupcake
        }
    }
    this_thread::sleep_for(chrono::milliseconds(100));
}

int main() {
    vector<thread> threads;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distribution(0, NUM_GUESTS - 1);

    auto startTime = chrono::high_resolution_clock::now();
    threads.emplace_back(countGuests); // first thread/guest keeps track of other guests

    for (int i = 1; i < NUM_GUESTS; i++) {
        threads.emplace_back(labyrinth, i);
    }

    while (currentTotal < NUM_GUESTS) {
        lock_guard<mutex> lock(threadsMutex);
        currentGuest = distribution(gen);
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    for (auto &thread : threads) {
        thread.join();
    }

    auto endTime = chrono::high_resolution_clock::now();
    auto executionTime = chrono::duration_cast<chrono::seconds>(endTime - startTime);
    cout << "All guests have entered the labyrinth" << endl;
    cout << "Time elapsed: " << executionTime.count() << " seconds" << endl;

    return 0;
}