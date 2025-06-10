#ifndef EMAIL_H
#define EMAIL_H

/**
 * Отправка письма через SMTP.
 *
 * @param smtp_url  URL SMTP-сервера, например, "smtp://smtp.gmail.com:587"
 * @param username  Имя пользователя (обычно email)
 * @param password  Пароль (или app password)
 * @param from      Адрес отправителя
 * @param to        Адрес получателя
 * @param subject   Тема письма
 * @param body      Текст письма
 *
 * @return 0 при успешной отправке, иначе -1
 */
int send_email(const char *smtp_url,
               const char *username,
               const char *password,
               const char *from,
               const char *to,
               const char *subject,
               const char *body);

#endif // EMAIL_H
