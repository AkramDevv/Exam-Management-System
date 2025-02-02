#ifndef ADMIN_H
#define ADMIN_H

#include <string>

class Admin {
private:
	std::string username;
	std::string password;

public:
	Admin(const std::string& username, const std::string& password);

	void setUsernamePas(const std::string& username, const std::string& password);
	bool checkAdminReg();
	void registerAdmin();
	bool login();
	void createUser();
	void deleteUser();
	void showPendingUsers();
	void approveOrRejectUsers();
	void showUsers();
	void showAllUsersResults();
	void showUsersRating();
	void addExam();
	void deleteExam();
	void showExams();


};

#endif