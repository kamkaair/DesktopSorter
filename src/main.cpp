#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

// File system
#include <filesystem>
// Get the desktop location
#include <Windows.h>
#include <shlobj.h>

#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>

using namespace std;

std::string path;
std::string rootFolderPath;

namespace Enum {
    enum class allowedFileTypes {
        // File types
        png,
        jpg,
        webp,
        gif,
        docx,
        pdf,

        none
    };

    enum class allowedTags {
        // Tags
        unreal,

        noTag
    };

    enum class allowedCommands {
        // Option selecting
        all,
        singular,
        show,
        exit,
        clear,

        noCommand
    };

    std::vector<allowedFileTypes> AllTypes = { allowedFileTypes::png, allowedFileTypes::jpg, allowedFileTypes::webp,
        allowedFileTypes::gif, allowedFileTypes::docx, allowedFileTypes::pdf };
}

Enum::allowedFileTypes hashstring(const std::string& str) {
    if ((str == ".png") || (str == ".PNG")) return Enum::allowedFileTypes::png;
    else if (str == ".jpg") return Enum::allowedFileTypes::jpg;
    else if (str == ".webp") return Enum::allowedFileTypes::webp;
    else if (str == ".gif") return Enum::allowedFileTypes::gif;
    else if (str == ".docx") return Enum::allowedFileTypes::docx;
    else if (str == ".pdf") return Enum::allowedFileTypes::pdf;
    return Enum::allowedFileTypes::none;
}

Enum::allowedTags hashTag(const std::string& str) {
    if (str == "Unreal") return Enum::allowedTags::unreal;
    return Enum::allowedTags::noTag;
}

Enum::allowedCommands selectOption(const char& key) {
    if ((key == 'A') || (key == 'a')) return Enum::allowedCommands::all;
    else if ((key == 'C') || (key == 'c')) return Enum::allowedCommands::clear;
    else if ((key == 'S') || (key == 's')) return Enum::allowedCommands::singular;
    else if ((key == 'E') || (key == 'e')) return Enum::allowedCommands::exit;
    else if ((key == 'D') || (key == 'd')) return Enum::allowedCommands::show;
    return Enum::allowedCommands::noCommand;
}

void getWinDesktopPath() {
    char winPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, winPath))) {
        path = std::string(winPath) + "/";
        rootFolderPath = winPath + std::string("/sorted/");
    }
    cout << "Current Windows desktop path: " << path << endl;
    cout << "Current folder path: " << rootFolderPath << endl;
    cout << "" << endl;
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
    if (!filesystem::exists(path + "/sorted/" + folder)) {
        filesystem::create_directories(path + "/sorted/" + folder);
        cout << folder << " created" << endl;
    }
    else {
        cout << "'" << folder << "'" << " folder already exists, no new folders created!" << endl;
        cout << endl;
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

bool isInString(const filesystem::directory_entry& dirString, const string& givenString) {
    string s_string = dirString.path().filename().string();
    // Check if the file size is larger than 9 mb
    bool b_inString = (dirString.file_size() <= 9000000 && s_string.find(givenString) < s_string.length()) ? /*if*/ b_inString = true : /*else*/ b_inString = false;

    return b_inString;
}

std::vector<string> processFiles(std::vector<string> fileNames, const filesystem::directory_entry& entry, const string& filetype, bool shouldMoveFiles) {
    if (isInString(entry, filetype))
    {
        fileNames.push_back(entry.path().filename().string());
        if (shouldMoveFiles)
            moveFile(entry.path().filename().string(), filetype.c_str());
        cout << entry << endl;
    }
    return fileNames;
}

// Make a check for file size!! Ignore files bigger than 5 mb!!!
// https://en.cppreference.com/w/cpp/filesystem/file_size

// Make the filetype variable into a vector and give in all the filetypes you want to include
std::vector<string> findAllFiles(const string& filetype, bool shouldMoveFiles) {
    int iter = 0;
    std::vector<string> fileNames;
    for (const auto& entry : filesystem::directory_iterator(path)) {
        for (const auto& enumType : Enum::AllTypes) {
            if ((hashstring(filetype) == enumType)) {
                if (isInString(entry, ".png") || (isInString(entry, ".PNG")))
                {
                    fileNames.push_back(entry.path().filename().string());
                    if (shouldMoveFiles)
                        moveFile(entry.path().filename().string(), filetype.c_str());
                    cout << entry << endl;
                }
                else {
                    fileNames = processFiles(fileNames, entry, filetype, shouldMoveFiles);
                }
            }
            iter++;
        }
    }
    std::cout << iter << std::endl;
    cout << "Amount of files: " << fileNames.size() << endl;
    coutPrint("//////////////////////////////////");
    return fileNames;
}

std::vector<string> findAllTags(const string& tag, bool shouldMoveFiles) {

    std::vector<string> fileNames;
    for (const auto& entry : filesystem::directory_iterator(path)) {
        switch (hashTag(tag)) {
        case Enum::allowedTags::unreal:
            fileNames = processFiles(fileNames, entry, tag, shouldMoveFiles);
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
    cout << "'D': Show all files" << endl;
    cout << "'E': Exit" << endl;
    cout << "'C': Clean cmd" << endl;

    string input = "";
    bool shouldMoveFiles = true;
    std::vector<const char*> folderTypes = { ".png", ".jpg", ".webp", ".gif", ".docx", ".pdf"};
    std::vector<const char*> folderTags = { "Unreal" };

    char selection;
    cin >> selection; // cin waits for user's input
    cout << endl;
    switch (selectOption(selection)) {
        //Added an enum to hande OR, since switch case can't handle ORs in their condition... odd
    case (Enum::allowedCommands::singular):
        coutPrint("What file would you like to target?");
        cin >> input;
        cout << endl;
        shouldMoveFiles = true;
        allFiles = findAllFiles(string(input), shouldMoveFiles);
        for (size_t i = 0; i < folderTypes.size(); i++) {
            createDirectory(folderTypes[i]);
        }
        break;

    case (Enum::allowedCommands::all):
        coutPrint("Are you sure you want to transfer all of the files? Y/N (.png, .jpg, .webp, .gif, .docx)");
        cin >> selection;
        cout << endl;
        if ((selection == 'y') || (selection == 'Y')) {
            shouldMoveFiles = true;
            createDirectory(folderTags[0]);// Testing tags (they should be first, because they take the priority over the filename)
            findAllTags(folderTags[0], shouldMoveFiles);
            for (size_t i = 0; i < folderTypes.size(); i++) {
                createDirectory(folderTypes[i]);
                findAllFiles(folderTypes[i], shouldMoveFiles);
            }
            coutPrint("Searched all the supported file types!");
        }
        break;

    case (Enum::allowedCommands::show):
        coutPrint("Showing all the available files... (.png, .jpg, .webp, .gif)");
        cout << endl;
        shouldMoveFiles = false;
        for (size_t i = 0; i < folderTypes.size(); i++) {
            //createDirectory(folderTypes[i]);
            findAllFiles(folderTypes[i], shouldMoveFiles);
        }
        coutPrint("Searched all the supported file types!");
        break;

    case (Enum::allowedCommands::exit):
        exit = true;
        coutPrint("Exiting...");
        break;

    case (Enum::allowedCommands::clear):
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
    getWinDesktopPath();

    while (!exit) {
        exit = loop(exit, allFiles);
    }
}