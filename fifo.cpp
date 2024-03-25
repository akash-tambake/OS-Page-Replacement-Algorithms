#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define FRAME_SIZE 4

vector<int> pageSequence;
vector<int> frame(FRAME_SIZE, 0);
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
                break;
            }
        }

        if (page_fault) {
            // Page replacement needed
            pageFaults++;
            cout << "Page " << page << " caused a page fault. Page ";
            cout << frame[0] << " is replaced.\n";
            // FIFO page replacement
            for (int i = 0; i < FRAME_SIZE - 1; ++i) {
                frame[i] = frame[i + 1];
            }
            frame[FRAME_SIZE - 1] = page;
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
    cout << "...This is FIFO simulation...\n";
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
