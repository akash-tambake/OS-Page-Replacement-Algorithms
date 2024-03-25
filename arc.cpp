#include <iostream>
#include <vector>
#include <algorithm>
#include <pthread.h>

using namespace std;

#define FRAME_SIZE 4

vector<int> frame(FRAME_SIZE, 0); // Frame to hold pages
vector<int> pages; // Page sequence
int page_faults = 0;
int page_hits = 0;

// Function to display the content of the frame
void displayFrame() {
    cout << "Frame Content: ";
    for (int i = 0; i < FRAME_SIZE; ++i) {
        if (frame[i] == 0)
            cout << "[ ] ";
        else
            cout << "[" << frame[i] << "] ";
    }
    cout << endl;
}

// Function to find the index of the page in the frame
int findPageIndex(int page) {
    auto it = find(frame.begin(), frame.end(), page);
    if (it != frame.end())
        return distance(frame.begin(), it);
    else
        return -1;
}

// Function to simulate the Adaptive Replacement Cache algorithm
void* simulateARC(void* arg) {
    for (int i = 0; i < pages.size(); ++i) {
        int page = pages[i];
        int index = findPageIndex(page);

        if (index != -1) { // Page Hit
            page_hits++;
            frame.erase(frame.begin() + index);
            frame.push_back(page);
        } else { // Page Fault
            page_faults++;
            if (frame.size() < FRAME_SIZE) {
                frame.push_back(page);
            } else {
                frame.erase(frame.begin());
                frame.push_back(page);
            }
        }

        displayFrame();
        cout << "Page Faults: " << page_faults << ", Page Hits: " << page_hits << endl;
    }
    pthread_exit(NULL);
}

int main() {
    int n;
    cout<<"\n...This is ADAPTIVE REPLACEMENT CACHE replacement algorithm...\n";
    cout << "Enter number of pages: ";
    cin >> n;

    cout << "Enter the page sequence: ";
    for (int i = 0; i < n; ++i) {
        int page;
        cin >> page;
        pages.push_back(page);
    }

    pthread_t thread;
    pthread_create(&thread, NULL, simulateARC, NULL);
    pthread_join(thread, NULL);

    cout << "Simulation completed." << endl;
    cout << "Total Page Faults: " << page_faults << ", Total Page Hits: " << page_hits << endl;

    return 0;
}
