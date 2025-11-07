#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <iomanip>

using namespace std;

// Color codes for better UI
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

class FileExplorer {
private:
    string currentPath;
    vector<string> fileList;
    
    // Helper function to get file permissions string
    string getPermissionsString(mode_t mode) {
        string perms = "";
        
        // File type
        if (S_ISDIR(mode)) perms += "d";
        else if (S_ISLNK(mode)) perms += "l";
        else perms += "-";
        
        // Owner permissions
        perms += (mode & S_IRUSR) ? "r" : "-";
        perms += (mode & S_IWUSR) ? "w" : "-";
        perms += (mode & S_IXUSR) ? "x" : "-";
        
        // Group permissions
        perms += (mode & S_IRGRP) ? "r" : "-";
        perms += (mode & S_IWGRP) ? "w" : "-";
        perms += (mode & S_IXGRP) ? "x" : "-";
        
        // Other permissions
        perms += (mode & S_IROTH) ? "r" : "-";
        perms += (mode & S_IWOTH) ? "w" : "-";
        perms += (mode & S_IXOTH) ? "x" : "-";
        
        return perms;
    }
    
    // Helper function to format file size
    string formatFileSize(off_t size) {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unitIndex = 0;
        double dSize = size;
        
        while (dSize >= 1024 && unitIndex < 4) {
            dSize /= 1024;
            unitIndex++;
        }
        
        char buffer[50];
        if (unitIndex == 0) {
            snprintf(buffer, sizeof(buffer), "%ld %s", size, units[unitIndex]);
        } else {
            snprintf(buffer, sizeof(buffer), "%.2f %s", dSize, units[unitIndex]);
        }
        return string(buffer);
    }
    
    // Helper function to get file modification time
    string getModificationTime(time_t mtime) {
        char buffer[100];
        struct tm* timeinfo = localtime(&mtime);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return string(buffer);
    }

public:
    FileExplorer() {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            currentPath = string(cwd);
        } else {
            currentPath = "/";
        }
    }
    
    // DAY 1: Basic file operations - List files in directory
    void listFiles(bool detailed = false) {
        fileList.clear();
        DIR* dir = opendir(currentPath.c_str());
        
        if (dir == NULL) {
            cout << RED << "Error: Cannot open directory!" << RESET << endl;
            return;
        }
        
        struct dirent* entry;
        vector<pair<string, bool>> entries; // pair<name, isDirectory>
        
        while ((entry = readdir(dir)) != NULL) {
            string filename = entry->d_name;
            string fullPath = currentPath + "/" + filename;
            struct stat fileStat;
            
            if (stat(fullPath.c_str(), &fileStat) == 0) {
                bool isDir = S_ISDIR(fileStat.st_mode);
                entries.push_back(make_pair(filename, isDir));
            }
        }
        closedir(dir);
        
        // Sort: directories first, then files
        sort(entries.begin(), entries.end(), [](const pair<string, bool>& a, const pair<string, bool>& b) {
            if (a.second != b.second) return a.second > b.second;
            return a.first < b.first;
        });
        
        cout << "\n" << BOLD << CYAN << "Current Directory: " << currentPath << RESET << "\n";
        cout << string(80, '=') << endl;
        
        if (detailed) {
            cout << left << setw(12) << "Permissions" << setw(10) << "Owner" 
                 << setw(10) << "Group" << setw(12) << "Size" 
                 << setw(20) << "Modified" << "Name" << endl;
            cout << string(80, '-') << endl;
        }
        
        for (const auto& entry : entries) {
            string filename = entry.first;
            string fullPath = currentPath + "/" + filename;
            struct stat fileStat;
            
            if (stat(fullPath.c_str(), &fileStat) == 0) {
                fileList.push_back(filename);
                
                if (detailed) {
                    // Get owner and group names
                    struct passwd* pw = getpwuid(fileStat.st_uid);
                    struct group* gr = getgrgid(fileStat.st_gid);
                    string owner = pw ? pw->pw_name : to_string(fileStat.st_uid);
                    string group = gr ? gr->gr_name : to_string(fileStat.st_gid);
                    
                    cout << left << setw(12) << getPermissionsString(fileStat.st_mode)
                         << setw(10) << owner
                         << setw(10) << group
                         << setw(12) << formatFileSize(fileStat.st_size)
                         << setw(20) << getModificationTime(fileStat.st_mtime);
                }
                
                if (S_ISDIR(fileStat.st_mode)) {
                    cout << BLUE << BOLD << filename << "/" << RESET << endl;
                } else if (fileStat.st_mode & S_IXUSR) {
                    cout << GREEN << filename << "*" << RESET << endl;
                } else {
                    cout << WHITE << filename << RESET << endl;
                }
            }
        }
        cout << "\nTotal items: " << fileList.size() << endl;
    }
    
    // DAY 2: Navigation features
    void changeDirectory(const string& path) {
        string newPath;
        
        if (path == "..") {
            // Go to parent directory
            size_t pos = currentPath.find_last_of('/');
            if (pos != string::npos && pos != 0) {
                newPath = currentPath.substr(0, pos);
            } else {
                newPath = "/";
            }
        } else if (path[0] == '/') {
            // Absolute path
            newPath = path;
        } else {
            // Relative path
            newPath = currentPath + "/" + path;
        }
        
        struct stat pathStat;
        if (stat(newPath.c_str(), &pathStat) == 0 && S_ISDIR(pathStat.st_mode)) {
            currentPath = newPath;
            if (chdir(currentPath.c_str()) == 0) {
                cout << GREEN << "Changed directory to: " << currentPath << RESET << endl;
            } else {
                cout << RED << "Error: Cannot access directory!" << RESET << endl;
            }
        } else {
            cout << RED << "Error: Directory does not exist!" << RESET << endl;
        }
    }
    
    string getCurrentPath() const {
        return currentPath;
    }
    
    // DAY 3: File manipulation - Create file
    void createFile(const string& filename) {
        string fullPath = currentPath + "/" + filename;
        ofstream file(fullPath);
        
        if (file.is_open()) {
            file.close();
            cout << GREEN << "File created successfully: " << filename << RESET << endl;
        } else {
            cout << RED << "Error: Cannot create file!" << RESET << endl;
        }
    }
    
    // DAY 3: Create directory
    void createDirectory(const string& dirname) {
        string fullPath = currentPath + "/" + dirname;
        
        if (mkdir(fullPath.c_str(), 0755) == 0) {
            cout << GREEN << "Directory created successfully: " << dirname << RESET << endl;
        } else {
            cout << RED << "Error: Cannot create directory!" << RESET << endl;
        }
    }
    
    // DAY 3: Delete file or directory
    void deleteItem(const string& name) {
        string fullPath = currentPath + "/" + name;
        struct stat pathStat;
        
        if (stat(fullPath.c_str(), &pathStat) != 0) {
            cout << RED << "Error: Item does not exist!" << RESET << endl;
            return;
        }
        
        if (S_ISDIR(pathStat.st_mode)) {
            // Try simple rmdir first (for empty directories)
            if (rmdir(fullPath.c_str()) == 0) {
                cout << GREEN << "Directory deleted successfully: " << name << RESET << endl;
            } else {
                // Directory is not empty, ask user
                cout << YELLOW << "Directory is not empty. Delete recursively? (yes/no): " << RESET;
                string confirm;
                getline(cin, confirm);
                
                if (confirm == "yes") {
                    if (deleteDirectoryRecursive(fullPath)) {
                        cout << GREEN << "Directory and all contents deleted successfully: " << name << RESET << endl;
                    } else {
                        cout << RED << "Error: Cannot delete directory!" << RESET << endl;
                    }
                } else {
                    cout << YELLOW << "Operation cancelled." << RESET << endl;
                }
            }
        } else {
            if (unlink(fullPath.c_str()) == 0) {
                cout << GREEN << "File deleted successfully: " << name << RESET << endl;
            } else {
                cout << RED << "Error: Cannot delete file!" << RESET << endl;
            }
        }
    }
    
    // Helper function to copy a single file
    bool copyFileInternal(const string& srcPath, const string& destPath) {
        ifstream src(srcPath, ios::binary);
        if (!src.is_open()) {
            return false;
        }
        
        ofstream dest(destPath, ios::binary);
        if (!dest.is_open()) {
            src.close();
            return false;
        }
        
        dest << src.rdbuf();
        src.close();
        dest.close();
        
        // Copy permissions from source to destination
        struct stat srcStat;
        if (stat(srcPath.c_str(), &srcStat) == 0) {
            chmod(destPath.c_str(), srcStat.st_mode);
        }
        
        return true;
    }
    
    // Helper function to recursively copy directory
    bool copyDirectoryRecursive(const string& srcPath, const string& destPath) {
        struct stat srcStat;
        if (stat(srcPath.c_str(), &srcStat) != 0) {
            return false;
        }
        
        // Create destination directory
        if (mkdir(destPath.c_str(), srcStat.st_mode) != 0) {
            return false;
        }
        
        DIR* dir = opendir(srcPath.c_str());
        if (dir == NULL) {
            return false;
        }
        
        struct dirent* entry;
        bool success = true;
        
        while ((entry = readdir(dir)) != NULL) {
            string filename = entry->d_name;
            
            // Skip . and ..
            if (filename == "." || filename == "..") continue;
            
            string srcFullPath = srcPath + "/" + filename;
            string destFullPath = destPath + "/" + filename;
            
            struct stat fileStat;
            if (stat(srcFullPath.c_str(), &fileStat) == 0) {
                if (S_ISDIR(fileStat.st_mode)) {
                    // Recursively copy subdirectory
                    if (!copyDirectoryRecursive(srcFullPath, destFullPath)) {
                        success = false;
                        break;
                    }
                } else {
                    // Copy file
                    if (!copyFileInternal(srcFullPath, destFullPath)) {
                        success = false;
                        break;
                    }
                }
            }
        }
        
        closedir(dir);
        return success;
    }
    
    // DAY 3: Copy file or directory
    void copyFile(const string& source, const string& destination) {
        string srcPath = currentPath + "/" + source;
        string destPath;
        
        if (destination[0] == '/') {
            destPath = destination;
        } else {
            destPath = currentPath + "/" + destination;
        }
        
        struct stat srcStat;
        if (stat(srcPath.c_str(), &srcStat) != 0) {
            cout << RED << "Error: Source does not exist!" << RESET << endl;
            return;
        }
        
        if (S_ISDIR(srcStat.st_mode)) {
            // Copy directory recursively
            cout << YELLOW << "Copying directory recursively..." << RESET << endl;
            if (copyDirectoryRecursive(srcPath, destPath)) {
                cout << GREEN << "Directory copied successfully from " << source << " to " << destination << RESET << endl;
            } else {
                cout << RED << "Error: Cannot copy directory!" << RESET << endl;
            }
        } else {
            // Copy single file
            if (copyFileInternal(srcPath, destPath)) {
                cout << GREEN << "File copied successfully from " << source << " to " << destination << RESET << endl;
            } else {
                cout << RED << "Error: Cannot copy file!" << RESET << endl;
            }
        }
    }
    
    // Helper function to recursively delete directory
    bool deleteDirectoryRecursive(const string& path) {
        DIR* dir = opendir(path.c_str());
        if (dir == NULL) {
            return false;
        }
        
        struct dirent* entry;
        bool success = true;
        
        while ((entry = readdir(dir)) != NULL) {
            string filename = entry->d_name;
            
            // Skip . and ..
            if (filename == "." || filename == "..") continue;
            
            string fullPath = path + "/" + filename;
            struct stat fileStat;
            
            if (stat(fullPath.c_str(), &fileStat) == 0) {
                if (S_ISDIR(fileStat.st_mode)) {
                    // Recursively delete subdirectory
                    if (!deleteDirectoryRecursive(fullPath)) {
                        success = false;
                        break;
                    }
                } else {
                    // Delete file
                    if (unlink(fullPath.c_str()) != 0) {
                        success = false;
                        break;
                    }
                }
            }
        }
        
        closedir(dir);
        
        // Finally, delete the directory itself
        if (success && rmdir(path.c_str()) != 0) {
            success = false;
        }
        
        return success;
    }
    
    // DAY 3: Move file or directory (to different location)
    void moveFile(const string& source, const string& destination) {
        string srcPath = currentPath + "/" + source;
        string destPath;
        
        // Destination must be absolute path or different directory
        if (destination[0] == '/') {
            destPath = destination;
        } else {
            destPath = currentPath + "/" + destination;
        }
        
        struct stat srcStat;
        if (stat(srcPath.c_str(), &srcStat) != 0) {
            cout << RED << "Error: Source does not exist!" << RESET << endl;
            return;
        }
        
        // Check if destination exists and is a directory
        struct stat destStat;
        if (stat(destPath.c_str(), &destStat) == 0) {
            if (S_ISDIR(destStat.st_mode)) {
                // Destination is a directory, move source INSIDE it
                string sourceName = source;
                size_t lastSlash = source.find_last_of('/');
                if (lastSlash != string::npos) {
                    sourceName = source.substr(lastSlash + 1);
                }
                destPath = destPath + "/" + sourceName;
                
                // Check if this new path already exists
                if (stat(destPath.c_str(), &destStat) == 0) {
                    cout << RED << "Error: '" << sourceName << "' already exists in destination directory!" << RESET << endl;
                    return;
                }
            } else {
                // Destination is a file
                cout << RED << "Error: Destination already exists as a file!" << RESET << endl;
                return;
            }
        }
        
        // Try simple rename first (works if same filesystem)
        if (rename(srcPath.c_str(), destPath.c_str()) == 0) {
            if (S_ISDIR(srcStat.st_mode)) {
                cout << GREEN << "Directory moved successfully to " << destPath << RESET << endl;
            } else {
                cout << GREEN << "File moved successfully to " << destPath << RESET << endl;
            }
        } else {
            // If rename fails (cross-filesystem), do copy + delete
            cout << YELLOW << "Cross-filesystem move detected, copying and deleting original..." << RESET << endl;
            
            if (S_ISDIR(srcStat.st_mode)) {
                // Copy directory then delete
                if (copyDirectoryRecursive(srcPath, destPath)) {
                    if (deleteDirectoryRecursive(srcPath)) {
                        cout << GREEN << "Directory moved successfully to " << destPath << RESET << endl;
                    } else {
                        cout << RED << "Error: Copied but could not delete source directory!" << RESET << endl;
                    }
                } else {
                    cout << RED << "Error: Cannot move directory!" << RESET << endl;
                }
            } else {
                // Copy file then delete
                if (copyFileInternal(srcPath, destPath)) {
                    if (unlink(srcPath.c_str()) == 0) {
                        cout << GREEN << "File moved successfully to " << destPath << RESET << endl;
                    } else {
                        cout << RED << "Error: Copied but could not delete source file!" << RESET << endl;
                    }
                } else {
                    cout << RED << "Error: Cannot move file!" << RESET << endl;
                }
            }
        }
    }
    
    // DAY 3: Rename file or directory (in same location)
    void renameItem(const string& oldName, const string& newName) {
        string oldPath = currentPath + "/" + oldName;
        string newPath = currentPath + "/" + newName;
        
        struct stat srcStat;
        if (stat(oldPath.c_str(), &srcStat) != 0) {
            cout << RED << "Error: Item does not exist!" << RESET << endl;
            return;
        }
        
        // Check if new name already exists
        struct stat destStat;
        if (stat(newPath.c_str(), &destStat) == 0) {
            cout << RED << "Error: An item with name '" << newName << "' already exists!" << RESET << endl;
            return;
        }
        
        if (rename(oldPath.c_str(), newPath.c_str()) == 0) {
            if (S_ISDIR(srcStat.st_mode)) {
                cout << GREEN << "Directory renamed from '" << oldName << "' to '" << newName << "'" << RESET << endl;
            } else {
                cout << GREEN << "File renamed from '" << oldName << "' to '" << newName << "'" << RESET << endl;
            }
        } else {
            cout << RED << "Error: Cannot rename item!" << RESET << endl;
        }
    }
    
    // DAY 4: Search functionality
    void searchFiles(const string& searchTerm, const string& searchPath = "") {
        string basePath = searchPath.empty() ? currentPath : searchPath;
        vector<string> results;
        searchRecursive(basePath, searchTerm, results);
        
        if (results.empty()) {
            cout << YELLOW << "No files found matching: " << searchTerm << RESET << endl;
        } else {
            cout << GREEN << "\nSearch results for '" << searchTerm << "':" << RESET << endl;
            cout << string(80, '-') << endl;
            for (const auto& result : results) {
                cout << result << endl;
            }
            cout << "\nTotal matches: " << results.size() << endl;
        }
    }
    
    void searchRecursive(const string& path, const string& searchTerm, vector<string>& results) {
        DIR* dir = opendir(path.c_str());
        if (dir == NULL) return;
        
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            string filename = entry->d_name;
            
            if (filename == "." || filename == "..") continue;
            
            string fullPath = path + "/" + filename;
            struct stat fileStat;
            
            if (stat(fullPath.c_str(), &fileStat) == 0) {
                // Check if filename contains search term (case-insensitive)
                string lowerFilename = filename;
                string lowerSearch = searchTerm;
                transform(lowerFilename.begin(), lowerFilename.end(), lowerFilename.begin(), ::tolower);
                transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
                
                if (lowerFilename.find(lowerSearch) != string::npos) {
                    if (S_ISDIR(fileStat.st_mode)) {
                        results.push_back(fullPath + "/");
                    } else {
                        results.push_back(fullPath);
                    }
                }
                
                // Recursively search subdirectories
                if (S_ISDIR(fileStat.st_mode)) {
                    searchRecursive(fullPath, searchTerm, results);
                }
            }
        }
        closedir(dir);
    }
    
    // DAY 5: File permission management
    void viewPermissions(const string& filename) {
        string fullPath = currentPath + "/" + filename;
        struct stat fileStat;
        
        if (stat(fullPath.c_str(), &fileStat) != 0) {
            cout << RED << "Error: File does not exist!" << RESET << endl;
            return;
        }
        
        cout << "\n" << BOLD << "File Permissions for: " << filename << RESET << endl;
        cout << string(50, '=') << endl;
        cout << "Permissions: " << getPermissionsString(fileStat.st_mode) << endl;
        cout << "Octal: " << oct << (fileStat.st_mode & 0777) << dec << endl;
        
        struct passwd* pw = getpwuid(fileStat.st_uid);
        struct group* gr = getgrgid(fileStat.st_gid);
        cout << "Owner: " << (pw ? pw->pw_name : to_string(fileStat.st_uid)) << endl;
        cout << "Group: " << (gr ? gr->gr_name : to_string(fileStat.st_gid)) << endl;
        cout << "Size: " << formatFileSize(fileStat.st_size) << endl;
        cout << "Last Modified: " << getModificationTime(fileStat.st_mtime) << endl;
    }
    
    void changePermissions(const string& filename, const string& permissions) {
        string fullPath = currentPath + "/" + filename;
        mode_t mode;
        
        // Convert octal string to mode_t
        try {
            mode = stoi(permissions, nullptr, 8);
        } catch (...) {
            cout << RED << "Error: Invalid permission format! Use octal notation (e.g., 755)" << RESET << endl;
            return;
        }
        
        if (chmod(fullPath.c_str(), mode) == 0) {
            cout << GREEN << "Permissions changed successfully for " << filename << RESET << endl;
        } else {
            cout << RED << "Error: Cannot change permissions!" << RESET << endl;
        }
    }
    
    void changeOwner(const string& filename, const string& owner, const string& group = "") {
        string fullPath = currentPath + "/" + filename;
        uid_t uid = -1;
        gid_t gid = -1;
        
        // Get UID from username
        if (!owner.empty()) {
            struct passwd* pw = getpwnam(owner.c_str());
            if (pw != NULL) {
                uid = pw->pw_uid;
            } else {
                cout << RED << "Error: Invalid owner username!" << RESET << endl;
                return;
            }
        }
        
        // Get GID from group name
        if (!group.empty()) {
            struct group* gr = getgrnam(group.c_str());
            if (gr != NULL) {
                gid = gr->gr_gid;
            } else {
                cout << RED << "Error: Invalid group name!" << RESET << endl;
                return;
            }
        }
        
        if (chown(fullPath.c_str(), uid, gid) == 0) {
            cout << GREEN << "Owner/Group changed successfully for " << filename << RESET << endl;
        } else {
            cout << RED << "Error: Cannot change owner/group! (May require root privileges)" << RESET << endl;
        }
    }
};

// Display menu
void displayMenu(const string& currentPath) {
    cout << "\n" << BOLD << CYAN << "╔════════════════════════════════════════════════════════╗" << RESET << endl;
    cout << BOLD << CYAN << "║          FILE EXPLORER - Linux C++ Application         ║" << RESET << endl;
    cout << BOLD << CYAN << "╚════════════════════════════════════════════════════════╝" << RESET << endl;
    cout << BOLD << MAGENTA << "  Current Directory: " << RESET << GREEN << currentPath << RESET << endl;
    cout << string(58, '=') << endl;
    
    cout << "\n" << BOLD << YELLOW << "📂 Navigation & Listing:" << RESET << endl;
    cout << "  " << CYAN << "1." << RESET << "  📋 List files (simple)" << endl;
    cout << "  " << CYAN << "2." << RESET << "  📊 List files (detailed)" << endl;
    cout << "  " << CYAN << "3." << RESET << "  🔄 Change directory" << endl;
    cout << "  " << CYAN << "4." << RESET << "  ⬆️  Go to parent directory" << endl;
    
    cout << "\n" << BOLD << YELLOW << "📁 File Operations:" << RESET << endl;
    cout << "  " << CYAN << "5." << RESET << "  ➕ Create file" << endl;
    cout << "  " << CYAN << "6." << RESET << "  📁 Create directory" << endl;
    cout << "  " << CYAN << "7." << RESET << "  🗑️  Delete file/directory" << endl;
    cout << "  " << CYAN << "8." << RESET << "  📄 Copy file/directory" << endl;
    cout << "  " << CYAN << "9." << RESET << "  📦 Move file/directory" << endl;
    cout << "  " << CYAN << "10." << RESET << " ✏️  Rename file/directory" << endl;
    
    cout << "\n" << BOLD << YELLOW << "🔍 Search:" << RESET << endl;
    cout << "  " << CYAN << "11." << RESET << " 🔎 Search files" << endl;
    
    cout << "\n" << BOLD << YELLOW << "🔐 Permissions Management:" << RESET << endl;
    cout << "  " << CYAN << "12." << RESET << " 👁️  View file permissions" << endl;
    cout << "  " << CYAN << "13." << RESET << " 🔧 Change permissions (chmod)" << endl;
    cout << "  " << CYAN << "14." << RESET << " 👤 Change owner/group (chown)" << endl;
    
    cout << "\n" << BOLD << YELLOW << "⚙️  Other:" << RESET << endl;
    cout << "  " << CYAN << "15." << RESET << " 📍 Display current path" << endl;
    cout << "  " << RED << "0." << RESET << "  ❌ Exit" << endl;
    
    cout << "\n" << string(58, '-') << endl;
}

int main() {
    FileExplorer explorer;
    int choice;
    string input1, input2, input3;
    
    // Attractive welcome screen
    cout << "\n" << string(60, '=') << endl;
    cout << BOLD << CYAN << "                    🚀 FILE EXPLORER 🚀                    " << RESET << endl;
    cout << BOLD << GREEN << "           Professional Linux File Management System       " << RESET << endl;
    cout << string(60, '=') << endl;
    cout << YELLOW << "           Developed in C++ | Linux Operating System       " << RESET << endl;
    cout << MAGENTA << "                   Capstone Project 2025                    " << RESET << endl;
    cout << string(60, '=') << endl;
    
    while (true) {
        displayMenu(explorer.getCurrentPath());
        cout << BOLD << YELLOW << "➤ Enter your choice: " << RESET;
        cin >> choice;
        cin.ignore(); // Clear newline from buffer
        
        switch (choice) {
            case 1:
                explorer.listFiles(false);
                break;
                
            case 2:
                explorer.listFiles(true);
                break;
                
            case 3:
                cout << "Enter directory path: ";
                getline(cin, input1);
                explorer.changeDirectory(input1);
                break;
                
            case 4:
                explorer.changeDirectory("..");
                break;
                
            case 5:
                cout << "Enter filename to create: ";
                getline(cin, input1);
                explorer.createFile(input1);
                break;
                
            case 6:
                cout << "Enter directory name to create: ";
                getline(cin, input1);
                explorer.createDirectory(input1);
                break;
                
            case 7:
                cout << "Enter file/directory name to delete: ";
                getline(cin, input1);
                cout << RED << "Are you sure? (yes/no): " << RESET;
                getline(cin, input2);
                if (input2 == "yes") {
                    explorer.deleteItem(input1);
                } else {
                    cout << "Operation cancelled." << endl;
                }
                break;
                
            case 8:
                cout << "Enter source file/directory name: ";
                getline(cin, input1);
                cout << "Enter destination path: ";
                getline(cin, input2);
                cout << YELLOW << "Copying " << input1 << " to " << input2 << "..." << RESET << endl;
                explorer.copyFile(input1, input2);
                break;
                
            case 9:
                cout << "Enter source file/directory name: ";
                getline(cin, input1);
                cout << "Enter destination path (e.g., /home/user/Documents/file.txt): ";
                getline(cin, input2);
                cout << YELLOW << "Moving " << input1 << " to " << input2 << "..." << RESET << endl;
                explorer.moveFile(input1, input2);
                break;
                
            case 10:
                cout << "Enter current name: ";
                getline(cin, input1);
                cout << "Enter new name: ";
                getline(cin, input2);
                explorer.renameItem(input1, input2);
                break;
                
            case 11:
                cout << "Enter search term: ";
                getline(cin, input1);
                explorer.searchFiles(input1);
                break;
                
            case 12:
                cout << "Enter filename: ";
                getline(cin, input1);
                explorer.viewPermissions(input1);
                break;
                
            case 13:
                cout << "Enter filename: ";
                getline(cin, input1);
                cout << "Enter permissions (octal, e.g., 755): ";
                getline(cin, input2);
                explorer.changePermissions(input1, input2);
                break;
                
            case 14:
                cout << "Enter filename: ";
                getline(cin, input1);
                cout << "Enter owner username: ";
                getline(cin, input2);
                cout << "Enter group name (or press Enter to skip): ";
                getline(cin, input3);
                explorer.changeOwner(input1, input2, input3);
                break;
                
            case 15:
                cout << CYAN << "Current path: " << explorer.getCurrentPath() << RESET << endl;
                break;
                
            case 0:
                cout << "\n" << string(60, '=') << endl;
                cout << BOLD << GREEN << "       ✨ Thank you for using File Explorer! ✨       " << RESET << endl;
                cout << CYAN << "              Have a great day ahead! 👋              " << RESET << endl;
                cout << string(60, '=') << endl;
                return 0;
                
            default:
                cout << RED << "❌ Invalid choice! Please select a valid option (0-15)." << RESET << endl;
        }
        
        cout << "\n" << BOLD << CYAN << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << endl;
        cout << YELLOW << "Press Enter to continue..." << RESET;
        cin.get();
    }
    
    return 0;
}
