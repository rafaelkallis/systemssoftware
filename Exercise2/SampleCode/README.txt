Upon start, the command line arguments are passed over to an argument_check() that checks for correct amount of arguments and existence of provided filenames.
Later on child processes are created and their process IDs are stored in pids[].
The child processes are replaced with /bin/sh grep command via exec()
Each exec creates a unique file.
After all child processes have been terminated, main() finds all created files and sums up all occurrences and gives appropriate output.