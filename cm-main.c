#include "cm-main.h"

//for arg handling
/*
should use default filename "out.txt", and print error if not found
options shoudl include:
    full update (requires filename) -f
    upadte without reconciling duplicates -u
*/
void print_usage(const char *prog_name) {
    printf("Usage: %s [-h] [-u filename] [-f filename]\n", prog_name);
    printf("Options:\n");
    printf("  -h        Show help\n");
    printf("  -f        use new file and reconcile duplicates\n");
    printf("  -u        use new file without reconcile\n");
}


// Entry point for the credential management program.
// Reads credentials from a file, processes duplicates, handles same accounts,
// saves results, and provides an interactive query interface.
int main(int argc, char *argv[]) {
    //args handling
    //int verbose = 0;
    char *filename = NULL;
    bool newfile = false;
    bool reconcile = false;
    
    // Start from 1 to skip program name (argv[0])
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 < argc) {
                filename = argv[++i];
                newfile = true;
                reconcile = true;
            } else {
                fprintf(stderr, "Error: -f requires a filename\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-u") == 0) {
            if (i + 1 < argc) {
                filename = argv[++i];
                newfile = true;
            } else {
                fprintf(stderr, "Error: -u requires a filename\n");
                return 1;
            }
        }
        else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    } 

    // Example processing

    // if (verbose) {
    //     printf("Verbose mode enabled\n");
    // }
    
    if (filename) {
        printf("Input file: %s\n", filename);
    }
    
    //use pre-processed cred file if available
    if (argc == 1) {
        filename = DEFAULT_FILE;
    }
    //end of args handling

    // Initialize credential array
    int num_creds = 0;
    count_creds(filename, &num_creds);
    Credential *creds = malloc(num_creds * sizeof(Credential));
    if (!creds) {
        perror("error allocating memory");
        return 0;
        
    }
    if (!read_credentials(filename, creds, &num_creds)) {
        return 1;
    }
    
    // use args to control flow
    if(newfile == true) {           
            
        // Process credentials
        remove_duplicates(creds, &num_creds);
        printf("Removed exact duplicate credential blocks.\n");
        sleep_ms(1000);

        if (reconcile == true) {
            handle_same_accounts(creds, &num_creds);
            printf("Processed multiple entries for same accounts.\n");
            sleep_ms(1000);
        }

        save_credentials(DEFAULT_FILE, creds, num_creds);
    }

    query_credentials(creds, num_creds);
    free_credentials(creds, num_creds);

    return 0;
}