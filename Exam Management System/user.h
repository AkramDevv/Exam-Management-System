#ifndef USER_H
#define USER_H

#include <string>

class User {
private:
	std::string username;
	std::string password;

public:
	User(const std::string& username, const std::string& password);

	void setUsernamePas(const std::string& username, const std::string& password);
	void registerUser();
	bool login();
	void takeExam();
	void showResults();
	void showUsersRating();

};

#endif