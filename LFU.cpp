#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <pthread.h>

using namespace std;

#define NUM_FRAMES 4

vector<int> pageFrames(NUM_FRAMES, 0);
vector<int> pageReference;
unordered_map<int, int> pageFrequency; // Map to store frequency of each page
int pageFaults = 0;
int pageHits = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to check if page is present in frames
bool isInFrames(int page) {
    return find(pageFrames.begin(), pageFrames.end(), page) != pageFrames.end();
}

// Function to get index of least frequently used page
int getLFUIndex() {
    int minIndex = 0;
    for (int i = 1; i < NUM_FRAMES; ++i) {
        if (pageFrequency[pageFrames[i]] < pageFrequency[pageFrames[minIndex]]) {
            minIndex = i;
        }
    }
    return minIndex;
}

// Function to simulate LFU page replacement algorithm
void* simulateLFU(void* arg) {
    int pageNumber = *((int*)arg);
    pthread_mutex_lock(&mutex);
    if (isInFrames(pageNumber)) {
        pageHits++;
        // Increment frequency of the page
        pageFrequency[pageNumber]++;
    } else {
        pageFaults++;
        if (pageFrames.size() < NUM_FRAMES) {
            // If there's space in frames, simply add the page
            pageFrames.push_back(pageNumber);
        } else {
            // Find the least frequently used page
            int lfuIndex = getLFUIndex();
            // Replace the least frequently used page with the new page
            pageFrequency.erase(pageFrames[lfuIndex]);
            pageFrames[lfuIndex] = pageNumber;
        }
        pageFrequency[pageNumber] = 1; // Initialize frequency of the new page
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    int numPages;
    cout<<"\n...This is LFU simulation...\n";
    cout << "Enter the number of pages: ";
    cin >> numPages;

    cout << "Enter the sequence of page numbers: ";
    for (int i = 0; i < numPages; ++i) {
        int page;
        cin >> page;
        pageReference.push_back(page);
    }

    pthread_t threads[numPages];

    // Simulate LFU page replacement algorithm using pthreads
    for (int i = 0; i < numPages; ++i) {
        pthread_create(&threads[i], NULL, simulateLFU, (void*)&pageReference[i]);
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
