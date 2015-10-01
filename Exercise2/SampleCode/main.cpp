#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>

// count the occurrences of pattern_ inside the filename_ and write the result (a single number) to a file called "result_PID.txt",
// where PID is the process id of the calling process. NOTE: must use execl to perform the count and write to file!

void occurrences_in_file( const std::string& filename_, const std::string& pattern_ )
{
  int my_pid;
  // my_pid = ...

  std::string search_command;
  // generate string containing the command to be passed as argument to /bin/sh

  // call exec() to execute the command in search_command as argument to /bin/sh

  // check correct termination
}


// open a text file, read partial count of occurrences and return it as an integer

int read_occurrences_file( const std::string& filename_ )
{
  // ...
}


// entry point of the application

int main( int argc, char* argv[] )
{
    if (argc != 2) {
        std::cerr << "None or Too many arguments passed\n.";
        exit(EXIT_FAILURE);
    }

  std::string pattern( argv[ 1 ] );  
  int files_count;
  // files_count = ...
 
  int* status = new int[ files_count ];
  pid_t* pids = new pid_t[ files_count ];

  // spawn files_count processes
  for( int f = 0; f < files_count; f++ )
  {
    // call fork and invoke occurrences_in_file() from child process
  }

  // wait for termination and check termination
  for( int f = 0; f < files_count; f++ )
  {
    // ...
  }
  

  // open results files, compute overall number of occurrences and print it to standard output
  // ...
  
  delete[] status;
  delete[] pids;
  
  return 0;
}
