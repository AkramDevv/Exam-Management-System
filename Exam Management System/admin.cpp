#include "admin.h"
#include "crypto.h"
#include <iostream>
#include "SQLite/sqlite3.h"

Admin::Admin(const std::string& username, const std::string& password)
	:username(username), password(password) {}

void Admin::setUsernamePas(const std::string& username, const std::string& password) {

	this->username = username;
	this->password = password;

}

bool Admin::checkAdminReg() {
	sqlite3* db;
	char* errMsg = nullptr;
	sqlite3_stmt* stmt;

	int rc = sqlite3_open("exam_system.db", &db);

	if (rc) {
		std::cerr << "Data base error: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	std::string createAdminTable = "CREATE TABLE IF NOT EXISTS admin_data ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"username TEXT NOT NULL, "
		"password TEXT NOT NULL);";

	rc = sqlite3_exec(db, createAdminTable.c_str(), nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK) {
		std::cerr << "Failed to create admin_data table: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}


	std::string sql = "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='admin_data';";
	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	bool adminExists = false;

	if (sqlite3_step(stmt) == SQLITE_ROW) {
		int count = sqlite3_column_int(stmt, 0);
		adminExists = (count > 0);
	}

	sql = "SELECT COUNT(*) FROM admin_data;";

	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	if (sqlite3_step(stmt) == SQLITE_ROW) {
		int count = sqlite3_column_int(stmt, 0);
		adminExists = (count > 0);
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return adminExists;

}

void Admin::registerAdmin() {

	sqlite3* db;
	char* errMsg = nullptr;
	int rc = sqlite3_open("exam_system.db", &db);

	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	std::string createSQL = "CREATE TABLE IF NOT EXISTS admin_data ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"username TEXT NOT NULL, "
		"password TEXT NOT NULL);";

	rc = sqlite3_exec(db, createSQL.c_str(), nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL create table error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		sqlite3_close(db);
		return;
	}



	std::string key = "A1B2C3D4E5F6G7H8I9J0K1L2M3N4O5P6";

	std::string encryptedUsername = encryptAES(username, key);
	std::string encryptedPassword = encryptAES(password, key);

	std::string insertSQL = "INSERT INTO admin_data (username, password) VALUES (?,?);";

	sqlite3_stmt* stmt;
	rc = sqlite3_prepare_v2(db, insertSQL.c_str(), -1, &stmt, nullptr);

	if (rc != SQLITE_OK) {
		std::cerr << "SQL insert error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	sqlite3_bind_text(stmt, 1, encryptedUsername.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, encryptedPassword.c_str(), -1, SQLITE_TRANSIENT);

	if (sqlite3_step(stmt) == SQLITE_DONE) {
		std::cout << "Registration successful!" << std::endl;
	}
	else {
		std::cerr << "Registartion failed! Error: " << sqlite3_errmsg(db) << std::endl;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

}

bool Admin::login() {

	sqlite3* db;
	sqlite3_stmt* stmt;

	int rc = sqlite3_open("exam_system.db", &db);
	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		return false;
	}

	std::string key = "A1B2C3D4E5F6G7H8I9J0K1L2M3N4O5P6";

	std::string encryptedUsername = encryptAES(username, key);
	std::string encryptedPassword = encryptAES(password, key);

	std::string checkSQL = "SELECT COUNT(*) FROM admin_data WHERE username = ? AND password = ?;";

	rc = sqlite3_prepare_v2(db, checkSQL.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "Check SQL error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return false;
	}

	sqlite3_bind_text(stmt, 1, encryptedUsername.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, encryptedPassword.c_str(), -1, SQLITE_STATIC);

	int count = 0;
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		count = sqlite3_column_int(stmt, 0);
	}
	else {
		std::cout << "Data checking failed!" << std::endl;
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return false;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	if (count == 1) {
		std::cout << "Login successfully!" << std::endl;
		return true;
	}
	else {
		std::cout << "Wrong username or password!" << std::endl;
		return false;
	}

}

void Admin::createUser() {

	sqlite3* db;
	char* errMsg = nullptr;

	int rc = sqlite3_open("exam_system.db", &db);

	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	std::string createSQL = "CREATE TABLE IF NOT EXISTS users_data ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"username TEXT NOT NULL, "
		"password TEXT NOT NULL);";

	rc = sqlite3_exec(db, createSQL.c_str(), nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL create table error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		sqlite3_close(db);
		return;
	}

	std::cin.ignore();
	std::string username, password;
	std::string key = "A1B2C3D4E5F6G7H8I9J0K1L2M3N4O5P6";

	while (true) {
		std::cout << "Enter username: ";
		std::getline(std::cin, username);

		std::string checkSQL = "SELECT COUNT(*) FROM users_data WHERE username = ?;";

		sqlite3_stmt* checkStmt;

		rc = sqlite3_prepare_v2(db, checkSQL.c_str(), -1, &checkStmt, nullptr);
		if (rc != SQLITE_OK) {
			std::cerr << "Check SQL error: " << sqlite3_errmsg(db) << std::endl;
			sqlite3_close(db);
			return;
		}

		std::string checkEncryptedUsername = encryptAES(username, key);

		sqlite3_bind_text(checkStmt, 1, checkEncryptedUsername.c_str(), -1, SQLITE_STATIC);

		int count = 0;
		if (sqlite3_step(checkStmt) == SQLITE_ROW) {
			count = sqlite3_column_int(checkStmt, 0);
		}
		sqlite3_finalize(checkStmt);

		if (count == 0) {
			break;
		}
		else {
			std::cout << "This username is already taken! Please choose another one.\n";
		}

	}

	std::cout << "Enter new user's password: ";
	std::getline(std::cin, password);


	std::string encryptedUsername = encryptAES(username, key);
	std::string encryptedPassword = encryptAES(password, key);

	std::string insertSQL = "INSERT INTO users_data (username, password) VALUES (?,?);";
	sqlite3_stmt* stmt;

	rc = sqlite3_prepare_v2(db, insertSQL.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	sqlite3_bind_text(stmt, 1, encryptedUsername.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, encryptedPassword.c_str(), -1, SQLITE_TRANSIENT);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		std::cerr << "SQL execution error: " << sqlite3_errmsg(db) << std::endl;
	}
	else {
		std::cout << "New user added successfully!" << std::endl;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

}

void Admin::deleteUser() {

	showUsers();

	sqlite3* db;
	char* errMsg = nullptr;
	int rc = sqlite3_open("exam_system.db", &db);

	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	int userId;
	std::cout << "Enter user ID to delete: ";
	std::cin >> userId;

	std::string deleteSQL = "DELETE FROM users_data WHERE id = ?;";
	sqlite3_stmt* stmt;

	rc = sqlite3_prepare_v2(db, deleteSQL.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	sqlite3_bind_int(stmt, 1, userId);

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_DONE) {
		std::cout << "User deleted successfully!" << std::endl;
	}
	else {
		std::cerr << "Error deleting user: " << sqlite3_errmsg(db) << std::endl;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

}

void Admin::showPendingUsers() {

	sqlite3* db;
	sqlite3_stmt* stmt;

	int rc = sqlite3_open("exam_system.db", &db);
	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	std::string sql = "SELECT * FROM pending_users;";

	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	std::string key = "A1B2C3D4E5F6G7H8I9J0K1L2M3N4O5P6";
	bool hasUsers = false;

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		hasUsers = true;

		int id = sqlite3_column_int(stmt, 0);
		std::string encryptedUsername = (const char*)sqlite3_column_text(stmt, 1);
		std::string encryptedPassword = (const char*)sqlite3_column_text(stmt, 2);

		std::cout << "ID: " << id << " | Username: " << decryptAES(encryptedUsername, key) << " | Password: " << decryptAES(encryptedPassword, key) << std::endl;

	}

	if (!hasUsers) {
		std::cout << "No pending users found!" << std::endl;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

}

void Admin::approveOrRejectUsers() {

	sqlite3* db;
	int rc = sqlite3_open("exam_system.db", &db);
	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	while (true) {
		showPendingUsers();
		std::cout << "_______________________________________________" << std::endl;
		char choice;
		std::cout << "A(Approve user) | R(Reject user) | E(Exit)\nEnter choice: ";
		std::cin >> choice;
		if (choice == 'e' || choice == 'E') {
			break;
		}

		int id;
		std::cout << "Enter user's ID for approve: ";
		std::cin >> id;

		if (choice == 'a' || choice == 'A') {

			std::string insertSQL = "INSERT INTO users_data (username, password) SELECT username, password FROM pending_users WHERE id = ?;";
			std::string deleteSQL = "DELETE FROM pending_users WHERE id = ?;";

			sqlite3_stmt* insertStmt = nullptr;
			sqlite3_stmt* deleteStmt = nullptr;

			rc = sqlite3_prepare_v2(db, insertSQL.c_str(), -1, &insertStmt, nullptr);
			if (rc != SQLITE_OK) {
				std::cerr << "Insert SQL error: " << sqlite3_errmsg(db) << std::endl;
				break;
			}
			rc = sqlite3_prepare_v2(db, deleteSQL.c_str(), -1, &deleteStmt, nullptr);
			if (rc != SQLITE_OK) {
				std::cerr << "Delete SQL error: " << sqlite3_errmsg(db) << std::endl;
				sqlite3_finalize(insertStmt);
				break;
			}

			sqlite3_bind_int(insertStmt, 1, id);
			if (sqlite3_step(insertStmt) == SQLITE_DONE) {
				std::cout << "User approved successfully!" << std::endl;
			}
			else {
				std::cout << "Unsuccessfull approve!" << std::endl;
			}

			sqlite3_bind_int(deleteStmt, 1, id);
			if (sqlite3_step(deleteStmt) == SQLITE_DONE) {
				std::cout << "User deleted from pending list successfully!" << std::endl;
			}
			else {
				std::cout << "Unsuccessfull user delete from pending list!" << std::endl;
			}

			sqlite3_finalize(insertStmt);
			sqlite3_finalize(deleteStmt);

		}
		else if (choice == 'r' || choice == 'R') {

			std::string deleteSQL = "DELETE FROM pending_users WHERE id = ?;";
			sqlite3_stmt* deleteStmt = nullptr;

			rc = sqlite3_prepare_v2(db, deleteSQL.c_str(), -1, &deleteStmt, nullptr);
			if (rc != SQLITE_OK) {
				std::cerr << "Delete SQL error: " << sqlite3_errmsg(db) << std::endl;
				break;
			}

			sqlite3_bind_int(deleteStmt, 1, id);
			if (sqlite3_step(deleteStmt) == SQLITE_DONE) {
				std::cout << "User deleted from pending list successfully!" << std::endl;
			}
			else {
				std::cout << "Unsuccessfull user delete from pending list!" << std::endl;
			}

			sqlite3_finalize(deleteStmt);

		}
	}

	sqlite3_close(db);

}

void Admin::showUsers() {

	sqlite3* db;

	int rc = sqlite3_open("exam_system.db", &db);
	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	std::string selectSQL = "SELECT * FROM users_data;";

	sqlite3_stmt* stmt;
	rc = sqlite3_prepare_v2(db, selectSQL.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL prepare error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	std::string key = "A1B2C3D4E5F6G7H8I9J0K1L2M3N4O5P6";
	bool hasUsers = false;

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		hasUsers = true;
		int id = sqlite3_column_int(stmt, 0);
		std::string encryptedUsername = (const char*)sqlite3_column_text(stmt, 1);
		std::string encryptedPassword = (const char*)sqlite3_column_text(stmt, 2);

		std::cout << "ID: " << id << " | Username: " << decryptAES(encryptedUsername, key) << " | Password: " << decryptAES(encryptedPassword, key) << std::endl;
	}

	if (!hasUsers) {
		std::cout << "No users foun in the database." << std::endl;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

}

void Admin::showAllUsersResults() {

	sqlite3* db;
	sqlite3_stmt* stmt;

	int rc = sqlite3_open("exam_system.db", &db);
	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	std::string showResultsSQL = "SELECT * FROM users_results;";

	rc = sqlite3_prepare_v2(db, showResultsSQL.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "Show results SQL error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	bool hasResults = false;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		hasResults = true;
		int id = sqlite3_column_int(stmt, 0);
		std::string userName = (sqlite3_column_text(stmt, 1)) ? (const char*)sqlite3_column_text(stmt, 1) : "NULL";
		std::string examName = (sqlite3_column_text(stmt, 2)) ? (const char*)sqlite3_column_text(stmt, 2) : "NULL";
		int result = sqlite3_column_int(stmt, 3);

		std::cout <<id<<". " << "Username: " << userName << " | " << "Exam: " << examName << " | " << "Score: " << result << "%" << std::endl;
	}

	if (!hasResults) {
		std::cout << "No exam results found!" << std::endl;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

}

void Admin::showUsersRating() {

	sqlite3* db;
	sqlite3_stmt* stmt;

	int rc = sqlite3_open("exam_system.db", &db);
	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	std::string showResultsSQL = "SELECT username, took_exam_name, result FROM users_results ORDER BY result DESC;";

	rc = sqlite3_prepare_v2(db, showResultsSQL.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "Show results SQL error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	std::cout << "Users Rating (Highest to Lowest Score):\n";
	std::cout << "--------------------------------------------\n";

	bool hasResults = false;
	int rank = 1;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		hasResults = true;
		std::string userName = (sqlite3_column_text(stmt, 0)) ? (const char*)sqlite3_column_text(stmt, 0) : "NULL";
		std::string examName = (sqlite3_column_text(stmt, 1)) ? (const char*)sqlite3_column_text(stmt, 1) : "NULL";
		int result = sqlite3_column_int(stmt, 2);

		std::cout << rank << ". " << "Username: " << userName << " | " << "Exam: " << examName << " | " << "Score: " << result << "%" << std::endl;
		rank++;
	}

	if (!hasResults) {
		std::cout << "No exam results found!" << std::endl;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

}

void Admin::addExam() {

	sqlite3* db;
	char* errMsg = nullptr;
	int rc = sqlite3_open("exam_system.db", &db);

	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	std::string createSQL = "CREATE TABLE IF NOT EXISTS exams ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"exam_name TEXT NOT NULL, "
		"question TEXT NOT NULL, "
		"options TEXT NOT NULL, "
		"correct_option TEXT NOT NULL);";

	rc = sqlite3_exec(db, createSQL.c_str(), nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK) {
		std::cerr << "Create SQL error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
		sqlite3_close(db);
		return;
	}

	std::cin.ignore();
	std::string exam_name;
	int questionCount{};
	std::cout << "Enter exam name: ";
	std::getline(std::cin, exam_name);

	std::cout << "Enter number of questions: ";
	std::cin >> questionCount;
	std::cin.ignore();

	int questionCounter = 1;
	while (questionCount--) {

		std::string question, options, correct_option;
		std::cout << "Enter " << questionCounter << ". question: ";
		std::getline(std::cin, question);
		question = "Q" + std::to_string(questionCounter) + ". " + question;
		std::cout << "Enter options (comma separated): ";
		std::getline(std::cin, options);
		std::cout << "Enter correct option in capital letters (A,B...): ";
		std::getline(std::cin, correct_option);

		std::string questionSQL = "INSERT INTO exams (exam_name, question, options, correct_option) VALUES (?,?,?,?);";
		sqlite3_stmt* stmt = nullptr;

		rc = sqlite3_prepare_v2(db, questionSQL.c_str(), -1, &stmt, nullptr);
		if (rc != SQLITE_OK) {
			std::cerr << "Insert SQL error: " << sqlite3_errmsg(db) << std::endl;
			break;
		}

		sqlite3_bind_text(stmt, 1, exam_name.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt, 2, question.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt, 3, options.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt, 4, correct_option.c_str(), -1, SQLITE_TRANSIENT);

		if (sqlite3_step(stmt) == SQLITE_DONE) {
			std::cout << "Question added successfully!" << std::endl;
		}
		else {
			std::cout << "Failed to add question!" << std::endl;
		}

		questionCounter++;
		sqlite3_finalize(stmt);

	}

	sqlite3_close(db);

}

void Admin::deleteExam() {

	sqlite3* db;
	int rc = sqlite3_open("exam_system.db", &db);
	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	while (true) {
		showExams();
		std::cout << "_________________________________________________" << std::endl;
		char choice;
		std::cout << "Do you want to delete any exam - Y | N: " << std::endl;
		std::cin >> choice;
		if (choice == 'n' || choice == 'N') {
			break;
		}
		else if (choice == 'y' || choice == 'Y') {
			std::cin.ignore();
			std::string examNameDel;
			std::cout << "Enter name of exam for delete: ";
			std::getline(std::cin, examNameDel);

			std::string deleteSQL = "DELETE FROM exams WHERE exam_name = ?;";
			sqlite3_stmt* stmt;

			rc = sqlite3_prepare_v2(db, deleteSQL.c_str(), -1, &stmt, nullptr);
			if (rc != SQLITE_OK) {
				std::cerr << "Delete SQL error: " << sqlite3_errmsg(db) << std::endl;
				break;
			}

			sqlite3_bind_text(stmt, 1, examNameDel.c_str(), -1, SQLITE_TRANSIENT);
			if (sqlite3_step(stmt) == SQLITE_DONE) {
				std::cout << "Exam deleted successfully!" << std::endl;
			}
			else {
				std::cout << "Unsuccessfull exam delete!" << std::endl;
			}

			sqlite3_finalize(stmt);

		}

	}

	sqlite3_close(db);

}

void Admin::showExams() {

	sqlite3* db;
	sqlite3_stmt* stmt;
	int rc = sqlite3_open("exam_system.db", &db);

	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	std::string sql = "SELECT DISTINCT exam_name FROM exams;";
	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "Prepare SQL error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	bool hasExam = false;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		hasExam = true;
		std::string examName = (sqlite3_column_text(stmt, 0)) ? (const char*)sqlite3_column_text(stmt, 0) : "NULL";
		std::cout << examName << std::endl;
	}

	if (!hasExam) {
		std::cout << "No exam found!" << std::endl;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

}
