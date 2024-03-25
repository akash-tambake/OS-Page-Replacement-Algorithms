#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

#define FRAME_SIZE 4

vector<int> pageSequence;
vector<int> frame(FRAME_SIZE, 0);
vector<int> pageLastUsed(FRAME_SIZE, 0); // Keeps track of the last used time of each page in the frame
int page_index = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int pageFaults = 0;
int pageHits = 0;

void* accessPage(void* arg) {
    while (true) {
        int page;
        pthread_mutex_lock(&mutex);
        if (page_index >= pageSequence.size()) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        page = pageSequence[page_index++];
        pthread_mutex_unlock(&mutex);

        bool page_fault = true;
        for (int i = 0; i < FRAME_SIZE; ++i) {
            if (frame[i] == page) {
                page_fault = false;
                pageHits++;
                pageLastUsed[i] = page_index; // Update last used time of the page
                break;
            }
        }

        if (page_fault) {
            // Page replacement needed
            pageFaults++;
            int lru_page_index = min_element(pageLastUsed.begin(), pageLastUsed.end()) - pageLastUsed.begin();
            cout << "Page " << page << " caused a page fault. Page ";
            cout << frame[lru_page_index] << " is replaced.\n";
            frame[lru_page_index] = page;
            pageLastUsed[lru_page_index] = page_index; // Update last used time of the replaced page
        }

        cout << "Page " << page << " is in the memory: ";
        for (int i = 0; i < FRAME_SIZE; ++i) {
            cout << frame[i] << " ";
        }
        cout << endl;
        usleep(1000000); // Sleep for 1 second (for better visualization)
    }
    return NULL;
}

int main() {
    int n;
    cout<<"\n...This is LRU simulation...\n";
    cout << "Enter the number of pages: ";
    cin >> n;

    cout << "Enter the sequence of page accesses:\n";
    for (int i = 0; i < n; ++i) {
        int page;
        cin >> page;
        pageSequence.push_back(page);
    }

    pthread_t threads[n];
    for (int i = 0; i < n; ++i) {
        pthread_create(&threads[i], NULL, accessPage, NULL);
    }

    for (int i = 0; i < n; ++i) {
        pthread_join(threads[i], NULL);
    }

    cout << "Number of Page Faults: " << pageFaults << endl;
    cout << "Number of Page Hits: " << pageHits << endl;

    pthread_mutex_destroy(&mutex);

    return 0;
}
