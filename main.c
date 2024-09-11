#include "main.h"
#include "stringConversion.h"
#include "CRC32Lut.h" //default polynomial LUT version
#include "CRC32Bit.h" //polynomial division version
#include "CRC32SIMD.h" //SIMD LUT version

static const int numberOfVersions = 3;
static const int defaultBenchmarkReps = 10;
static const uint32_t defaultGenerator = 0x04C11DB7;

//Prints a help message
void printHelp(void) {
    printf("This program calculates the checksum for a given file or String using the CRC32 algorithm.\n"
           "Usage: crc32 [options] input\n"
           "       - by default, the input is interpreted as a file\n"
           "       - when using hex numbers, this is the allowed format: 0x...\n"
           "Options:\n"
           "    -V[int]: specifies which version to use, default is 0 (available versions: %d)\n"
           "    -B[int]: runs the program in benchmark mode and outputs the required runtime;\n"
           "             [int] to set the number of repetitions, default is %d\n"
           "    -G[uint32]: calculates the checksum for a given generator polynomial,\n"
           "                default is the IEEE802.3 specified polynomial 0x04C11DB7\n"
           "    -s: interprets the input as a string\n"
           "    -h/--help: outputs a description and available options for the CRC32 program\n"
           "Examples:\n"
           "    ./crc32 input.txt\n"
           "    ./crc32 -V1 \"The quick brown fox jumps over the lazy dog\"\n"
           "    ./crc32 -V0 -B50 \"The quick brown fox jumps over the lazy dog\"\n",
           numberOfVersions, defaultBenchmarkReps);
}



int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Invalid number of arguments. See help message below:\n");
        printHelp();
        exit(EXIT_FAILURE);
    }

    char* charOptions = "V:B::G:hs";
    struct option longopts[] = {{"help", 0, NULL, 'h'}};

    int option;
    int optB = 0;
    int64_t version = 0;
    int64_t testIterations = defaultBenchmarkReps;
    uint32_t generator = defaultGenerator;
    bool isPath = true;

    //Iterates through options and sets corresponding flags
    while ((option = getopt_long(argc, argv, charOptions, longopts, NULL)) != -1) {
        switch(option) {
            case 'h':
                printHelp();
                exit(EXIT_SUCCESS);
            case 'V':
                if ((version = convertStringToInt(optarg)) < 0 ||
                version > (numberOfVersions - 1)) {
                    fprintf(stderr, "Invalid version number. Please select a version from 0 to %d.\n", (numberOfVersions - 1));
                    printHelp();
                    exit(EXIT_FAILURE);
                }
                break;
            case 'B':
                optB = 1;
                if (optarg == 0) {
                    break;
                } else if ((testIterations = convertStringToInt(optarg)) == -1 || testIterations < 1) {
                    fprintf(stderr, "Invalid number of test iterations.\n"
                           "Please specify a number greater than 0 or omit the number for %d iterations\n", defaultBenchmarkReps);
                    printHelp();
                    exit(EXIT_FAILURE);
                }
                break;
            case 'G':
                if (convertStringToInt(optarg) <= 0) {
                    fprintf(stderr, "Invalid generator polynomial. The polynomial cannot be 0 or negative.\n");
                    printHelp();
                    exit(EXIT_FAILURE);
                }
                generator = convertStringToInt(optarg);
                break;
            case 's':
                isPath = 0;
                break;
            default:
                fprintf(stderr, "Option does not exist.\n");
                printHelp();
                exit(EXIT_FAILURE);
        }
    }

    //Checks if there are too little or too many args after the options
    if (optind >= (argc)) {
        fprintf(stderr, "Not enough arguments\n");
        printHelp();
        exit(EXIT_FAILURE);
    } else if ((argc - optind) > 1) {
        fprintf(stderr, "Too many arguments\n");
        printHelp();
    }

    char *input;
    size_t length;

    //Reads a file into a buffer if a file has been passed to the program. Otherwise, it's interpreted as a string.
    if (isPath == 0) {
        input = argv[optind];
        length = strlen(input);
    } else  {
        struct stat isFile;
        stat(argv[optind], &isFile);
        if (S_ISREG(isFile.st_mode)) {
            FILE *f = fopen(argv[optind], "r");
            fseek(f, 0, SEEK_END);
            length = ftell(f);
            fseek(f, 0, SEEK_SET);
            if ((input = malloc(length)) != NULL) {
                size_t temp = fread(input, 1, length, f);
                if (temp != length) {
                    fprintf(stderr, "Read error\n");
                    exit(EXIT_FAILURE);
                }
            } else {
                fprintf(stderr, "Unable to allocate memory.\n");
                fclose(f);
                exit(EXIT_FAILURE);
            }
            fclose(f);
        } else {
            fprintf(stderr, "Invalid file or argument is a path.\n");
            exit(EXIT_FAILURE);
        }
    }

    //Run benchmark
    if (optB) {
        printf("Running %"PRId64" iterations of version %"PRId64" for benchmarking.\n", testIterations, version);
        printf("Polynomial used: 0x%"PRIx32"\n", generator);
        struct timespec start;
        struct timespec end;
        uint32_t result = 0;
        switch(version) {
            case 0:
                clock_gettime(CLOCK_MONOTONIC, &start);
                for (int i = 0; i < testIterations; i++) {
                    CRC32_V0(length, input, reverseUnsigned(generator));
                }
                clock_gettime(CLOCK_MONOTONIC, &end);
                result = CRC32_V0(length, input, reverseUnsigned(generator));
                break;
            case 1:
                clock_gettime(CLOCK_MONOTONIC, &start);
                for (int i = 0; i < testIterations; i++) {
                    CRC32_V1(length, input, generator);
                }
                clock_gettime(CLOCK_MONOTONIC, &end);
                result = CRC32_V1(length, input, generator);
                break;
            case 2:
                clock_gettime(CLOCK_MONOTONIC, &start);
                for (int i = 0; i < testIterations; i++) {
                    CRC32_V2(length, input, generator);
                }
                clock_gettime(CLOCK_MONOTONIC, &end);
                result = CRC32_V2(length, input, reverseUnsigned(generator));
                break;
            default:
                break;
        }
        printf("Result: 0x%"PRIx32"\n", result);

        if (isPath) {
            free(input);
        }

        double time = (end.tv_sec - start.tv_sec) * 1000 + 1e-6 * (end.tv_nsec - start.tv_nsec);
        double average = time / testIterations;
        printf("Benchmark result for %"PRId64" iterations: %lf msec\n", testIterations, average);
        return 0;
    }

    //Run program
    uint32_t result = 0;
    printf("Running version %"PRId64"\n", version);
    printf("Polynomial used: 0x%"PRIx32"\n", generator);
    printf("Message to be sent: %s\n", input);
    switch(version) {
        case 0:
            result = CRC32_V0(length, input, reverseUnsigned(generator));
            break;
        case 1:
            result = CRC32_V1(length, input, generator);
            break;
        case 2:
            result = CRC32_V2(length, input, reverseUnsigned(generator));
            break;
        default:
            break;
    }
    printf("Result: 0x%"PRIx32"\n", result);

    if (isPath) {
        free(input);
    }

    return 0;
}