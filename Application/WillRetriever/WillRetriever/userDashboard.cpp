#include "mainMenu.hpp"

void dashboard::dashboardTui() {
	system("cls");
	cout << "  ____            _     _                         _ " << endl;
	cout << " |  _ \\  __ _ ___| |__ | |__   ___   __ _ _ __ __| |" << endl;
	cout << " | | | |/ _` / __| '_ \\| '_ \\ / _ \\ / _` | '__/ _` |" << endl;
	cout << " | |_| | (_| \\__ \\ | | | |_) | (_) | (_| | | | (_| |" << endl;
	cout << " |____/ \\__,_|___/_| |_|_.__/ \\___/ \\__,_|_|  \\__,_|" << endl;
	cout << "  User: " << loggedInUsername << endl;
}

void dashboard::userDashboard(){
	dashboardTui();
	willOptions();
}

void dashboard::willOptions() {
	int flag;
	cout << endl << "1.Create a will        2.Access a will        3.Exit" << endl;
	cin >> flag;
	if (flag == 1) {
		willCreator();
	}
	if (flag == 2) {
		willAccess();
	}
	if (flag == 3) {
		system("cls");
		exit(0);
	}
}

void dashboard::createDatabaseAndTable(sqlite3* db) {
    char* errMsg = 0;
    const char* createTableQuery = "CREATE TABLE IF NOT EXISTS TextTable (ID INTEGER PRIMARY KEY AUTOINCREMENT, Username TEXT, Text TEXT);";
    if (sqlite3_exec(db, createTableQuery, 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "Error creating table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

bool dashboard::checkUser(const string& username, sqlite3* db) {
    const char* selectQuery = "SELECT * FROM LoginInfo WHERE username = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, selectQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    int result = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    return result == SQLITE_ROW;
}

void dashboard::willCreator() {
	dashboardTui();
    sqlite3* db;
    int rc = sqlite3_open("database.db", &db);

    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;        
    }

    createDatabaseAndTable(db);

    std::string username;
    while (true) {
        cout << "Enter the name of our inheritor: ";
        cin >> username;
        if (checkUser(username, db)) break;
    }
    
    //username += " ";

    std::string inputText;

    std::cout << "Enter text (type '0' to exit):" << std::endl;

    while(true){
        std::getline(std::cin, inputText);

        if (inputText == "0") {
            break;
        }

        const char* insertQuery = "INSERT INTO TextTable (Username, Text) VALUES (?, ?);";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, insertQuery, -1, &stmt, 0) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, inputText.c_str(), -1, SQLITE_STATIC);

            int stepResult = sqlite3_step(stmt);
            if (stepResult != SQLITE_DONE) {
                std::cerr << "Error inserting data into the database. SQLite step result: " << stepResult << std::endl;
            }

            sqlite3_finalize(stmt);
        }
        else {
            std::cerr << "Error preparing statement for insertion." << std::endl;
        }
    }

    sqlite3_close(db);
}

void dashboard::printTextForUsername(const std::string& username) {
    sqlite3* db;
    int rc = sqlite3_open("database.db", &db);

    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    const char* selectQuery = "SELECT Text FROM TextTable WHERE Username = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, selectQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing statement for selection." << std::endl;
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    bool printedText = false; // Track whether any non-empty text has been printed

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        if (!std::string(text).empty()) {
            if (printedText) {
                std::cout << std::endl; // Print a new line before non-empty text, if not the first
            }

            std::cout << text;
            printedText = true;
        }
    }

    if (printedText) {
        std::cout << std::endl; // Print a new line after the last non-empty text
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void dashboard::willAccess(){
	dashboardTui();
    cout << "Here is your inheritance:" << endl;
    printTextForUsername(loggedInUsername);
}
