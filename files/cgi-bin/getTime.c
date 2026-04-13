#include <stdio.h>
#include <time.h>

int main(void) {
    // 1. Get the current system time
    time_t now = time(NULL);
    char *time_str = ctime(&now);

    // 2. Print HTTP Headers (Required for CGI)
    // No spaces between header name and colon
    printf("Content-Type: text/html; charset=utf-8\r\n");
    printf("Status: 200 OK\r\n");
    
    // 3. The empty line that separates headers from the body
    printf("\r\n");

    // 4. The HTML Body
    printf("<!DOCTYPE html>\n");
    printf("<html>\n<head><title>CGI Time</title></head>\n");
    printf("<body>\n");
    printf("<h1>Current Server Time</h1>\n");
    printf("<p>The time is: <strong>%s</strong></p>\n", time_str);
    printf("</body>\n</html>\n");

    return 0;
}