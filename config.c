#include <syslog.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include "defs.h"

void stripPath(char * path) {
    /**
     * remove trailing newlines from path
     */
    while (path[strlen((const char*)path) - 1] == '\n')
        path[strlen((const char*)path) - 1] = 0;
}

int readUInt(char * key, unsigned int * confVal, char * line) {
    int len = strlen(key);
    if (strncmp(line, key, len) != 0) return 1;
    char * ptr = line + len;
    while(ptr[0] != '=') ptr++;
    ptr++;
    *confVal = (unsigned int) strtoul(ptr, NULL, 10);
    printf("set %s to %u\n", key, *confVal);
    return 0;
}

int readPath(char * key, char * confVal, char * line) {
    int len = strlen(key);
    if (strncmp(line, key, len) != 0) return 1;
    char * ptr = line + len;
    while(ptr[0] != '/') ptr++;
    strncpy(confVal, ptr, M_PATH);
    stripPath(confVal);
    printf("set %s to %s\n", key, confVal);
    return 0;

}

int readDouble(char * key, double * confVal, char * line) {
    int len = strlen(key);
    if (strncmp(line, key, len) != 0) return 1;
    char * ptr = line + len;
    while(ptr[0] != '=') ptr++;
    ptr++;
    *confVal = (double) strtod(ptr, NULL);
    printf("set %s to %e\n", key, *confVal);
    return 0;

}


int readConfig(SConfig *config) {
    /**
     * read config from /etc/autothrottle.conf
     * TODO refactor config path to allow setting with program arguments
     */
    FILE *fd = NULL;
    char line[1024];
    //char *ptr = NULL;
    int currLine = 0;

    syslog(LOG_INFO, "reading configuration");

    // Set defaults
    config->maxMaxFreq = 3500;
    config->minMaxFreq =  800;
    config->pollingDelay = 1000;
    config->verbosity = 1; //TODO use

    config->targetTemp = 80;

    config->thermalTempPath[0] = 0;
    config->maxCpuFreqPath[0] = 0;

    config->numCPUs = 8;

    config->writeCSV = 1;
    config->CSVPath[0] = 0;

    config->kP = 70;
    config->kI = 0.01;
    config->kD = 750000;
    config->kA = 0.0001;
    strcpy((char*)config->thermalTempPath, (const char*)"/sys/class/thermal/thermal_zone0/temp");
    strcpy((char*)config->maxCpuFreqPath, (const char*)"/sys/devices/system/cpu/cpu%d/cpufreq/scaling_max_freq");
    strcpy((char*)config->CSVPath, (const char*)"/tmp/autothrottle.csv");

    fd = fopen("/etc/autothrottle.conf", "r");
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

        if (readUInt("max_freq", &config->maxMaxFreq, line) == 0) continue;
        if (readUInt("min_freq", &config->minMaxFreq, line) == 0) continue;
        if (readUInt("delay", &config->pollingDelay, line) == 0) continue;
        if (readUInt("target_temp", &config->targetTemp, line) == 0) continue;
        if (readUInt("write_csv", &config->writeCSV, line) == 0) continue;
        if (readUInt("num_cpus", &config->numCPUs, line) == 0) continue;

        if (readPath("temp_path", (char *) config->thermalTempPath, line) == 0) continue;
        if (readPath("cpufreq_path", (char *) config->maxCpuFreqPath, line) == 0) continue;
        if (readPath("csv_path", (char *) config->CSVPath, line) == 0) continue;

        if (readDouble("k_p", &config->kP, line) == 0) continue;
        if (readDouble("k_i", &config->kI, line) == 0) continue;
        if (readDouble("k_d", &config->kD, line) == 0) continue;
        if (readDouble("k_a", &config->kA, line) == 0) continue;
    }
    syslog(LOG_NOTICE, "Found config file");
    fclose(fd);

    // Sanity Checks
    if (config->pollingDelay < 500) {

        if (config->pollingDelay == 0) {
            syslog(LOG_ERR, "delay value equals 0");
            return 1;
        }
        syslog(LOG_WARNING, "delay is very low. this can cause problems");
    }

    if (config->kD == 0 || config->kI == 0) {
        syslog(LOG_WARNING, "config.kD or config.kI is set to 0 or could not be read");
        syslog(LOG_WARNING, "this may be intended");
    }


    if (config->kP == 0 || config->kA == 0) {
        syslog(LOG_ERR, "config.kP or config.kA is set to 0 or could not be read");
        return 2;
    }

    return 0;
}

