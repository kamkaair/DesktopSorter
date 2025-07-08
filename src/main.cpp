#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

// File system
#include <filesystem>
#include <fstream>

// Get the desktop location
#include <Windows.h>
#include <shlobj.h>

// Time
#include <chrono>
#include <iterator>

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
        add,
        remove,
        exit,
        clear,

        noCommand
    };

    std::vector<allowedFileTypes> AllTypes = { allowedFileTypes::png, allowedFileTypes::jpg, allowedFileTypes::webp,
        allowedFileTypes::gif, allowedFileTypes::docx, allowedFileTypes::pdf };

    //std::vector<const char*> typeList = { ".png", ".PNG", ".jpg", ".webp", ".gif", ".docx", ".pdf" };
    //std::vector<const char*> folderTypes = { ".png", ".jpg", ".webp", ".gif", ".docx", ".pdf" };
    std::vector<const char*> folderTypes;
    std::vector<const char*> folderTags = { "Unreal" };
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
    else if ((key == 'W') || (key == 'w')) return Enum::allowedCommands::add;
    else if ((key == 'R') || (key == 'r')) return Enum::allowedCommands::remove;
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

bool addFileType(const char* addedFile) {
    //bool found = (std::find(Enum::typeList.begin(), Enum::typeList.end(), filetype) != Enum::typeList.end());

    //std::cout << addedFile << endl;
    //cout << endl;

    //if (std::find(Enum::folderTypes.begin(), Enum::folderTypes.end(), addedFile) != Enum::folderTypes.end()) {
    //    std::cout << "not sexisting!" << endl;
    //}
    //else {
    //    std::cout << "sexistings!!!!" << endl;
    //}

    for (int i = 0; i < Enum::folderTypes.size(); i++) {
        if (std::strcmp(Enum::folderTypes[i], addedFile) == 0) {
            cout << "Found type in the folder! " << Enum::folderTypes[i] << " - " << addedFile << endl;
        }
    }

    char* cpyAdd = new char[strlen(addedFile)];
    strcpy(cpyAdd, addedFile);

    Enum::folderTypes.push_back(cpyAdd);

    // TODO: Write the new changes into the save file

    cout << "Containing file types: " << endl;
    for (int i = 0; i < Enum::folderTypes.size(); i++) {
        cout << Enum::folderTypes[i] + string(" ");
    }
    cout << endl;

    return true;
}

void createFile(const char* file) {
    if (!filesystem::exists(path + "/sorted/" + file)) {
        ofstream createFile;
        createFile.open(path + "/sorted/" + file);

        std::vector<string> createFiles = { ".png", ".jpg", ".webp", ".gif", ".docx", ".pdf" };

        for (int i = 0; i < createFiles.size(); i++) {
            createFile << createFiles[i];
        }

        createFile.close();
        cout << file << " created" << endl;
    }
    else {
        cout << "'" << file << "'" << " -save file already exists, using the existing one!" << endl;
        cout << endl;

        string texties;
        ifstream readFile(path + "/sorted/" + file);

        while (getline(readFile, texties)) {
            const char* textChar = texties.c_str();
            char* textiesChar = new char[texties.size() + 1]; // allocate memory for the char

            strcpy(textiesChar, texties.c_str());
            Enum::folderTypes.push_back(textiesChar);
        }

        cout << "Containing file types: " << endl;
        for (int i = 0; i < Enum::folderTypes.size(); i++) {
            cout << Enum::folderTypes[i] + string(" ");
        }

        coutPrint("");

        readFile.close();
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

// Make a check for file size!! Ignore files bigger than 9 mb!!!
// https://en.cppreference.com/w/cpp/filesystem/file_size
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

// Make the filetype variable into a vector and give in all the filetypes you want to include
std::vector<string> findAllFiles(const string& filetype, bool shouldMoveFiles) {
    int iter = 0;
    std::vector<string> fileNames;

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();

    for (const auto& entry : filesystem::directory_iterator(path)) {

        //bool found = (std::find(Enum::typeList.begin(), Enum::typeList.end(), filetype) != Enum::typeList.end());

        //if (found) {
        //    if (isInString(entry, ".png") || (isInString(entry, ".PNG")))
        //    {
        //        fileNames.push_back(entry.path().filename().string());
        //        if (shouldMoveFiles)
        //            moveFile(entry.path().filename().string(), filetype.c_str());
        //        cout << entry << endl;
        //    }
        //    else {
        //        fileNames = processFiles(fileNames, entry, filetype, shouldMoveFiles);
        //    }
        //    iter++;
        //}
        //else {
        //    continue;
        //}

        for (const auto& enumType : Enum::AllTypes) { // Maybe replace this with a regular array with filetypes
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

    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    std::cout << ms_int.count() << "ms" << " - " << ms_double.count() << "ms" << std::endl;
    std::cout << "Iterations: " << iter << std::endl;
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
    cout << "'D': Show all desktop files" << endl;

    cout << endl;

    cout << "'W': Add an entry" << endl;
    cout << "'R': Remove an entry" << endl;

    cout << endl;

    cout << "'E': Exit" << endl;
    cout << "'C': Clean cmd" << endl;

    string input = "";
    bool shouldMoveFiles = true;

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
        createDirectory(Enum::folderTags[0]);// Testing tags (they should be first, because they take the priority over the filename)
        createFile("values.txt");
        allFiles = findAllFiles(string(input), shouldMoveFiles);
        for (size_t i = 0; i < Enum::folderTypes.size(); i++) {
            createDirectory(Enum::folderTypes[i]);
        }
        break;

    case (Enum::allowedCommands::all):
        coutPrint("Are you sure you want to transfer all of the files? Y/N (.png, .jpg, .webp, .gif, .docx)");
        cin >> selection;
        cout << endl;
        if ((selection == 'y') || (selection == 'Y')) {
            shouldMoveFiles = true;
            createDirectory(Enum::folderTags[0]);// Testing tags (they should be first, because they take the priority over the filename)
            createFile("values.txt");
            findAllTags(Enum::folderTags[0], shouldMoveFiles);
            for (size_t i = 0; i < Enum::folderTypes.size(); i++) {
                createDirectory(Enum::folderTypes[i]);
                findAllFiles(Enum::folderTypes[i], shouldMoveFiles);
            }
            coutPrint("Searched all the supported file types!");
        }
        break;

    case (Enum::allowedCommands::add):
        std::cin >> input;
        addFileType(input.c_str());
        break;

    case (Enum::allowedCommands::remove):

        break;

    case (Enum::allowedCommands::show):
        coutPrint("Showing all the available files... (.png, .jpg, .webp, .gif)");
        cout << endl;
        shouldMoveFiles = false;
        for (size_t i = 0; i < Enum::folderTypes.size(); i++) {
            //createDirectory(folderTypes[i]);
            findAllFiles(Enum::folderTypes[i], shouldMoveFiles);
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
    createFile("values.txt");

    while (!exit) {
        exit = loop(exit, allFiles);
    }
}