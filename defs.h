#define M_PATH 512

typedef struct {
    unsigned int maxMaxFreq;
    unsigned int minMaxFreq;
    unsigned int pollingDelay;
    unsigned char thermalTempPath[M_PATH];
    unsigned char maxCpuFreqPath[M_PATH];
    unsigned int targetTemp;
    unsigned int verbosity;
    int writeCSV;
    unsigned char CSVPath[M_PATH];
} SConfig;

typedef struct {
    long long I;
    unsigned int P;
    /* TODO allow configuring PID controller parameters without recompiling 
    double kP;
    double kI;
    double kD;
    double kA;
    */
    /* new_freq = old_freq + kA* (P*kP + I*kI + D*kD); */
} PID;
