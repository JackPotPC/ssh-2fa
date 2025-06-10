#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include "email.h"

int send_email(const char *smtp_url, const char *username, const char *password, const char *from, const char *to, const char *subject, const char *body) {
    CURL *curl;
    CURLcode res;
    struct curl_slist *recipients = NULL;

    char payload[1024];
    snprintf(payload, sizeof(payload),
             "To: %s\r\n"
             "From: %s\r\n"
             "Subject: %s\r\n"
             "\r\n"
             "%s\r\n",
             to, from, subject, body);

    curl = curl_easy_init();
    if (!curl) return -1;

    curl_easy_setopt(curl, CURLOPT_USERNAME, username);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
    curl_easy_setopt(curl, CURLOPT_URL, smtp_url);
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from);

    recipients = curl_slist_append(recipients, to);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

    curl_easy_setopt(curl, CURLOPT_READDATA, payload);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, (long)strlen(payload));

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Email send failed: %s\n", curl_easy_strerror(res));
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
        return -1;
    }

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    return 0;
}
