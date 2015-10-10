#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>

// count the occurrences of pattern_ inside the filename_ and write the result (a single number) to a file called "result_PID.txt",
// where PID is the process id of the calling process. NOTE: must use execl to perform the count and write to file!

void occurrences_in_file( const std::string& filename_, const std::string& pattern_ )
{
    // generate string containing the command to be passed as argument to /bin/sh
    std::string search_command("grep -o " + pattern_ + " " + filename_ + " | wc -l > result-" + std::to_string(getpid()) + ".txt");
    
    // call exec() to execute the command in search_command as argument to /bin/sh
    execl("/bin/sh",
          "/bin/sh",
          "-c",
          search_command.c_str(),
          (char*)0);
    
    
    // check correct termination
    std::cerr << "Exec has returned.\n";
}


// open a text file, read partial count of occurrences and return it as an integer

int read_occurrences_file( const std::string& filename_ )
{
    int occurences = 0;
    std::ifstream in(filename_);
    if (in.is_open()) {
        in >> occurences;
    }
    in.close();
    return occurences;
}


/*
 *  performs a check through command line arguments
 */
int argument_check(int argc, char* argv[]){
    if (argc <3) {
        std::cerr <<    "Too few arguments. " <<
                        "Provide at least a pattern and a filename\n";
        return EXIT_FAILURE;
    }
    while (--argc > 1) {
        std::ifstream file(argv[argc]);
        if (file.good()) {
            file.close();
        }else{
            std::cerr << "File " << argv[argc] << " doesn't exist.\n";
            file.close();
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int main( int argc, char* argv[] )
{
    /*
     Terminates if parameters are invalid
     */
    if(argument_check(argc, argv)){
        std::cerr << "Terminating.\n";
        exit(EXIT_FAILURE);
    }

    std::string pattern( argv[ 1 ] );
    int files_count = argc - 2;;
    int total_occurences = 0;
    int* status = new int[ files_count ];
    pid_t* pids = new pid_t[ files_count ];

    // spawn files_count processes
    for( int f = 0; f < files_count; f++ )
    {
        // call fork and invoke occurrences_in_file() from child process
        pids[f] = fork();
        if (pids[f] == 0) {
            occurrences_in_file(std::string(argv[2+f]), pattern);
            exit(EXIT_SUCCESS);
            
        }else if (pids[f] < 0) {
            std::cerr << "Error on fork()\n";
            exit(EXIT_FAILURE);
        }
    }

    // wait for termination and check termination
    for( int f = 0; f < files_count; f++ )
    {
        waitpid(pids[f], &status[f],0);
        if (status[f] == EXIT_FAILURE) {
            std::cerr << argv[2+f] << " had abnormal behaviour during pattern recognition.\n";
        }
    }
  
    // open results files, compute overall number of occurrences and print it to standard output
    for(int f=0;f < files_count; f++)
    {
        total_occurences += read_occurrences_file("result-"+std::to_string(pids[f])+".txt");
    }
    
    std::cout << "Total Occurences of " << pattern << " are " << total_occurences << ".\n";
  
    delete[] status;
    delete[] pids;
  
  return 0;
}
