#include <iostream>
#include <vector>
#include <algorithm>
#include <pthread.h>

using namespace std;

#define FRAME_SIZE 4

vector<int> frame(FRAME_SIZE, 0); // Frame to hold pages
vector<bool> reference_bit(FRAME_SIZE, false); // Reference bit for each frame entry
vector<bool> modified_bit(FRAME_SIZE, false); // Modified bit for each frame entry
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

// Function to simulate the Enhanced Second Chance algorithm
void* simulateEnhancedSecondChance(void* arg) {
    for (int i = 0; i < pages.size(); ++i) {
        int page = pages[i];
        int index = findPageIndex(page);

        if (index != -1) { // Page Hit
            page_hits++;
            reference_bit[index] = true;
        } else { // Page Fault
            page_faults++;

            bool page_replaced = false;
            while (!page_replaced) {
                for (int j = 0; j < FRAME_SIZE; ++j) {
                    if (!reference_bit[j] && !modified_bit[j]) {
                        frame[j] = page;
                        reference_bit[j] = true;
                        page_replaced = true;
                        break;
                    } else {
                        reference_bit[j] = false;
                    }
                }

                if (!page_replaced) {
                    for (int j = 0; j < FRAME_SIZE; ++j) {
                        if (!reference_bit[j] && modified_bit[j]) {
                            frame[j] = page;
                            reference_bit[j] = true;
                            page_replaced = true;
                            break;
                        } else {
                            reference_bit[j] = false;
                        }
                    }
                }
            }
        }

        displayFrame();
        cout << "Page Faults: " << page_faults << ", Page Hits: " << page_hits << endl;
    }
    pthread_exit(NULL);
}

int main() {
    int n;
    cout<<"\nThis is ENHANCED SECOND CHANCE algorithm...\n";
    cout << "Enter number of pages: ";
    cin >> n;

    cout << "Enter the page sequence: ";
    for (int i = 0; i < n; ++i) {
        int page;
        cin >> page;
        pages.push_back(page);
    }

    pthread_t thread;
    pthread_create(&thread, NULL, simulateEnhancedSecondChance, NULL);
    pthread_join(thread, NULL);

    cout << "Simulation completed." << endl;
    cout << "Total Page Faults: " << page_faults << ", Total Page Hits: " << page_hits << endl;

    return 0;
}
