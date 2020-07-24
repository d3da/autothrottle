#define M_PATH 512

typedef struct {
    unsigned int maxMaxFreq;
    unsigned int minMaxFreq;
    unsigned int pollingDelay;
    char thermalTempPath[M_PATH];
    char maxCpuFreqPath[M_PATH];
    unsigned int targetTemp;
    unsigned int verbosity;
    unsigned int writeCSV;
    unsigned int numCPUs;
    char CSVPath[M_PATH];
    double kP;
    double kI;
    double kD;
    double kA;

} SConfig;

// struct for the PID controller to use
typedef struct {
    long long I;
    unsigned int P;
} PID;
