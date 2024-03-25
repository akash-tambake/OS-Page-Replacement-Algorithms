#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <pthread.h>

using namespace std;

#define NUM_FRAMES 4

vector<int> pageFrames(NUM_FRAMES, 0);
queue<int> fifoQueue; // Queue to maintain the order of page accesses
unordered_set<int> pageSet; // Set to check if page is in frames
int pageFaults = 0;
int pageHits = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to simulate FIFO Second Chance page replacement algorithm
void* simulateFIFOSecondChance(void* arg) {
    int pageNumber = *((int*)arg);
    pthread_mutex_lock(&mutex);
    if (pageSet.find(pageNumber) != pageSet.end()) {
        pageHits++;
    } else {
        pageFaults++;
        if (pageSet.size() < NUM_FRAMES) {
            // If there's space in frames, simply add the page
            pageFrames[pageSet.size()] = pageNumber;
            pageSet.insert(pageNumber);
            fifoQueue.push(pageNumber);
        } else {
            // Remove pages from frames until finding a page with reference bit 0
            while (true) {
                int pageToReplace = fifoQueue.front();
                fifoQueue.pop();
                if (pageFrames[0] == pageToReplace) {
                    pageFrames[0] = pageNumber;
                    fifoQueue.push(pageNumber);
                    break;
                } else {
                    fifoQueue.push(pageFrames[0]);
                    pageFrames.erase(pageFrames.begin());
                    pageFrames.push_back(pageNumber);
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    int numPages;
    cout<<"\n...This Is FIFO SECOND CHANCE simulation...\n";
    cout << "Enter the number of pages: ";
    cin >> numPages;

    cout << "Enter the sequence of page numbers: ";
    vector<int> pageReference(numPages);
    for (int i = 0; i < numPages; ++i) {
        cin >> pageReference[i];
    }

    pthread_t threads[numPages];

    // Simulate FIFO Second Chance page replacement algorithm using pthreads
    for (int i = 0; i < numPages; ++i) {
        pthread_create(&threads[i], NULL, simulateFIFOSecondChance, (void*)&pageReference[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < numPages; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Display final results
    cout << "Page faults: " << pageFaults << endl;
    cout << "Page hits: " << pageHits << endl;

    return 0;
}
