#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SCORERS 5

typedef struct {
    char teamName[50];
    int score;
} Scorer;

Scorer heap[MAX_SCORERS];
int heapSize = 0;

void swap(Scorer *a, Scorer *b) {
    Scorer temp = *a;
    *a = *b;
    *b = temp;
}

void maxHeapify(int i, int size) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < size && heap[left].score > heap[largest].score)
        largest = left;

    if (right < size && heap[right].score > heap[largest].score)
        largest = right;

    if (largest != i) {
        swap(&heap[i], &heap[largest]);
        maxHeapify(largest, size);
    }
}

void insertScorer(Scorer newScorer) {
    if (heapSize < MAX_SCORERS) {
        // If the heap is not full, add the new scorer
        heap[heapSize] = newScorer;
        heapSize++;

        // Heapify from the bottom up
        for (int i = heapSize / 2 - 1; i >= 0; i--) {
            maxHeapify(i, heapSize);
        }
    } else if (newScorer.score > heap[0].score) {
        // If the heap is full and the new scorer's score is higher than the smallest in the heap
        heap[0] = newScorer;
        maxHeapify(0, heapSize);
    }
}

// Sort the heap in ascending order
void sortHeapDescending() {
    // First, sort the heap in ascending order
    for (int i = heapSize - 1; i > 0; i--) {
        swap(&heap[0], &heap[i]); // Move the root (largest element) to the end
        maxHeapify(0, i); // Heapify the reduced heap
    }

    // Reverse the array to get descending order
    for (int i = 0; i < heapSize / 2; i++) {
        swap(&heap[i], &heap[heapSize - 1 - i]);
    }
}

void displayTopScorers(HWND hwnd) {
    char buffer[1024] = {0};
    strcat(buffer, "Top 5 Scorers (Descending Order):\n");

    // Sort the heap in descending order before displaying
    sortHeapDescending();

    for (int i = 0; i < heapSize; i++) {
        char scorerInfo[100];
        sprintf(scorerInfo, "%s: %d\n", heap[i].teamName, heap[i].score);
        strcat(buffer, scorerInfo);
    }

    MessageBox(hwnd, buffer, "Top Scorers", MB_OK);
}

void AdjustControls(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    MoveWindow(GetDlgItem(hwnd, 1), 100, 10, width - 110, 25, TRUE);  // Team Name input
    MoveWindow(GetDlgItem(hwnd, 2), 100, 40, width - 110, 25, TRUE);  // Score input
    MoveWindow(GetDlgItem(hwnd, 3), 10, 80, (width - 30) / 2, 30, TRUE);  // Add Scorer button
    MoveWindow(GetDlgItem(hwnd, 4), 20 + (width - 30) / 2, 80, (width - 30) / 2, 30, TRUE);  // Show Top Scorers button
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            CreateWindow("STATIC", "Team Name:", WS_VISIBLE | WS_CHILD, 10, 10, 80, 25, hwnd, NULL, NULL, NULL);
            CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 10, 200, 25, hwnd, (HMENU)1, NULL, NULL);
            CreateWindow("STATIC", "Score:", WS_VISIBLE | WS_CHILD, 10, 40, 80, 25, hwnd, NULL, NULL, NULL);
            CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 40, 200, 25, hwnd, (HMENU)2, NULL, NULL);
            CreateWindow("BUTTON", "Add Scorer", WS_VISIBLE | WS_CHILD, 10, 80, 120, 30, hwnd, (HMENU)3, NULL, NULL);
            CreateWindow("BUTTON", "Show Top Scorers", WS_VISIBLE | WS_CHILD, 140, 80, 150, 30, hwnd, (HMENU)4, NULL, NULL);
            break;
        }
        case WM_SIZE: {
            AdjustControls(hwnd);
            break;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == 3) { // "Add Scorer" button clicked
                char teamName[50];
                char scoreText[10];
                GetWindowText(GetDlgItem(hwnd, 1), teamName, 50); // Get team name
                GetWindowText(GetDlgItem(hwnd, 2), scoreText, 10); // Get score
                int score = atoi(scoreText);

                if (strlen(teamName) > 0 && score > 0) { // Validate input
                    Scorer newScorer;
                    strcpy(newScorer.teamName, teamName);
                    newScorer.score = score;

                    insertScorer(newScorer);

                    // Clear input fields
                    SetWindowText(GetDlgItem(hwnd, 1), "");
                    SetWindowText(GetDlgItem(hwnd, 2), "");
                } else {
                    MessageBox(hwnd, "Please enter valid team name and score.", "Error", MB_OK | MB_ICONERROR);
                }
            } else if (LOWORD(wParam) == 4) { // "Show Top Scorers" button clicked
                displayTopScorers(hwnd);
            }
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "TopScorersApp";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("TopScorersApp", "Top Scorers Application", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 200, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

