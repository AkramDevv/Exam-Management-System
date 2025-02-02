#include "user.h"
#include "crypto.h"
#include <iostream>
#include <limits>
#include "SQLite/sqlite3.h"

User::User(const std::string& username, const std::string& password)
	:username(username), password(password) {}

void User::setUsernamePas(const std::string& username, const std::string& password) {

	this->username = username;
	this->password = password;

}

void User::registerUser() {

	sqlite3* db;
	char* errMsg = nullptr;
	int rc = sqlite3_open("exam_system.db", &db);

	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	std::string createSQL = "CREATE TABLE IF NOT EXISTS pending_users ("
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

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::string username, password;

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

	std::cout << "Enter password: ";
	std::getline(std::cin, password);

	std::string encryptedUsername = encryptAES(username, key);
	std::string encryptedPassword = encryptAES(password, key);

	std::string insertSQL = "INSERT INTO pending_users (username, password) VALUES (?,?);";

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
		std::cout << "Registration successful! Please wait for admin approval." << std::endl;
	}
	else {
		std::cout << "Registartion failed!" << std::endl;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

}

bool User::login() {

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

	std::string checkSQL = "SELECT COUNT(*) FROM users_data WHERE username = ? AND password = ?;";

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

void User::takeExam() {

	sqlite3* db;
	int rc = sqlite3_open("exam_system.db", &db);
	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		return;
	}

	sqlite3_stmt* showExamsStmt;
	std::string showExamsSQL = "SELECT DISTINCT exam_name FROM exams;";
	rc = sqlite3_prepare_v2(db, showExamsSQL.c_str(), -1, &showExamsStmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "Show exam SQL error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	bool hasExam = false;
	while (sqlite3_step(showExamsStmt) == SQLITE_ROW) {
		hasExam = true;
		std::string examName = (sqlite3_column_text(showExamsStmt, 0)) ? (const char*)sqlite3_column_text(showExamsStmt, 0) : "NULL";
		std::cout << "- " << examName << std::endl;
	}

	sqlite3_finalize(showExamsStmt);

	if (!hasExam) {
		std::cout << "No exam found!" << std::endl;
		sqlite3_close(db);
		return;
	}

	char choice;
	std::cout << "Do you want to take the exam? - Y | N: ";
	std::cin >> choice;
	std::string chosenExamName;
	if (choice == 'n' || choice == 'N') {
		sqlite3_close(db);
		return;
	}
	else if (choice == 'y' || choice == 'Y') {
		int questionCount = 0;
		std::cin.ignore();
		while (true) {
			std::cout << "Enter exam name for take the exam: ";
			std::getline(std::cin, chosenExamName);

			std::string checkExamNameSQL = "SELECT COUNT(*) FROM exams WHERE exam_name = ?;";
			sqlite3_stmt* checkExamNameStmt;
			rc = sqlite3_prepare_v2(db, checkExamNameSQL.c_str(), -1, &checkExamNameStmt, nullptr);
			if (rc != SQLITE_OK) {
				std::cerr << "Check exam name SQL error: " << sqlite3_errmsg(db) << std::endl;
				sqlite3_close(db);
				return;
			}

			sqlite3_bind_text(checkExamNameStmt, 1, chosenExamName.c_str(), -1, SQLITE_STATIC);


			if (sqlite3_step(checkExamNameStmt) == SQLITE_ROW) {
				questionCount = sqlite3_column_int(checkExamNameStmt, 0);
			}

			sqlite3_finalize(checkExamNameStmt);

			if (questionCount > 0) {
				break;
			}
			else {
				std::cout << "Enter the exam name correctly!" << std::endl;
			}

		}

		int usersCorrectAnswer = 0;
		std::cout << "Exam " << chosenExamName << " started!!!\n_____________________________________________________" << std::endl;

		std::string questionsSQL = "SELECT * FROM exams WHERE exam_name = ?;";
		sqlite3_stmt* questionsStmt;
		rc = sqlite3_prepare_v2(db, questionsSQL.c_str(), -1, &questionsStmt, nullptr);
		if (rc != SQLITE_OK) {
			std::cerr << "Questions load SQL error: " << sqlite3_errmsg(db) << std::endl;
			sqlite3_close(db);
			return;
		}

		sqlite3_bind_text(questionsStmt, 1, chosenExamName.c_str(), -1, SQLITE_STATIC);

		while (sqlite3_step(questionsStmt) == SQLITE_ROW) {
			std::string question = (const char*)sqlite3_column_text(questionsStmt, 2);
			std::cout << question << std::endl;
			std::string options = (const char*)sqlite3_column_text(questionsStmt, 3);
			std::cout << options << std::endl;

			char chosenOption;
			char correctOption = ' ';
			if (sqlite3_column_text(questionsStmt, 4) != nullptr) {
				correctOption = *(const char*)sqlite3_column_text(questionsStmt, 4);
			}

			std::cout << "Enter your variant in capital letters (A,B...): ";
			std::cin >> chosenOption;
			if (chosenOption == correctOption) {
				std::cout << "Correct answer!" << std::endl;
				usersCorrectAnswer++;
			}
			else {
				std::cout << "Wrong answer! Correct answer was: " << correctOption << std::endl;
			}
		}

		sqlite3_finalize(questionsStmt);
		int examResult = (100 / questionCount) * usersCorrectAnswer;

		std::cout << "Exam finished! Your score: " << examResult << "%" << std::endl;

		std::string createSQL = "CREATE TABLE IF NOT EXISTS users_results ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"username TEXT NOT NULL, "
			"took_exam_name TEXT NOT NULL, "
			"result INTEGER NOT NULL);";

		char* errMsg = nullptr;
		rc = sqlite3_exec(db, createSQL.c_str(), nullptr, nullptr, &errMsg);
		if (rc) {
			std::cerr << "Exam results table create SQL error: " << errMsg << std::endl;
			sqlite3_free(errMsg);
			sqlite3_close(db);
			return;
		}

		std::string insertSQL = "INSERT INTO users_results (username, took_exam_name, result) VALUES (?,?,?);";
		sqlite3_stmt* insertStmt;
		rc = sqlite3_prepare_v2(db, insertSQL.c_str(), -1, &insertStmt, nullptr);
		if (rc != SQLITE_OK) {
			std::cerr << "Insert result SQL error: " << sqlite3_errmsg(db) << std::endl;
		}

		sqlite3_bind_text(insertStmt, 1, username.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(insertStmt, 2, chosenExamName.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(insertStmt, 3, examResult);
		if (sqlite3_step(insertStmt) == SQLITE_DONE) {
			std::cout << "Exam results saved successfully!" << std::endl;
		}
		else {
			std::cout << "Exam results saving failed!" << std::endl;
			sqlite3_finalize(insertStmt);
			sqlite3_close(db);
			return;
		}

		sqlite3_finalize(insertStmt);

	}

	sqlite3_close(db);

}

void User::showResults() {

	sqlite3* db;
	sqlite3_stmt* stmt;

	int rc = sqlite3_open("exam_system.db", &db);
	if (rc) {
		std::cerr << "Database error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	std::string showResultsSQL = "SELECT took_exam_name, result FROM users_results WHERE username = ?;";

	rc = sqlite3_prepare_v2(db, showResultsSQL.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "Show results SQL error: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

	std::cout << "Your Exam Results:\n";
	std::cout << "________________________\n";

	bool hasResults = false;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		hasResults = true;
		std::string examName = (sqlite3_column_text(stmt, 0)) ? (const char*)sqlite3_column_text(stmt, 0) : "NULL";
		int result = sqlite3_column_int(stmt, 1);

		std::cout << "Exam: " << examName << " | " << "Score: " << result << "%" << std::endl;
	}

	if (!hasResults) {
		std::cout << "No exam results found!" << std::endl;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

}

void User::showUsersRating() {

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
