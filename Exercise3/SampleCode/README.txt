14708887 14706584

n_rows = number of rows in image
n_cols = number of columns in image
n_threads = number of threads

if the program works in parallel mode, each thread analyses the pixels inside the rectangle:
starting point: (0, i * (n_rows/n_threads) )
ending point: (n_cols, (i+1) * (n_rows/n_threads) )
	where i is the thread number in range [0,n_threads-1]
	
a instance of the class task is being passed to each thread. task contains information about:
- starting & ending points
- window size
- input image
- pointer to filtered image

after all threads have finished executing, the filtered image is being written on a txt file