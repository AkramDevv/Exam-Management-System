#include "admin.h"
#include "user.h"
#include "SQLite/sqlite3.h"
#include <iostream>

void adminPanel(Admin& admin);
void userPanel(User& user);

void enableWALMode() {
    sqlite3* db;
    char* errMsg = nullptr;

    int rc = sqlite3_open("exam_system.db", &db);
    if (rc) {
        std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::string walSQL = "PRAGMA journal_mode=WAL;";
    rc = sqlite3_exec(db, walSQL.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "Failed to enable WAL mode: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    else {
        std::cout << "WAL mode enabled successfully!" << std::endl;
    }

    sqlite3_close(db);
}


int main() {
    enableWALMode();

    char enterChoice;
    std::cout << "Who are you? - 1. Admin | 2. User: ";
    std::cin >> enterChoice;

    if (enterChoice == '1') {
        Admin admin("", "");
        if (admin.checkAdminReg()) {
            std::string username, password;
            std::cout << "Admin Login\nUsername: ";
            std::cin >> username;
            std::cout << "Password: ";
            std::cin >> password;

            admin.setUsernamePas(username, password);
            if (!admin.login()) {
                std::cout << "Login failed!\n";
                return 0;
            }
        }
        else {
            std::cout << "No admin found! Please register first.\n";
            std::string username, password;
            std::cout << "Register Admin\nUsername: ";
            std::cin >> username;
            std::cout << "Password: ";
            std::cin >> password;

            admin.setUsernamePas(username, password);
            admin.registerAdmin();
        }
        adminPanel(admin);
    }

    else if (enterChoice == '2') {
        char userChoice;
        std::cout << "1. Login | 2. Register: ";
        std::cin >> userChoice;

        if (userChoice == '1') {
            std::string username, password;
            std::cout << "User Login\nUsername: ";
            std::cin >> username;
            std::cout << "Password: ";
            std::cin >> password;

            User user(username, password);
            if (!user.login()) {
                std::cout << "Login failed!\n";
                return 0;
            }
            userPanel(user);
        }
        else if (userChoice == '2') {
            User user("", "");
            user.registerUser();
            return 0;
        }
        else {
            std::cout << "Invalid choice! Exiting...\n";
        }
    }

    else {
        std::cout << "Invalid choice! Exiting...\n";
    }

    return 0;
}



void adminPanel(Admin& admin) {
    while (true) {
        std::cout << "\n Admin Panel:\n";
        std::cout << "1. Add Exam\n";
        std::cout << "2. Delete Exam\n";
        std::cout << "3. Show All Exams\n";
        std::cout << "4. Add User\n";
        std::cout << "5. Delete User\n";
        std::cout << "6. Show Users\n";
        std::cout << "7. Approve or Reject Pending Users\n";
        std::cout << "8. Show Users Results\n";
        std::cout << "9. Show Users Rating\n";
        std::cout << "0. Logout\n";
        std::cout << "Enter choice: ";

        char choice;
        std::cin >> choice;

        switch (choice) {
        case '1': admin.addExam(); break;
        case '2': admin.deleteExam(); break;
        case '3': admin.showExams(); break;
        case '4': admin.createUser(); break;
        case '5': admin.deleteUser(); break;
        case '6': admin.showUsers(); break;
        case '7': admin.approveOrRejectUsers(); break;
        case '8': admin.showAllUsersResults(); break;
        case '9': admin.showUsersRating(); break;
        case '0': return;
        default: std::cout << "Invalid choice! Try again.\n";
        }
    }
}

void userPanel(User& user) {
    while (true) {
        std::cout << "\n User Panel:\n";
        std::cout << "1. Take Exam\n";
        std::cout << "2. Show My Results\n";
        std::cout << "3. Show All Users Rating\n";
        std::cout << "0. Logout\n";
        std::cout << "Enter choice: ";

        char choice;
        std::cin >> choice;

        switch (choice) {
        case '1': user.takeExam(); break;
        case '2': user.showResults(); break;
        case '3': user.showUsersRating(); break;
        case '0': return;
        default: std::cout << "Invalid choice! Try again.\n";
        }
    }
}


