#ifndef DATABASE_H
#define DATABASE_H

int database_init(void);
int s_add_sender(const char *username, const char *password, const char *smtp_url, const char *sender_email);
int u_add_user(const char *username, const char *email);
int print_out_all_users(void);
int select_sender_settings(void);

#endif