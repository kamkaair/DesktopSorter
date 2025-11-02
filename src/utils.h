#pragma once

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

namespace Enum {
    enum class allowedCommands {
        // Option selecting
        all,
        singular,
        add,
        remove,
        showfiles,
        showtype,
        debug,
        clearall,
        exit,
        clear,

        noCommand
    };

    enum lineTypes {
        files = '1',
        tags = '2'
    };

    std::vector<const char*> folderTypes;
    std::vector<const char*> folderTags;

    std::string path;
    std::string rootFolderPath;
    const char* saveFile = "values.txt";
    bool debug = true, clearAll = false;
}

////////////////////
// Option selecting
////////////////////

Enum::allowedCommands selectOption(const char key) {
    char lowerKey = std::tolower(static_cast<unsigned char>(key));

    switch (lowerKey) {
    case ('a'): return Enum::allowedCommands::all;
    case ('c'): return Enum::allowedCommands::clear;
    case ('s'): return Enum::allowedCommands::singular;
    case ('w'): return Enum::allowedCommands::add;
    case ('r'): return Enum::allowedCommands::remove;
    case ('q'): return Enum::allowedCommands::showtype;
    case ('t'): return Enum::allowedCommands::debug;
    case ('g'): return Enum::allowedCommands::clearall;
    case ('d'): return Enum::allowedCommands::showfiles;
    case ('e'): return Enum::allowedCommands::exit;

    default: return Enum::allowedCommands::noCommand;
    }
}

////////////////////
// Get windows path
////////////////////

void getWinDesktopPath() {
    char winPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, winPath))) {
        Enum::path = std::string(winPath) + "/";
        Enum::rootFolderPath = winPath + std::string("/sorted/");
    }
    cout << "Current Windows desktop path: " << Enum::path << endl;
    cout << "Current folder path: " << Enum::rootFolderPath << endl;
    cout << "" << endl;
}

////////////////////
// General helpers
////////////////////

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

void eraseCLS(char selection) {
    if (Enum::clearAll) {
        system("CLS"); //cmd clear
        cout << "> " << selection << endl;
    }
}

////////////////////
// File editing
////////////////////

void openFileEdit(ofstream& addFile, const char* file) {
    addFile.open(Enum::path + "/sorted/" + file);
    cout << "Opened file: " << file << endl;
}

void closeFileEdit(ofstream& addFile) {
    coutPrint("//////////////////////////////////");
    addFile.close();
}

void createDirectory(const char* folder) {
    if (!filesystem::exists(Enum::path + "/sorted/" + folder)) {
        filesystem::create_directories(Enum::path + "/sorted/" + folder);
        cout << folder << " created" << endl;
    }
    else {
        if (Enum::debug) {
            cout << "'" << folder << "'" << " folder already exists, no new folders created!" << endl;
            cout << endl;
        }
    }
}

bool doesFileExist(const char* file) {
    if (!filesystem::exists(Enum::path + "/sorted/" + file))
        return false;

    return true;
}

void writeSaveEntry(ofstream& addFile, vector<const char*>& container, int count) {
    addFile << to_string(count) + string("{");
    for (int i = 0; i < container.size(); i++) {
        cout << container[i] + string(" ");
        addFile << container[i] + string(";");
    }
    addFile << string("}") + string("\n");
}

// Function to be used with openFileEdit() and closeFileEdit()
bool newWriteSave(const char* addedFile, vector<const char*>& container, ofstream& addFile, int line, bool addToVector) {
    if (addToVector) {
        char* cpyAdd = new char[strlen(addedFile)];
        strcpy_s(cpyAdd, strlen(addedFile) + 1, addedFile);

        container.push_back(cpyAdd);
    }

    writeSaveEntry(addFile, container, line);

    return true;
}

bool newRemoveSave(const char* addedFile, vector<const char*>& container, int i) {
    char* cpyAdd = new char[strlen(addedFile)];
    strcpy_s(cpyAdd, strlen(addedFile) + 1, addedFile);

    container.erase(container.begin() + i);

    ofstream addFile;
    string strCache;

    addFile.open(Enum::path + "/sorted/" + Enum::saveFile);

    cout << "New items: " << endl;
    writeSaveEntry(addFile, Enum::folderTypes, 1);
    writeSaveEntry(addFile, Enum::folderTags, 2);

    coutPrint("//////////////////////////////////");

    addFile.close();

    return true;
}

void readLines(string lineText, vector<const char*>& container, string items) {
    string strCache;
    vector<string> strVec;

    for (int i = 1; i < lineText.size(); i++) { // skip the first entry, since it contains the line number / ID
        if (lineText[i] == ';') { // Push_back the current selection of characters and clear the string
            strVec.push_back(strCache);
            strCache = "";
        }
        else if (lineText[i] == '}') { // End if found
            break;
        }
        else if (lineText[i] == '{') {
            continue;
        }
        else {
            strCache = strCache + lineText[i]; // Add the current character into the cache
        }
    }

    cout << "Containing following " << items << ": " << endl;
    cout << "{ ";
    for (int i = 0; i < strVec.size(); i++) {
        const char* strC = strVec[i].c_str();
        char* newChar = new char[strVec[i].size() + 1];

        strcpy_s(newChar, strVec[i].size() + 1, strVec[i].c_str()); // convert from string to char

        container.push_back(newChar);
        cout << container[i] << "; ";
    }
    cout << "}" << endl;
    cout << endl;
}

bool readSaveNew(const char* addedFile) {
    // Push line one to filetypes, line two to tags
    string lineText;
    ifstream readFile(Enum::path + "/sorted/" + addedFile);

    while (getline(readFile, lineText)) {

        //if ('1' == lineText[0]) {
        if (Enum::lineTypes::files == lineText[0]) { // files = '1'
            readLines(lineText, Enum::folderTypes, "filetypes");
        }
        //else if ('2' == lineText[0]) {
        else if (Enum::lineTypes::tags == lineText[0]) { // tags = '2'
            readLines(lineText, Enum::folderTags, "tags");
        }
    }
    readFile.close();

    return true;
}

bool addFileType(const char* addedFile) {
    ofstream editFile;
    int lineEdit = 1;

    for (int i = 0; i < Enum::folderTypes.size(); i++) {
        if (std::strcmp(Enum::folderTypes[i], addedFile) == 0) {
            cout << "Found type in the folder! " << Enum::folderTypes[i] << " - " << addedFile << endl;
            coutPrint("//////////////////////////////////");
            return false;
        }
    }
    openFileEdit(editFile, Enum::saveFile);
    if (addedFile[0] == '.') { // This way of adding and writing saves is probably quite stupid, but it'll do for now
        lineEdit = 1;
        cout << string("Adding ") + addedFile + string(" to file types") << endl;
        if (!newWriteSave(addedFile, Enum::folderTypes, editFile, lineEdit, true))
            return false;
        writeSaveEntry(editFile, Enum::folderTags, 2);
    }
    else {
        lineEdit = 2;
        cout << string("Adding ") + addedFile + string(" to tags") << endl;
        writeSaveEntry(editFile, Enum::folderTypes, 1);
        if (!newWriteSave(addedFile, Enum::folderTags, editFile, lineEdit, true))
            return false;
    }
    closeFileEdit(editFile);

    return true;
}

bool removeFileType(const char* addedFile) {
    for (int i = 0; i < Enum::folderTypes.size(); i++) {
        if (std::strcmp(Enum::folderTypes[i], addedFile) == 0) {
            cout << "Found " << Enum::folderTypes[i] << " -type in the folder! Deleting..." << endl;
            cout << endl;
            newRemoveSave(addedFile, Enum::folderTypes, i);
            //return false;

            return true;
        }
    }

    for (int i = 0; i < Enum::folderTags.size(); i++) {
        if (std::strcmp(Enum::folderTags[i], addedFile) == 0) {
            cout << "Found " << Enum::folderTags[i] << " -type in the folder! Deleting..." << endl;
            cout << endl;
            newRemoveSave(addedFile, Enum::folderTags, i);
            //return false;

            return true;
        }
    }
    return false;
}

void readWriteFile(const char* file) {
    if (!filesystem::exists(Enum::path + "/sorted/" + file)) {
        cout << string(file) + " doesn't exist, creating... " << endl;
        cout << endl;
        std::vector<const char*> createTypes = { ".png", ".jpg", ".webp", ".gif" };
        std::vector<const char*> createTags = { "Unreal", "codes" };

        ofstream addFile;

        openFileEdit(addFile, Enum::saveFile);
        if (!newWriteSave(file, createTypes, addFile, 1, false))
            cout << "value.txt creation failed! Error in file types" << endl;

        if (!newWriteSave(file, createTags, addFile, 2, false))
            cout << "value.txt creation failed! Error in tags" << endl;

        // Add the new types and tags into the a standard vector
        for (const char* files : createTypes)
            Enum::folderTypes.push_back(files);
        for (const char* tags : createTags)
            Enum::folderTags.push_back(tags);
        //Enum::folderBool.push_back(createBool[0]);

        closeFileEdit(addFile);
    }
    else {
        if (Enum::debug)
            cout << "'" << file << "'" << " -save file already exists, using the existing one!" << endl; cout << endl;

        if (!readSaveNew(file))
            cout << file << " file reading failed" << endl;
    }
}

// https://stackoverflow.com/questions/22201663/find-and-move-files-in-c#48614612
void moveFile(string fileName, const char* folder) {
    try {
        filesystem::copy(Enum::path + fileName, Enum::rootFolderPath + folder + "/" + fileName);
        filesystem::remove(Enum::path + fileName);
    }
    catch (filesystem::filesystem_error& error) {
        cout << error.what() << endl;
    }
}

// Just an interesting exercise. To be deleted...
// https://www.digitalocean.com/community/tutorials/reverse-string-c-plus-plus
string getFileType(const filesystem::directory_entry& dirString) {
    string newStr = dirString.path().string();
    cout << "Original: " << newStr << endl;
    reverse(newStr.begin(), newStr.end());

    string strCache;
    for (int i = 0; i < 9; i++) {
        if (newStr[i] == '.') {
            strCache = strCache + '.';
            break;
        }

        strCache = strCache + newStr[i];
    }
    newStr = strCache;
    reverse(newStr.begin(), newStr.end());
    cout << "ReREVERSE: " << newStr << endl;

    return newStr;
}

// Make a check for file size!! Ignore files bigger than 9 mb!!!
// https://en.cppreference.com/w/cpp/filesystem/file_size
bool isInString(const filesystem::directory_entry& dirString, const string& givenString, bool useFileNames) {
    string s_string; // Since tags read the whole filename and filetypes only getting the extension prevents errors
    useFileNames ? s_string = dirString.path().extension().string() : s_string = dirString.path().filename().string();

    // Check if the file size is larger than 9 mb
    bool b_inString = (dirString.file_size() <= 9000000 && s_string.find(givenString) < s_string.length()) ? /*if*/ b_inString = true : /*else*/ b_inString = false;

    return b_inString;
}