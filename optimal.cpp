#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <pthread.h>

using namespace std;

#define NUM_FRAMES 4

vector<int> pageFrames(NUM_FRAMES, -1); // Frames initialized to -1 indicating empty
vector<int> pageReference;
int pageFaults = 0;
int pageHits = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to check if page is present in frames
bool isInFrames(int page) {
    return find(pageFrames.begin(), pageFrames.end(), page) != pageFrames.end();
}

// Function to find the page that will not be used for the longest time in the future
int findOptimalPage(int currentIndex) {
    unordered_map<int, int> nextPageIndex;
    for (int i = 0; i < NUM_FRAMES; ++i) {
        int nextPage = -1;
        for (int j = currentIndex + 1; j < pageReference.size(); ++j) {
            if (pageReference[j] == pageFrames[i]) {
                nextPage = j;
                break;
            }
        }
        if (nextPage == -1)
            return i;
        nextPageIndex[i] = nextPage;
    }

    int optimalPage = 0;
    for (int i = 1; i < NUM_FRAMES; ++i) {
        if (nextPageIndex[i] > nextPageIndex[optimalPage])
            optimalPage = i;
    }
    return optimalPage;
}

// Function to simulate Optimal page replacement algorithm
void* simulateOptimal(void* arg) {
    int pageNumber = *((int*)arg);
    pthread_mutex_lock(&mutex);
    if (isInFrames(pageNumber)) {
        pageHits++;
    } else {
        pageFaults++;
        if (pageFrames.size() < NUM_FRAMES) {
            // If there's space in frames, simply add the page
            pageFrames.push_back(pageNumber);
        } else {
            // Find the page that will not be used for the longest time in the future
            int indexToReplace = findOptimalPage(*((int*)arg));
            pageFrames[indexToReplace] = pageNumber;
        }s
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    int numPages;
    cout<<"\n...This is OPTIMAL page replacement Algorithm...\n";
    cout << "Enter the number of pages: ";
    cin >> numPages;

    cout << "Enter the sequence of page numbers: ";
    for (int i = 0; i < numPages; ++i) {
        int page;
        cin >> page;
        pageReference.push_back(page);
    }

    pthread_t threads[numPages];

    // Simulate Optimal page replacement algorithm using pthreads
    for (int i = 0; i < numPages; ++i) {
        pthread_create(&threads[i], NULL, simulateOptimal, (void*)&pageReference[i]);
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
