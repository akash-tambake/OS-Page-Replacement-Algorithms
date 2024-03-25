#include <iostream>
#include <vector>
#include <algorithm>
#include <pthread.h>

using namespace std;

#define NUM_FRAMES 4

vector<int> pageFrames(NUM_FRAMES, 0);
vector<int> pageReference;
int pageFaults = 0;
int pageHits = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to check if page is present in frames
bool isInFrames(int page) {
    return find(pageFrames.begin(), pageFrames.end(), page) != pageFrames.end();
}

// Function to simulate MRU page replacement algorithm
void* simulateMRU(void* arg) {
    int pageNumber = *((int*)arg);
    pthread_mutex_lock(&mutex);
    if (isInFrames(pageNumber)) {
        pageHits++;
        // Move the accessed page to the front of the frames (most recently used)
        pageFrames.erase(remove(pageFrames.begin(), pageFrames.end(), pageNumber), pageFrames.end());
        pageFrames.insert(pageFrames.begin(), pageNumber);
    } else {
        pageFaults++;
        if (pageFrames.size() < NUM_FRAMES) {
            // If there's space in frames, simply add the page to the front
            pageFrames.insert(pageFrames.begin(), pageNumber);
        } else {
            // Replace the most recently used page with the new page
            pageFrames.erase(pageFrames.begin());
            pageFrames.insert(pageFrames.begin(), pageNumber);
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    int numPages;
    cout<<"\n..This is MRU simulation...\n";
    cout << "Enter the number of pages: ";
    cin >> numPages;

    cout << "Enter the sequence of page numbers: ";
    for (int i = 0; i < numPages; ++i) {
        int page;
        cin >> page;
        pageReference.push_back(page);
    }

    pthread_t threads[numPages];

    // Simulate MRU page replacement algorithm using pthreads
    for (int i = 0; i < numPages; ++i) {
        pthread_create(&threads[i], NULL, simulateMRU, (void*)&pageReference[i]);
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
