#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <functional>

//File system
#include <filesystem>

using namespace std;

std::string path = "C:/Users/altti/Desktop/";
std::string rootFolderPath = "C:/Users/altti/Desktop/sorted/";

enum class allowedFileTypes {
    // File types
    png,
    jpg,
    webp,
    gif,
    // Option selecting
    all,
    search,
    exit,
    clear,

    none
};

allowedFileTypes hashstring(const std::string& str) {
    if ((str == ".png") || (str == ".PNG")) return allowedFileTypes::png;
    else if (str == ".jpg")  return allowedFileTypes::jpg;
    else if (str == ".webp")  return allowedFileTypes::webp;
    else if (str == ".gif")  return allowedFileTypes::gif;
    return allowedFileTypes::none;
}

allowedFileTypes selectOption(const char& key) {
    if ((key == 'A') || (key == 'a')) return allowedFileTypes::all;
    else if ((key == 'C') || (key == 'c')) return allowedFileTypes::clear;
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
    if (!filesystem::exists(path + "sorted/" + folder)) {
        filesystem::create_directories(path + "sorted/" + folder);
        cout << folder << " created" << endl;
    }
    else {
        cout <<"'" << folder << "'" << " folder already exists, no new folders created!" << endl;
    }
}

// https://stackoverflow.com/questions/22201663/find-and-move-files-in-c#48614612
void moveFile(string fileName, const char* folder) {
    try {
        filesystem::copy(path + fileName, rootFolderPath + folder + "/" + fileName);
        filesystem::remove(path + fileName);
    }
    catch (filesystem::filesystem_error& error) {
        cout << error.what() << endl;
    }
}

bool isInString(const filesystem::directory_entry &dirString, const string& givenString) {
    string s_string = dirString.path().filename().string();
    // Check if the file size is larger than 5 mb
    bool b_inString = (dirString.file_size() <= 5000000 && s_string.find(givenString) < s_string.length()) ? /*if*/ b_inString = true : /*else*/ b_inString = false;

    return b_inString;
}

// Make a check for file size!! Ignore files bigger than 5 mb!!!
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
                moveFile(entry.path().filename().string(), filetype.c_str());
                cout << entry << endl;
            }
            break;
        case allowedFileTypes::jpg:
            if (isInString(entry, filetype))
            {
                fileNames.push_back(entry.path().filename().string());
                moveFile(entry.path().filename().string(), filetype.c_str());
                cout << entry << endl;
            }
            break;
        case allowedFileTypes::webp:
            if (isInString(entry, filetype))
            {
                fileNames.push_back(entry.path().filename().string());
                moveFile(entry.path().filename().string(), filetype.c_str());
                cout << entry << endl;
            }
            break;
        case allowedFileTypes::gif:
            if (isInString(entry, filetype))
            {
                fileNames.push_back(entry.path().filename().string());
                moveFile(entry.path().filename().string(), filetype.c_str());
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
    cout << "'S': Singular" << endl;
    cout << "'A': All" << endl;
    cout << "'E': Exit" << endl;
    cout << "'C': Clean cmd" << endl;

    string input = "";

    std::vector<const char*> folderTypes = {".png", ".jpg", ".webp", ".gif"};

    char selection;
    cin >> selection; // cin waits for user's input
    switch (selectOption(selection)) {
        //Added an enum to hande OR, since switch case can't handle ORs in their condition... odd
    case (allowedFileTypes::search):
        coutPrint("What file would you like to target?");
        cin >> input;
        allFiles = findAllFiles(string(input));
        for (size_t i = 0; i < folderTypes.size(); i++) {
            createDirectory(folderTypes[i]);
        }   
        break;

    case (allowedFileTypes::all):
        coutPrint("Are you sure you want to transfer all of the files? Y/N (.png, .jpg, .webp, .gif)");
        cin >> selection;
        if ((selection == 'y') || (selection == 'Y')) {
            for (size_t i = 0; i < folderTypes.size(); i++) {
                createDirectory(folderTypes[i]);
                findAllFiles(folderTypes[i]);
            }
            coutPrint("Searched all the supported file types!");
        }
        break;

    case (allowedFileTypes::exit):
        exit = true;
        coutPrint("Exiting...");
        break;

    case (allowedFileTypes::clear):
        system("CLS"); //cmd clear
        break;

    default:
        coutPrint("What?! Command not recognized");
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