
# 🧮 Linux File Explorer Application

A comprehensive **console-based File Explorer** written in **C++** for **Linux operating systems**.  
This project provides a complete file management system with navigation, file manipulation, search, and permission management capabilities — all in a terminal interface.

---

## 📋 Project Overview

**Assignment:** Capstone Project – File Explorer Application  
**Language:** C++  
**Platform:** Linux OS (Ubuntu / Debian / Fedora / WSL)  
**Development Duration:** 5 Days  

---

## 🎯 Features Overview

### 🗓️ Day 1: Basic Operations
- ✅ List files in the current directory (simple and detailed view)
- ✅ Display file permissions, size, owner, group, and modification time
- ✅ Color-coded file display (directories, executables, and regular files)

### 🗓️ Day 2: Navigation
- ✅ Change directories (absolute and relative paths)
- ✅ Navigate to parent directory
- ✅ Display current working directory
- ✅ Real-time directory tracking and updates

### 🗓️ Day 3: File Manipulation
- ✅ Create files and directories
- ✅ Delete files and directories (with recursive option)
- ✅ Copy files and directories (recursive support)
- ✅ Move files or directories (cross-filesystem supported)
- ✅ Rename files or directories

### 🗓️ Day 4: Search Functionality
- ✅ Recursive search through directories
- ✅ Case-insensitive matching
- ✅ Display full file paths in search results

### 🗓️ Day 5: Permission Management
- ✅ View file permissions (symbolic and octal)
- ✅ Change permissions (`chmod`)
- ✅ Change ownership and group (`chown`)
- ✅ Display owner, group, and size information

---

## 🛠️ Prerequisites

- Linux environment (Ubuntu, Debian, Fedora, or WSL)
- Installed `g++` compiler
- Installed `make` utility
- Root/sudo access (for ownership/permission changes)

---

## ⚙️ Installation & Setup

### 1️⃣ Clone or Download the Repository
```bash
git clone https://github.com/<your-username>/file-explorer-cpp.git
cd file-explorer-cpp
````

### 2️⃣ Compile the Program

**Using Makefile:**

```bash
make
```

**Manual Compilation:**

```bash
g++ -Wall -Wextra -std=c++17 -O2 -o file_explorer file_explorer.cpp
```

### 3️⃣ Run the Application

```bash
./file_explorer
```

### 4️⃣ Optional: Install Globally

```bash
sudo cp file_explorer /usr/local/bin/
file_explorer
```

---

## 🧭 Application Menu

```
==================== FILE EXPLORER MENU ====================
Current Directory: /home/user
------------------------------------------------------------
1.  List files (simple)
2.  List files (detailed)
3.  Change directory
4.  Go to parent directory
5.  Create file
6.  Create directory
7.  Delete file/directory
8.  Copy file
9.  Move file
10. Rename file/directory
11. Search files
12. View file permissions
13. Change permissions (chmod)
14. Change owner/group (chown)
15. Display current path
0.  Exit
------------------------------------------------------------
```

---

## 🧩 Example Usage

### 1. List Files

```bash
Choose an option: 1
Documents/
Projects/
readme.txt
```

### 2. Detailed File Listing

```bash
Choose an option: 2
Permissions  Owner   Group   Size     Modified             Name
-rw-r--r--   user    user    2.0 KB   2025-11-09 10:15:21 readme.txt
drwxr-xr-x   user    user    4.0 KB   2025-11-09 10:00:10 Projects/
```

### 3. Change Directory

```bash
Choose an option: 3
Enter directory: Projects
Changed directory to: /home/user/Projects
```

### 5. Create File

```bash
Choose an option: 5
Enter filename: new.txt
File created successfully.
```

### 6. Create Directory

```bash
Choose an option: 6
Enter directory name: new_folder
Directory created successfully.
```

### 7. Delete File/Directory

```bash
Choose an option: 7
Enter file/directory: old.txt
File deleted successfully.
```

### 8. Copy File

```bash
Choose an option: 8
Enter source: new.txt
Enter destination: copy.txt
File copied successfully.
```

### 9. Move File

```bash
Choose an option: 9
Enter source: copy.txt
Enter destination: moved.txt
File moved successfully.
```

### 10. Rename File

```bash
Choose an option: 10
Enter current name: moved.txt
Enter new name: renamed.txt
Item renamed successfully.
```

### 11. Search Files

```bash
Choose an option: 11
Enter search term: read
Search results:
./readme.txt
./Documents/read.txt
```

### 12. View File Permissions

```bash
Choose an option: 12
Enter filename: renamed.txt
Permissions: -rw-r--r--
Owner: user
Group: user
Size: 1.2 KB
Last Modified: 2025-11-09 10:30:11
```

### 13. Change File Permissions

```bash
Choose an option: 13
Enter filename: renamed.txt
Enter permission (octal, e.g., 755): 644
Permissions changed successfully.
```

### 14. Change Owner/Group

```bash
Choose an option: 14
Enter filename: renamed.txt
Enter owner: newuser
Enter group: newgroup
Owner/Group changed successfully.
```

### 15. Display Current Path

```bash
Choose an option: 15
Current path: /home/user/Documents
```

### 0. Exit

```bash
Choose an option: 0
Exiting File Explorer. Goodbye!
```

---

## 🎨 UI Highlights

* **Color-coded output**

  * Blue → Directories
  * Green → Executable files
  * White → Regular files
  * Red → Errors
  * Yellow → Warnings / Menu headers
  * Cyan → Information / paths
* **Real-time directory updates**
* **Bold, clean text layout**
* **Interactive prompts and confirmations**

---

## 🧠 Technical Details

### Core System Calls

* `opendir()`, `readdir()`, `closedir()` – Directory operations
* `stat()` – Retrieve file metadata
* `mkdir()`, `rmdir()`, `unlink()` – File/directory creation/deletion
* `rename()` – Rename/move files
* `chmod()` – Modify permissions
* `chown()` – Change file ownership
* `getcwd()`, `chdir()` – Directory management

### Permission Representation

| Format   | Example      |
| -------- | ------------ |
| Symbolic | `drwxr-xr-x` |
| Octal    | `755`        |

---

## 📂 Folder Structure

```
file-explorer-cpp/
 ├── file_explorer.cpp    # Main source file
 ├── Makefile             # Build automation file
 └── README.md            # Project documentation
```

---

## 🧱 Build Options

**Debug Mode**

```bash
g++ -Wall -Wextra -std=c++17 -g -o file_explorer_debug file_explorer.cpp
```

**Optimized Release**

```bash
g++ -Wall -Wextra -std=c++17 -O3 -o file_explorer file_explorer.cpp
```

---

## 📚 Learning Outcomes

This project demonstrates:

1. **Linux system programming** using C++
2. **File and directory management**
3. **Permission and ownership control**
4. **Terminal UI design**
5. **Recursive algorithms** for search and copy
6. **Robust error handling** and safe user operations

---

## 🔮 Future Enhancements

* Recursive directory deletion
* File content previewer
* Archive support (ZIP/TAR)
* Sorting and filtering of files
* Command history and bookmarks
* File comparison and diff utilities

---

## 👨‍💻 Developer Information

**Developer:** Subham Nayak
**Role:** C++ Developer / Linux Enthusiast

---

## 📄 License

This project is licensed under the **MIT License**.
You may freely use, modify, and distribute it for educational or personal purposes.

---

## 🎓 Conclusion

The **Linux File Explorer Application** successfully implements essential file management features in C++.
It demonstrates complete mastery of system calls, file handling, and terminal UI — offering a powerful and intuitive Linux command-line tool.

**Happy Exploring! 🚀**

```


