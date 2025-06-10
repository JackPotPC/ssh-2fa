#include <sqlite3.h>
#include <stddef.h>
#include <stdio.h>
#include "database.h"


int database_init() {
    sqlite3 *db;
    char *sql = "CREATE TABLE IF NOT EXISTS users ("
                      "id INTEGER PRIMARY KEY, "
                      "username TEXT NOT NULL, "
                      "email TEXT NOT NULL);";
    char *err_msg = NULL;

    int rc = sqlite3_open("pam_email.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }

    sql = "CREATE TABLE IF NOT EXISTS sender ("
    				  "id INTEGER PRIMARY KEY, "
    				  "sender_username TEXT NOT NULL, "
    				  "sender_password TEXT NOT NULL, "
    				  "smtp_url TEXT NOT NULL, "
    				  "sender_email TEXT NOT NULL);";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(db);
		return -1;
	}

    sqlite3_close(db);
    return 0;
}

int s_add_sender(const char *username, const char *password, const char *smtp_url, const char *sender_email) {
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int return_code;

	return_code = sqlite3_open("pam_email.db", &db);
	if (return_code != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	const char *sql = "INSERT INTO sender (sender_username, sender_password, smtp_url, sender_email) VALUES (?, ?, ?, ?)";

	return_code = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (return_code != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, smtp_url, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, sender_email, -1, SQLITE_STATIC);

	return_code = sqlite3_step(stmt);
	if (return_code != SQLITE_DONE) {
		fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return -1;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return 0;
}

int u_add_user(const char *username, const char *email) {
	sqlite3 *db;
	sqlite3_stmt *stmt;  // указатель, а не структура!
	int return_code;

	return_code = sqlite3_open("pam_email.db", &db);
	if(return_code != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	const char *sql = "INSERT INTO users (username, email) VALUES (?, ?);";

	return_code = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (return_code != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);

	return_code = sqlite3_step(stmt);
	if (return_code != SQLITE_DONE) {
		fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return -1;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return 0;
}

int print_out_all_users() {
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int return_code;

	return_code = sqlite3_open("pam_email.db", &db);
	if (return_code != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	const char *sql = "SELECT id, username, email FROM users;";

	return_code = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (return_code != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare SELECT: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	printf("Users:\n");
    while ((return_code = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *username = sqlite3_column_text(stmt, 1);
        const unsigned char *email = sqlite3_column_text(stmt, 2);

        printf("ID: %d, Username: %s, Email: %s\n", id, username, email);
    }

    if (return_code != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
	return 0;
}

int select_sender_settings() {
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int rc;

	rc = sqlite3_open("pam_email.db", &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}

	const char *sql = "SELECT id, sender_username, sender_password, smtp_url, sender_email FROM sender;";
	printf("Sender setting:\n");
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		int id = sqlite3_column_int(stmt, 0);
		const unsigned char *username = sqlite3_column_text(stmt, 1);
        const unsigned char *password = sqlite3_column_text(stmt, 2);
        const unsigned char *smtp_url = sqlite3_column_text(stmt, 3);
        const unsigned char *sender_email = sqlite3_column_text(stmt, 4);
        printf("ID: %d\nUsername: %s\nPassword: %s\nSmtp_url: %s\nEmail: %s\n", id, username, password, smtp_url, sender_email);
	}

	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return 0;
}