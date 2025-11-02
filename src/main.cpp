#include "utils.h"

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

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

    for (const auto& entry : filesystem::directory_iterator(Enum::path)) {
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

    if (fileNames.size() > 0 || Enum::debug) {
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

    for (const auto& entry : filesystem::directory_iterator(Enum::path)) {
        fileNames = processFiles(fileNames, entry, tag, useFileNames);
        iter++;
    }

    auto t2 = high_resolution_clock::now();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;
    if (fileNames.size() > 0 || Enum::debug) {
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

    cout << "'T': Toggle additional prints: " << "(" << boolalpha << Enum::debug << ")" << endl;
    cout << "'G': Toggle clear history after command: " << "(" << boolalpha << Enum::clearAll << ")" << endl;

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

        if (!doesFileExist(Enum::saveFile))
            readWriteFile(Enum::saveFile);

        createDirectory(input.c_str()); // Create directory for selected files
        //for (size_t i = 0; i < Enum::folderTypes.size(); i++) {
        //    createDirectory(Enum::folderTypes[i]);
        //}
        //for (size_t i = 0; i < Enum::folderTags.size(); i++) {
        //    createDirectory(Enum::folderTags[i]);
        //}

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
            if (!doesFileExist(Enum::saveFile))
                readWriteFile(Enum::saveFile);

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
        for (const auto& entry : filesystem::directory_iterator(Enum::path)) {
            string conEntry = entry.path().filename().string();
            cout << "- " << conEntry << endl;
        }
        cout << endl;
        break;

    case (Enum::allowedCommands::debug):
        eraseCLS(selection);

        Enum::debug = !Enum::debug;
        cout << "Toggled additional info: " << boolalpha << Enum::debug << endl;
        cout << endl;
        break;

    case (Enum::allowedCommands::clearall):
        eraseCLS(selection);

        Enum::clearAll = !Enum::clearAll;
        cout << "Toggled clearing previous commands: " << boolalpha << Enum::clearAll << endl;
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
    readWriteFile(Enum::saveFile);

    while (!exit) {
        exit = loop(exit);
    }
}