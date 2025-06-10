#include <stdio.h>
#include <string.h>
#include "database.h"


int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage:\n");
		printf("%s init-database\n", argv[0]);
		printf("%s add-user <username> <email>\n", argv[0]);
		printf("%s delete-user <username> <email>\n", argv[0]);
		printf("%s users\n", argv[0]);
		printf("%s sender-settings", argv[0]);
		return -1;
	}

	if (strcmp(argv[1], "init-database") == 0) {
		if (argc != 2) {
			fprintf(stderr, "Usage: %s init", argv[0]);
			return -1;
		}
		return database_init();
	}

	if (strcmp(argv[1], "add-user") == 0) {
		if (argc != 4) {
			fprintf(stderr, "Usage: %s add-user <username> <email>", argv[0]);
			return -1;
		}
		return u_add_user(argv[2], argv[3]);
	}

	if (strcmp(argv[1], "delete-user") == 0) {
		if (argc != 4) {
			fprintf(stderr, "Usage: %s delete-user <username> <email>", argv[0]);
		}
		return 0;
	}

	if (strcmp(argv[1], "users") == 0) {
		if (argc != 2) {
			fprintf(stderr, "Usage: %s users", argv[0]);
			return -1;
		}
		return print_out_all_users();
	}

	if (strcmp(argv[1], "sender-settings") == 0) {
		if (argc != 2) {
			fprintf(stderr, "Usage: %s sender-settings", argv[0]);
			return -1;
		}
		return select_sender_settings();
	}
	return -1;
}