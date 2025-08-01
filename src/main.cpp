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
const char* saveFile = "values.txt";
bool debug = true, clearAll = true;

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
}

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

void eraseCLS(char selection) {
    if(clearAll)
    system("CLS"); //cmd clear
    cout << "> " << selection << endl;
}

void openFileEdit(ofstream& addFile, const char* file) {
    addFile.open(path + "/sorted/" + file);
    cout << "Opened file: " << file << endl;
}

void closeFileEdit(ofstream& addFile) {
    coutPrint("//////////////////////////////////");
    addFile.close();
}

void createDirectory(const char* folder) {
    if (!filesystem::exists(path + "/sorted/" + folder)) {
        filesystem::create_directories(path + "/sorted/" + folder);
        cout << folder << " created" << endl;
    }
    else {
        if (debug) {
            cout << "'" << folder << "'" << " folder already exists, no new folders created!" << endl;
            cout << endl;
        }
    }
}

bool doesFileExist(const char* file) {
    if (!filesystem::exists(path + "/sorted/" + file))
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

    addFile.open(path + "/sorted/" + saveFile);

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
    ifstream readFile(path + "/sorted/" + addedFile);

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
    openFileEdit(editFile, saveFile);
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
    if (!filesystem::exists(path + "/sorted/" + file)) {
        cout << string(file) + " doesn't exist, creating... " << endl;
        cout << endl;
        std::vector<const char*> createTypes = { ".png", ".jpg", ".webp", ".gif" };
        std::vector<const char*> createTags = { "Unreal", "codes" };

        ofstream addFile;

        openFileEdit(addFile, saveFile);
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
        if(debug)
            cout << "'" << file << "'" << " -save file already exists, using the existing one!" << endl; cout << endl;

        if (!readSaveNew(file))
            cout << file << " file reading failed" << endl;
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

std::vector<string> processFiles(std::vector<string> fileNames, const filesystem::directory_entry& entry, const string& filetype, bool useFileNames) {
    if (isInString(entry, filetype, useFileNames))
    {
        fileNames.push_back(entry.path().filename().string());
        moveFile(entry.path().filename().string(), filetype.c_str());
        cout << entry << endl;
    }
    return fileNames;
}

// Make the filetype variable into a vector and give in all the filetypes you want to include
std::vector<string> findAllFiles(const string& filetype) {
    int iter = 0;
    bool useFileNames = true;
    std::vector<string> fileNames;

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();

    for (const auto& entry : filesystem::directory_iterator(path)) {
        if ((filetype == ".png" && ((isInString(entry, ".png", useFileNames)) || (isInString(entry, ".PNG", useFileNames)))))
        {
            fileNames.push_back(entry.path().filename().string());
            moveFile(entry.path().filename().string(), filetype.c_str());
            cout << entry << endl;
        }
        else {
            fileNames = processFiles(fileNames, entry, filetype, useFileNames);
        }
        iter++;
    }

    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    if (fileNames.size() > 0 || debug) {
        std::cout << ms_int.count() << "ms" << " - " << ms_double.count() << "ms" << std::endl;
        std::cout << "Iterations: " << iter << std::endl;
        cout << "Amount of files: " << fileNames.size() << endl;
        coutPrint("//////////////////////////////////");
    }
    return fileNames;
}

std::vector<string> findAllTags(const string& tag) {
    int iter = 0;
    bool useFileNames = false;
    std::vector<string> fileNames;

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();

    for (const auto& entry : filesystem::directory_iterator(path)) {
        fileNames = processFiles(fileNames, entry, tag, useFileNames);
        iter++;
    }

    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
    if (fileNames.size() > 0 || debug) {
        std::cout << ms_int.count() << "ms" << " - " << ms_double.count() << "ms" << std::endl;
        cout << "Number of iterations: " << iter << endl;
        cout << "Amount of files: " << fileNames.size() << endl;
        coutPrint("//////////////////////////////////");
    }
    return fileNames;
}

bool loop(bool exit) {
    cout << "MENU:" << endl;
    cout << "--------" << endl;
    cout << "Do you want to orgazine all files?" << endl;
    cout << "'S': Singular" << endl;
    cout << "'A': All" << endl;
    cout << "'D': Show all desktop files" << endl;

    cout << endl;

    cout << "'W': Add an entry" << endl;
    cout << "'R': Remove an entry" << endl;
    cout << "'Q': Show all Filetypes/Tags" << endl;

    cout << endl;

    cout << "'T': Toggle additional prints: " << "(" << boolalpha << debug << ")" << endl;
    cout << "'G': Toggle clear history after command: " << "(" << boolalpha << clearAll << ")" << endl;

    cout << endl;

    cout << "'E': Exit" << endl;
    cout << "'C': Clean cmd" << endl;

    string input = "";

    char selection;
    cout << "> ";
    cin >> selection; // cin waits for user's input
    cout << endl;
    switch (selectOption(selection)) {
        //Added an enum to hande OR, since switch case can't handle ORs in their condition... odd
    case (Enum::allowedCommands::singular):
        eraseCLS(selection);

        coutPrint("What file would you like to target?");
        cout << "> ";
        cin >> input;
        cout << endl;

        if (!doesFileExist(saveFile))
            readWriteFile(saveFile);
        for (size_t i = 0; i < Enum::folderTypes.size(); i++) {
            createDirectory(Enum::folderTypes[i]);
        }
        for (size_t i = 0; i < Enum::folderTags.size(); i++) {
            createDirectory(Enum::folderTags[i]);
        }

        findAllTags(string(input));
        findAllFiles(string(input));

        break;

    case (Enum::allowedCommands::all):
        eraseCLS(selection);

        coutPrint("Are you sure you want to transfer all of the files? Y/N (.png, .jpg, .webp, .gif, .docx)");
        cout << "> ";
        cin >> selection;
        cout << endl;
        if ((selection == 'y') || (selection == 'Y')) {
            if (!doesFileExist(saveFile))
                readWriteFile(saveFile);

            // First go through all the tags, prioritized higher than the filetypes
            for (size_t i = 0; i < Enum::folderTags.size(); i++) {
                createDirectory(Enum::folderTags[i]);
                findAllTags(Enum::folderTags[i]);
            }                         

            for (size_t i = 0; i < Enum::folderTypes.size(); i++) {
                createDirectory(Enum::folderTypes[i]);
                findAllFiles(Enum::folderTypes[i]);
            }
            coutPrint("Searched all the supported file types!");
        }
        break;

    case (Enum::allowedCommands::add):
        eraseCLS(selection);

        cout << "Write a filename to ADD: ";
        std::cin >> input;
        cout << endl;
        if (!addFileType(input.c_str()))
            cout << "Addition FAILED" << endl;
        break;

    case (Enum::allowedCommands::remove):
        eraseCLS(selection);

        cout << "Write a filename to REMOVE: ";
        std::cin >> input;
        cout << endl;
        if (!removeFileType(input.c_str()))
            cout << "Filetype/Tag not found" << endl;
        break;

    case (Enum::allowedCommands::showtype):
        eraseCLS(selection);

        coutPrint("Showing all the available files...");
        cout << endl;
        cout << "All the filetypes and tags: " << endl;
        for (const auto& files : Enum::folderTypes) {
            cout << "TYPES: " << files << endl;
        }
        cout << endl;
        for (const auto& tags : Enum::folderTags) {
            cout << "TAGS: " << tags << endl;
        }
        cout << endl;
        break;

    case (Enum::allowedCommands::showfiles):
        eraseCLS(selection);

        coutPrint("Showing all the desktop files: ");
        for (const auto& entry : filesystem::directory_iterator(path)) {
            string conEntry = entry.path().filename().string();
            cout << "- " << conEntry << endl;
        }
        cout << endl;
        break;

    case (Enum::allowedCommands::debug):
        eraseCLS(selection);

        debug = !debug;
        cout << "Toggled additional info: " << boolalpha << debug << endl;
        cout << endl;
        break;

    case (Enum::allowedCommands::clearall):
        eraseCLS(selection);

        clearAll = !clearAll;
        cout << "Toggled clearing previous commands: " << boolalpha << clearAll << endl;
        cout << endl;
        break;

    case (Enum::allowedCommands::exit):
        exit = true;
        coutPrint("Exiting...");
        break;

    case (Enum::allowedCommands::clear):
        system("CLS"); //cmd clear
        break;

    default:
        eraseCLS(selection);
        coutPrint("What?! Command not recognized");
    }
    return exit;
}

void main() {
    bool exit = false;
    getWinDesktopPath();
    createDirectory("");
    readWriteFile(saveFile);

    while (!exit) {
        exit = loop(exit);
    }
}