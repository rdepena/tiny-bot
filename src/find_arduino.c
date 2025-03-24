#include <dirent.h>
#include <stdio.h>
#include <string.h>

char* find_arduino() {
    static char portname[128];
    struct dirent *entry;
    DIR *dp = opendir("/dev/");

    if (dp == NULL) {
        perror("opendir");
        return NULL;
    }

    while ((entry = readdir(dp))) {
        if (strncmp(entry->d_name, "ttyUSB", 6) == 0 ||  // Linux
            strncmp(entry->d_name, "ttyACM", 6) == 0 ||  // Linux
            strncmp(entry->d_name, "cu.usbmodem", 11) == 0 ||  // macOS
            strncmp(entry->d_name, "cu.usbserial", 12) == 0) {  // macOS
            snprintf(portname, sizeof(portname), "/dev/%s", entry->d_name);
            closedir(dp);
            return portname;
        }
    }
    closedir(dp);
    return NULL;
}