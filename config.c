#include <syslog.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include "defs.h"

void stripPath(char * path) {
    while (path[strlen((const char*)path) - 1] == '\n')
        path[strlen((const char*)path) - 1] = 0;
}


int readConfig(SConfig *config) {
    FILE *fd = NULL;
    char line[1024];
    char *ptr = NULL;
    int currLine = 0;

    syslog(LOG_INFO, "reading configuration");

    // Set defaults
    config->maxMaxFreq = 3500000;
    config->minMaxFreq =  800000;
    config->pollingDelay = 1000;
    config->verbosity = 1; //TODO use

    config->targetTemp = 80000;

    config->thermalTempPath[0] = 0;
    config->maxCpuFreqPath[0] = 0;

    config->writeCSV = 1;
    config->CSVPath[0] = 0;

    strcpy((char*)config->thermalTempPath, (const char*)"/sys/class/thermal/thermal_zone0/temp");
    strcpy((char*)config->maxCpuFreqPath, (const char*)"/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq");
    strcpy((char*)config->CSVPath, (const char*)"/tmp/autothrottle.csv");

    fd = fopen("/home/d3da/dev/autothrottle/autothrottle.conf", "r");
    if (fd == NULL) {
            syslog(LOG_WARNING, "failed to open autothrottle configuration, using defaults");
            return 0;
    }

    while (!feof(fd)) {
        memset(line, 0, 1024);
        if (fgets(line, 1022, fd) == NULL) break;

        currLine++;

        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;

        if (strncmp(line, "max_freq", 8) == 0) {
            ptr = line + 8;
            while (ptr[0] != '=') ptr++;
            ptr++;
            config->maxMaxFreq = (unsigned int) strtoul(ptr, NULL, 10);
            printf("set max freq to %u\n", config->maxMaxFreq);
            continue;
        }


        if (strncmp(line, "min_freq", 8) == 0) {
            ptr = line + 8;
            while (ptr[0] != '=') ptr++;
            ptr++;
            config->minMaxFreq = (unsigned int) strtoul(ptr, NULL, 10);
            printf("set min freq to %u\n", config->minMaxFreq);
            continue;
        }
        if (strncmp(line, "delay", 5) == 0) {
            ptr = line + 5;
            while (ptr[0] != '=') ptr++;
            ptr++;
            config->pollingDelay = (unsigned int) strtoul(ptr, NULL, 10);
            printf("set polling delay to %u\n", config->pollingDelay);
            continue;
        }
        if (strncmp(line, "target_temp", 11) == 0) {
            ptr = line + 11;
            while (ptr[0] != '=') ptr++;
            ptr++;
            config->targetTemp = (unsigned int) strtoul(ptr, NULL, 10);
            printf("set target temp to %u\n", config->targetTemp);
            continue;
        }
        if (strncmp(line, "temp_path", 9) == 0) {
            ptr = line + 9;
            while (ptr[0] != '/') ptr++;
            strncpy((char*)config->thermalTempPath, ptr, M_PATH);
            stripPath(config->thermalTempPath);
            printf("set temperature path to %s\n", config->thermalTempPath);
            continue;
        }
        if (strncmp(line, "cpufreq_path", 12) == 0) {
            ptr = line + 12;
            while (ptr[0] != '/') ptr++;
            strncpy((char*)config->maxCpuFreqPath, ptr, M_PATH);
            stripPath(config->maxCpuFreqPath);
            printf("set cpu frequency path to %s\n", config->maxCpuFreqPath);
            continue;
        }
        if (strncmp(line, "write_csv", 9) == 0) {
            ptr = line + 9;
            while (ptr[0] != '=') ptr++;
            ptr++;
            config->writeCSV = (int) strtol(ptr, NULL, 10);
            if (config->writeCSV == 0)
                printf("set CSV file writing to false\n");
            else
                printf("set CSV file writing to true\n");
            continue;
        }
        if (strncmp(line, "csv_path", 8) == 0) {
            ptr = line + 8;
            while (ptr[0] != '/') ptr++;
            strncpy((char *)config->CSVPath, ptr, M_PATH);
            stripPath(config->CSVPath);
            printf("set csv path to %s\n", config->CSVPath);
            continue;
        }
    }
    syslog(LOG_NOTICE, "Found config file");
    fclose(fd);

    // Sanity Checks
    if (config->pollingDelay == 0) {
        syslog(LOG_ERR, "delay value equals 0");
        return 1;
    }

    return 0;
}

