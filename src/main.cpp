#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <functional>

//File system
#include <filesystem>

using namespace std;

std::string path = "C:/Users/altti/Desktop/";

enum class allowedFileTypes {
    // File types
    png,
    PNG,
    jpg,
    webp,
    // Option selecting
    all,
    search,
    exit,

    none
};

allowedFileTypes hashstring(const std::string& str) {
    if ((str == ".png") || (str == ".PNG")) return allowedFileTypes::png;
    else if (str == ".jpg")  return allowedFileTypes::jpg;
    else if (str == ".webp")  return allowedFileTypes::webp;
    return allowedFileTypes::none;
}

allowedFileTypes selectOption(const char& key) {
    if ((key == 'A') || (key == 'a')) return allowedFileTypes::all;
    else if ((key == 'S') || (key == 's')) return allowedFileTypes::search;
    else if ((key == 'E') || (key == 'e')) return allowedFileTypes::exit;
    return allowedFileTypes::none;
}

void coutPrint(const char* text) {
    cout << endl;
    cout << text << endl;
    cout << endl;
}

void coutPrint(string text) {
    cout << endl;
    cout << text << endl;
    cout << endl;
}

void createDirectory(const char* folder) {
    if (!filesystem::exists(path + folder)) {
        filesystem::create_directory(path + folder);
        cout << folder << " created" << endl;
    }
    else {
        cout <<"'" << folder << "'" << " folder already exists, no new folders created!" << endl;
    }

}

bool isInString(const filesystem::directory_entry &dirString, const string& givenString) {
    string s_string = dirString.path().filename().string();

    bool b_inString = (dirString.file_size() <= 500000 && s_string.find(givenString) < s_string.length()) ? /*if*/ b_inString = true : /*else*/ b_inString = false;

    return b_inString;
}

// Make a check for file size!! Ignore files bigger than 500 mb!!!
// https://en.cppreference.com/w/cpp/filesystem/file_size

// Make the filetype variable into a vector and give in all the filetypes you want to include
std::vector<string> findAllFiles(const string& filetype) {

    enum allowedFileTypes fileT;

    std::vector<string> fileNames;
    for (const auto& entry : filesystem::directory_iterator(path)) {

        switch (hashstring(filetype)) {
        case allowedFileTypes::png:
            if (isInString(entry, ".png") || (isInString(entry, ".PNG")))
            {
                fileNames.push_back(entry.path().filename().string());
                cout << entry << endl;
            }
            break;
        case allowedFileTypes::jpg:
            if (isInString(entry, filetype))
            {
                fileNames.push_back(entry.path().filename().string());
                cout << entry << endl;
            }
            break;
        case allowedFileTypes::webp:
            if (isInString(entry, filetype))
            {
                fileNames.push_back(entry.path().filename().string());
                cout << entry << endl;
            }
            break;
        }
    }
    cout << "Amount of files: " << fileNames.size() << endl;
    coutPrint("//////////////////////////////////");
    return fileNames;
}

bool loop(bool exit, std::vector<string> allFiles) {
    cout << "MENU:" << endl;
    cout << "--------" << endl;
    cout << "Do you want to orgazine all files?" << endl;
    cout << "'S': singular" << endl;
    cout << "'A': All" << endl;
    cout << "'E': Exit" << endl;

    string input = "";

    std::vector<const char*> folderTypes = {".png", ".jpg", ".webp"};

    char selection;
    cin >> selection; // cin waits for user's input
    switch (selectOption(selection)) {
        //Added an enum to hande OR, since switch case can't handle them in their condition... odd
    case (allowedFileTypes::all):
        coutPrint("What file would you like to target?");
        cin >> input;
        allFiles = findAllFiles(string(input));
        for (size_t i = 0; i < folderTypes.size(); i++) {
            createDirectory(folderTypes[i]);
        }   
        break;
    case (allowedFileTypes::search):
        coutPrint("Ei vittu sit...");
        for (size_t i = 0; i < folderTypes.size(); i++) {
            findAllFiles(folderTypes[i]);
        }
        break;
    case (allowedFileTypes::exit):
        exit = true;
        coutPrint("Exiting...");
        break;
    default:
        coutPrint("What?!");
    }
    return exit;
}

void main() {
    bool exit = false;
    std::vector<string> allFiles;

    while (!exit) {
        exit = loop(exit, allFiles);
    }
}

/*
* #include <mutex>
#include <thread>
#include <atomic>
#include <time.h>
* 
    using namespace std;

void coutPrint(const char* text) {
    cout << endl;
    cout << text << endl;
    cout << endl;
}

void child(atomic<bool>& done, atomic<clock_t>& now, clock_t start)
{
    cout << "Child START!" << endl;
    while (!done) {
        //cout << "child doing work" << endl;
        now = clock() - start;
        if (now >= 12000 && !done) {
            done = 1;
            cout << "done by child at " << now << endl;
        }
        //cout << "child at " << now << endl;
        this_thread::yield();
    }
}

void startThread(atomic<bool>& done, atomic<clock_t>& now, clock_t start) {
    thread t(child, std::ref(done), std::ref(now), start); // attention, without ref, you get clones
    t.join();
}

//void parent(atomic<bool>& done, atomic<clock_t>& now, clock_t start)
//{
//    cout << "Parent START!" << endl;
//    while (!done) {
//        //cout << "parent doing work" << endl;
//        now = clock() - start;
//        if (now >= 2000 && !done) {
//            done = 1;
//            cout << "done by parent at " << now << endl;
//        }
//        //cout << "parent at " << now << endl;
//        this_thread::yield();
//    }
//}

bool parent(atomic<bool>& done, atomic<clock_t>& now, clock_t start, bool exit)
{
    cout << "Parent START!" << endl;
    while (!done) {

        cout << "MENU:" << endl;
        cout << "--------" << endl;
        cout << "0: none" << endl;
        cout << "1: Start Timer" << endl;
        cout << "2: Exit" << endl;

        int selection = 0;
        cin >> selection; // cin waits for user's input
        switch (selection) {
        case 0:
            break;
        case 1:
            startThread(done, now, start);
            break;
        case 2:
            exit = true;
            coutPrint("Exiting...");
            break;
        default:
            coutPrint("What?!");
        }
        //cout << "parent at " << now << endl;
        this_thread::yield();
        return exit;
    }
}

//void startTimer() {
//    atomic<bool> done{ 0 };
//    clock_t start = clock();
//    atomic<clock_t> now;
//
//    thread t(child, std::ref(done), std::ref(now), start); // attention, without ref, you get clones
//    parent(done, now, start);
//    t.join();
//}

bool loop(bool exit) {
    cout << "MENU:" << endl;
    cout << "--------" << endl;
    cout << "0: none" << endl;
    cout << "1: Start Timer" << endl;
    cout << "2: Exit" << endl;

    int selection = 0;
    cin >> selection; // cin waits for user's input
    switch (selection) {
    case 0:
        break;
    case 1:
        //startTimer();
        break;
    case 2:
        exit = true;
        coutPrint("Exiting...");
        break;
    default:
        coutPrint("What?!");
    }
    return exit;
}

void main() {
    bool exit = false;

    atomic<bool> done{ 0 };
    clock_t start = clock();
    atomic<clock_t> now;

    //thread t(child, std::ref(done), std::ref(now), start); // attention, without ref, you get clones
    parent(done, now, start, exit);
    //t.join();
    //while (!exit) {
    //    exit = loop(exit);
    //}
}
*/