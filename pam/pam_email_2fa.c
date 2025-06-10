#define _GNU_SOURCE
#include <security/pam_modules.h>
#include <security/pam_ext.h>
#include <security/pam_appl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
#include <time.h>

#define DB_PATH "/pam_sms_2fa/pam_sms.db"
#define EMAIL_SCRIPT "/pam_sms_2fa/send_email.sh"

static int get_email_for_user(const char *username, char *email_out, size_t len) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int return_code;

    return_code = sqlite3_open(DB_PATH, &db);
    if (return_code != SQLITE_OK) return -1;

    const char *query = "SELECT email FROM users WHERE username = ?";
    return_code = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (return_code != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *email = sqlite3_column_text(stmt, 0);
        strncpy(email_out, (const char *)email, len);
        email_out[len - 1] = '\0';
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 0;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return -1;
}

static void generate_code(char *out, size_t len) {
    srand(time(NULL) ^ getpid());
    snprintf(out, len, "%06d", rand() % 1000000);
}

PAM_EXTERN int pam_sms_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    const char *username;
    char email[64] = {0};
    char code[8] = {0};
    char user_input[16] = {0};

    if (pam_get_user(pamh, &username, NULL) != PAM_SUCCESS) {
        return PAM_AUTH_ERR;
    }

    if (get_email_for_user(username, email, sizeof(email)) != 0) {
        return PAM_AUTH_ERR;
    }

    generate_code(code, sizeof(code));

    char code_file[64];
    snprintf(code_file, sizeof(code_file), "/tmp/code_%s", username);
    FILE *fp = fopen(code_file, "w");
    if (!fp) return PAM_AUTH_ERR;
    fprintf(fp, "%s\n", code);
    fclose(fp);

    char cmd[256];
    snprintf(cmd, sizeof(cmd), "%s \"%s\" \"%s\"", EMAIL_SCRIPT, email, code);
    system(cmd);

    const char *input = NULL;
    int ret = pam_prompt(pamh, PAM_PROMPT_ECHO_ON, &input, "Enter sent code: ");
    if (ret != PAM_SUCCESS || !input) return PAM_AUTH_ERR;

    strncpy(user_input, input, sizeof(user_input) - 1);
    user_input[sizeof(user_input) - 1] = '\0';

    size_t len = strlen(user_input);
    if (len > 0 && user_input[len - 1] == '\n') {
        user_input[len - 1] = '\0';
    }

    if (strcmp(user_input, code) == 0) {
        return PAM_SUCCESS;
    }

    return PAM_AUTH_ERR;
}

PAM_EXTERN int pam_sms_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}
