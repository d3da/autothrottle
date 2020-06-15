#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "defs.h"
#include "config.h"

SConfig config;
int allowedToRun = 1;
PID pid_c;

unsigned int getTemperature(void) {
    unsigned int i = 0;
    char content[100];
    unsigned int conlen = 0;
    char *ptr = NULL;
    FILE *fd = NULL;
    int read = 0;

    fd = fopen((const char*) config.thermalTempPath, "r");
    if (fd == NULL) {
        syslog(LOG_ERR, "failed to open \"%s\" (%s)", config.thermalTempPath, strerror(errno));
        return 0;
    }
    memset(content, 0, 100);
    read = fread(content, sizeof(char), 99, fd);

    if (read == 0) {
        syslog(LOG_ERR, "failed to read \"%s\" (%s)", config.thermalTempPath, strerror(errno));
        fclose(fd);
        return 0;
    }

    fclose(fd);
    conlen = strlen(content);

    for (i = 0; i < conlen; i++) {
        if (content[i] >= '0' && content[i] <= '9') {
            ptr = content + i;
            break;
        }
    }
    return (unsigned int) strtol(ptr, NULL, 10);
}


unsigned int getMaxFreq(void) {
    unsigned int i = 0;
    char content[100];
    unsigned int conlen = 0;
    char *ptr = NULL;
    FILE *fd = NULL;
    int read = 0;

    fd = fopen((const char*) config.maxCpuFreqPath, "r");
    if (fd == NULL) {
        syslog(LOG_ERR, "failed to open \"%s\" (%s)", config.maxCpuFreqPath, strerror(errno));
        return 0;
    }
    memset(content, 0, 100);
    read = fread(content, sizeof(char), 99, fd);

    if (read == 0) {
        syslog(LOG_ERR, "failed to read \"%s\" (%s)", config.maxCpuFreqPath, strerror(errno));
        fclose(fd);
        return 0;
    }

    fclose(fd);
    conlen = strlen(content);

    for (i = 0; i < conlen; i++) {
        if (content[i] >= '0' && content[i] <= '9') {
            ptr = content + i;
            break;
        }
    }
    return (unsigned int) strtol(ptr, NULL, 10);

}

unsigned int getTargetCpuFreq(unsigned int temp, unsigned int currFreq) {
    /*
     * Currently P-controller, should be tweaked PID-controller eventually
     */
    long P = (long) ((long)temp - (long)config.targetTemp);
    double kP = 69;
    double kI = 0.05;
    double kD = 500000;
    double kA = 0.2;
    double derivative = (double) ((int)temp - (int)pid_c.P) / (int)config.pollingDelay;

    printf("P=%f\n", P*kP);
    printf("I=%f\n", pid_c.I*kI);
    printf("D=%f\n", derivative*kD);


    // update info
    pid_c.P = temp;
    pid_c.I = pid_c.I + (P * config.pollingDelay);
    if (pid_c.I < 0) pid_c.I = 0;

    double err = P * kP + derivative * kD + pid_c.I * kI;
    long diff = err * kA;
    printf("diff=%ld\n",diff);
    long target = currFreq - diff;

    printf("err=%f\n", err);
    printf("target=%ld\n", target);

    if (target > config.maxMaxFreq) return config.maxMaxFreq;
    if (target < config.minMaxFreq) return config.minMaxFreq;
    return (unsigned int) target;
}

int setCpuFreq(unsigned int targetFreq) {
    FILE * fd = fopen(config.maxCpuFreqPath, "w");

    if (fd == NULL) {
        syslog(LOG_ERR, "failed to open \"%s\" (%s)", config.maxCpuFreqPath, strerror(errno));
        return 1;
    }
    fprintf(fd, "%u", targetFreq);
    fclose(fd);
    return 0;

}


void writeCSV(FILE* fd, int i, unsigned int temp, unsigned int freq) {
    fprintf(fd, "%i,%u,%u;\n", i, temp, freq);
    fflush(fd);
}


void sighandler(int sig) {
    if (sig == SIGTERM) {
        allowedToRun = 0;
        return;
    }
}
    


int main(int argc, char** argv) {

    int daemonMode = 1;

    if (getuid() != 0 || getgid() != 0) {
        fprintf(stderr, "autothrottle should be run as root\n");
        return 1;
    }

    if (argc != 1) {
        if (strcmp(argv[1], "--no-daemon") == 0) {
            daemonMode = 0;
        }
    }

    if (daemonMode != 0) {
        if (daemon(0, 0) == -1) {
            fprintf(stderr, "failed to create child process\n");
            return 2;
        }
    }

    signal(SIGTERM, sighandler);
    if (readConfig(&config) != 0) {
        return 3;
    }
    syslog(LOG_NOTICE, "Starting autothrottle");

    unsigned int temp = 0;
    unsigned int maxFreq = 0;
    unsigned int targFreq = 0;
    unsigned int i = 0;

    FILE * CSVfd;

    if (config.writeCSV != 0) {
        CSVfd = fopen(config.CSVPath, "w");
        if (CSVfd == NULL) {
            syslog(LOG_ERR, "Could not open \"%s\" (%s)", config.CSVPath, strerror(errno));
            return 4;
        }
        setlinebuf(CSVfd);
    }


    while (allowedToRun) {
        i++;
        temp = getTemperature();
        if (temp == 0) {
            syslog(LOG_ERR, "cant read temperature, terminating");
            break;
        }
        maxFreq = getMaxFreq();
        if (maxFreq == 0) {
            syslog(LOG_ERR, "cant get current maximum frequency, terminating");
            break;
        }
        targFreq = getTargetCpuFreq(temp, maxFreq);
        printf("temperature=%u\n", temp);
        printf("maxfreq=%u\n", maxFreq);
        printf("target=%u\n", targFreq);

        if (setCpuFreq(targFreq) != 0) {
            syslog(LOG_ERR, "cant set maximum frequency, terminating");
            break;
        }
        if (config.writeCSV != 0) {
            writeCSV(CSVfd, i, temp, maxFreq);
        }
        

        usleep((useconds_t) config.pollingDelay * 1000);
    }
    if (config.writeCSV != 0) {
        fclose(CSVfd);
    }
    syslog(LOG_NOTICE, "Stopping autothrottle");
}
