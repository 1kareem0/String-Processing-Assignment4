/*
Assignment 4: String Processing

This program implements string matching using:
- Boyer-Moore Algorithm
- Rabin-Karp Algorithm

Features:
- Load text from file
- Manual text input
- Multiple pattern search
- Pattern highlighting
- Algorithm comparison
- Statistics (comparisons, matches, time)
*/

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
using namespace std;

// global text storage
string text;

// ASCII size for bad character table
#define ASCII_CHAR_NO 256

// statistics structure
struct Stats {
    int comparisons = 0;
    int matches = 0;
    double timeMs = 0;
    string algorithm;
};

// load text from file
void loadFile() {

    string filename;
    cout << "Enter file name: ";
    cin >> filename;

    ifstream file(filename);

    if (!file) {
        cout << "Error opening file\n";
        return;
    }

    text = "";
    string line;

    cin.ignore();

    while (getline(file, line)) {
        text += line + " ";
    }

    file.close();

    cout << "File loaded successfully\n";
}

// manual text input
void manualInput() {

    cin.ignore();
    cout << "Enter text:\n";

    getline(cin, text);

    cout << "Text stored successfully\n";
}

// highlight matched patterns
void printHighlighted(string pattern, vector<int> matches) {

    int m = pattern.length();
    int n = text.length();

    vector<bool> open(n + 1, false);
    vector<bool> close(n + 1, false);

    for (int pos : matches) {
        open[pos] = true;
        close[pos + m - 1] = true;
    }

    for (int i = 0; i < n; i++) {

        if (open[i]) cout << "[";

        cout << text[i];

        if (close[i]) cout << "]";
    }

    cout << "\n";
}

// build bad character table
void badCharHeuristic(string pattern, int badchar[], int size) {

    for (int i = 0; i < ASCII_CHAR_NO; i++)
        badchar[i] = -1;

    for (int i = 0; i < size; i++)
        badchar[(int)pattern[i]] = size - i - 1;
}

// Boyer-Moore search
Stats boyerMoorSearch(string pattern) {

    Stats st;
    st.algorithm = "Boyer-Moore";

    vector<int> matches;

    int m = pattern.length();
    int n = text.length();

    int badChar[ASCII_CHAR_NO];
    badCharHeuristic(pattern, badChar, m);

    int s = 0;

    auto start = chrono::high_resolution_clock::now();

    while (s <= n - m) {

        int j = m - 1;

        st.comparisons++;

        while (j >= 0 && pattern[j] == text[s + j]) {
            st.comparisons++;
            j--;
        }

        if (j < 0) {
            matches.push_back(s);
            st.matches++;
            s++;
        }
        else {
            int shift = badChar[(int)text[s + j]];
            if (shift < 1) shift = 1;
            s += shift;
        }
    }

    auto end = chrono::high_resolution_clock::now();

    st.timeMs = chrono::duration<double, milli>(end - start).count();

    printHighlighted(pattern, matches);

    for (int i : matches)
        cout << "Match at index: " << i << "\n";

    if (matches.empty())
        cout << "NO MATCHES\n";

    return st;
}

// Rabin-Karp search
Stats rabinKarpSearch(string pattern) {

    Stats st;
    st.algorithm = "Rabin-Karp";

    vector<int> matches;

    int d = ASCII_CHAR_NO;
    int q = 101;

    int m = pattern.length();
    int n = text.length();

    int p = 0, t = 0, h = 1;

    for (int i = 0; i < m - 1; i++)
        h = (h * d) % q;

    for (int i = 0; i < m; i++) {
        p = (d * p + pattern[i]) % q;
        t = (d * t + text[i]) % q;
    }

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i <= n - m; i++) {

        st.comparisons++;

        if (p == t) {

            bool match = true;

            for (int j = 0; j < m; j++) {

                st.comparisons++;

                if (text[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                matches.push_back(i);
                st.matches++;
            }
        }

        if (i < n - m) {
            t = (d * (t - text[i] * h) + text[i + m]) % q;
            if (t < 0) t += q;
        }
    }

    auto end = chrono::high_resolution_clock::now();

    st.timeMs = chrono::duration<double, milli>(end - start).count();

    printHighlighted(pattern, matches);

    for (int i : matches)
        cout << "Match at index: " << i << "\n";

    if (matches.empty())
        cout << "NO MATCHES\n";

    return st;
}

// print statistics
void printStatistics(Stats st) {

    cout << "\n===== STATISTICS =====\n";
    cout << "Algorithm: " << st.algorithm << "\n";
    cout << "Matches: " << st.matches << "\n";
    cout << "Comparisons: " << st.comparisons << "\n";
    cout << "Time: " << st.timeMs << " ms\n";
}

// multiple pattern input
vector<string> getPatterns() {

    vector<string> patterns;
    string input;

    cout << "Enter patterns (space-separated): ";
    cin.ignore();
    getline(cin, input);

    string temp = "";

    for (char c : input) {

        if (c == ' ') {
            if (!temp.empty()) {
                patterns.push_back(temp);
                temp = "";
            }
        }
        else {
            temp += c;
        }
    }

    if (!temp.empty())
        patterns.push_back(temp);

    return patterns;
}

// main menu
int main() {

    int choice = 0;

    while (true) {

        cout << "\n1. Load Text File\n";
        cout << "2. Enter Text Manually\n";
        cout << "3. Search Using Boyer-Moore\n";
        cout << "4. Search Using Rabin-Karp\n";
        cout << "5. Compare Algorithms\n";
        cout << "6. Exit\n";

        cout << "Choice: ";
        cin >> choice;

        if (choice == 6)
            break;

        vector<string> patterns;

        switch (choice) {

        case 1:
            loadFile();
            break;

        case 2:
            manualInput();
            break;

        case 3:
            patterns = getPatterns();

            for (string p : patterns) {
                cout << "\nPattern: " << p << "\n";
                printStatistics(boyerMoorSearch(p));
            }
            break;

        case 4:
            patterns = getPatterns();

            for (string p : patterns) {
                cout << "\nPattern: " << p << "\n";
                printStatistics(rabinKarpSearch(p));
            }
            break;

        case 5:
            patterns = getPatterns();

            for (string p : patterns) {
                cout << "\n===== Pattern: " << p << " =====\n";

                Stats bm = boyerMoorSearch(p);
                Stats rk = rabinKarpSearch(p);

                printStatistics(bm);
                printStatistics(rk);
            }
            break;

        default:
            cout << "Invalid choice\n";
            break;
        }
    }

    return 0;
}