#define M_PATH 512

typedef struct {
    int * numCPUs;
    char * paths[M_PATH + 1];
} path_array;

typedef struct {
    unsigned int maxMaxFreq;
    unsigned int minMaxFreq;
    unsigned int pollingDelay;
    char thermalTempPath[M_PATH];
    char maxCpuFreqPath[M_PATH];
    unsigned int targetTemp;
    unsigned int verbosity;
    int writeCSV;
    int numCPUs;
    char CSVPath[M_PATH];
    path_array * path_arr;

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
