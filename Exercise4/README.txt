

The program starts by branching to the selected mode.
In serial mode the code executes  normally.
In parallel image mode, the number of threads is defined by the minimum between the number of threads passed by the user and the number of images.
In parallel pixel mode the code is Executed similar to Exercise 3 (only difference being that we are using the OpenMP framework)
In benchmark mode, we are measuring time intervalls by the Timetracker class. We measure the interval needed by each possible mode in milliseconds, and display a nice summary of the intervals.