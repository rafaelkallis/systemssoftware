#include <iostream>
#include <vector>
#include <fstream>

#include <pthread.h>

#include "image_matrix.hpp"

#define N_THREADS 4




// function that performs the median filtering on pixel p(r_,c_) of input_image_,
// using a window of size window_size_
// the function returns the new filtered value p'(r_,c_)
float median_filter_pixel( const image_matrix& input_image_,
                           int r_,
                           int c_,
                           int window_size_ )
{
  //float filtered_value;
    std::vector<float> values;
    
    for(int i=r_-window_size_;i < r_+window_size_;i--){
        for (int j=c_-window_size_; j < c_+window_size_; j--) {
            if (i > 0 && j > 0 && i < input_image_.get_n_rows() && j < input_image_.get_n_cols()) {
                values.push_back(input_image_.get_pixel(i, j));
            }
        }
    }
    std::nth_element(values.begin(), values.begin() + values.size()/2, values.end());
    return values[values.size()/2];
}



void test(const image_matrix& filtered_image, const int& rows, const int& cols){
    for (int i=rows; i>=0; i--) {
        for (int j=cols; j>=0; j--) {
            if (filtered_image.get_pixel(i, j) != median_filter_pixel(filtered_image, i, j, 2)) {
                printf("Problem\n");
            }
        }
    }
}



// struct passed to each thread, containing the information necessary
// to process its assigned range
class task
{
public:
    task(int start_x,
         int start_y,
         int end_x,
         int end_y,
         int window_size,
         image_matrix original_image,
         image_matrix & filtered_image)
    :start_x(start_x),
    start_y(start_y),
    end_x(end_x),
    end_y(end_y),
    window_size(window_size),
    original_image(original_image),
    filtered_image(filtered_image)
    {};
    ~task(){};
    int get_start_x(){return start_x;}
    int get_start_y(){return start_y;}
    int get_end_x(){return end_x;}
    int get_end_y(){return end_y;}
    int get_window_size(){return window_size;}
    image_matrix get_original_image(){return original_image;}
    void set_pixel(int x,int y,float value){filtered_image.set_pixel(y, x, value);}
private:
    int start_x,start_y,end_x,end_y;
    int window_size;
    image_matrix original_image, filtered_image;

};


// function run by each thread
void* func( void* arg ) 
{
  task t_arg = *( task* )arg;

    for (int i=t_arg.get_start_x(); i<t_arg.get_end_x(); i++) {
        for (int j=t_arg.get_start_y(); j<t_arg.get_end_y(); j++) {
            //float f_px= median_filter_pixel(t_arg.get_original_image(), j, i, t_arg.get_window_size())
            t_arg.set_pixel(j,i,median_filter_pixel(t_arg.get_original_image(), j, i, t_arg.get_window_size()));
        }
    }

  pthread_exit( NULL );
}


// read input image from file filename_ into image_in_
bool read_input_image( const std::string& filename_, image_matrix& image_in_ )
{
    std::string tmp;
    bool ret = false;
    std::ifstream is( filename_.c_str() );
    if( is.is_open() )
    {
        int n_rows;
        int n_cols;

        is >> n_rows;
        is >> n_cols;

        image_in_.resize( n_rows, n_cols );

        for( int r = 0; r < n_rows; r++ )
        {
            for( int c = 0; c < n_cols; c++ )
            {
                float value;
                is >> value;
                image_in_.set_pixel( r, c, value );
            }
        }
        is.close();
        ret = true;
    }
    return ret;
}


// write filtered image_out_ to file filename_
bool write_filtered_image( const image_matrix& image_out_ )
{
  bool ret = false;
  std::ofstream os( "filtered.txt" );
  if( os.is_open() )
  {
    int n_rows = image_out_.get_n_rows();
    int n_cols = image_out_.get_n_cols();

    os << n_rows << std::endl;
    os << n_cols << std::endl;

    for( int r = 0; r < n_rows; r++ )
    {
      for( int c = 0; c < n_cols; c++ )
      {
        os << image_out_.get_pixel( r, c ) << " ";
      }
      os << std::endl;
    }
    os.close();
    ret = true;
  }
  return ret;
}



int main( int argc, char* argv[] )
{
  if( argc < 5 )
  {
    std::cerr << "Not enough arguments provided to " << argv[ 0 ] << ". Terminating." << std::endl;
    return 1;
  }

  // get input arguments
  std::string input_filename( argv[ 1 ] );
  int window_size = std::stoi( argv[ 2 ] );
  int n_threads = std::stoi( argv[ 3 ] );
  int mode = std::stoi( argv[ 4 ] );

  std::cout << argv[ 0 ] << " called with parameters " << input_filename << " " << window_size << " " << n_threads << " " << mode << std::endl;

  // input and the filtered image matrices
  image_matrix input_image;
  image_matrix filtered_image;
   
  // read input matrix
  read_input_image( input_filename, input_image );

  // get dimensions of the image matrix
  int n_rows = input_image.get_n_rows();
  int n_cols = input_image.get_n_cols();

  filtered_image.resize( n_rows, n_cols );

  // start with the actual processing
  if( mode == 0 )
  {
    // ******    SERIAL VERSION     ******

    for( int r = 0; r < n_rows; r++ )
    {
      for( int c = 0; c < n_cols; c++ )
      {
        float p_rc_filt = median_filter_pixel( input_image, r, c, window_size );
        filtered_image.set_pixel( r, c, p_rc_filt );
      }
    }

    // ***********************************
  }
  else if( mode == 1 )
  {
    // ******   PARALLEL VERSION    ******

    // declaration of pthread_t variables for the threads
      pthread_t thread[n_threads];

    // declaration of the struct variables to be passed to the threads
      task * t[n_threads];
      
      for (int i=0; i < n_threads; i++) {
                      //Starting X, Starting Y,             Ending X,   Ending Y
          t[i]=new task(0,          i*(n_rows/n_threads),   n_cols,     ((i+1)*(n_rows/n_threads))-1,   window_size,input_image,filtered_image);
      }
      
//      t[0] = new task(0,            0,          n_cols/2-1, n_rows/2-1, window_size,input_image,filtered_image);
//      t[1] = new task(n_cols/2,     0,          n_cols,     n_rows/2-1, window_size,input_image,filtered_image);
//      t[2] = new task(0,            n_rows/2,   n_cols/2-1, n_rows,     window_size,input_image,filtered_image);
//      t[3] = new task(n_cols/2,     n_rows/2,   n_rows,     n_cols,     window_size,input_image,filtered_image);

    // create threads
      for (int i =0; i < N_THREADS; i++) {
          if (pthread_create(&thread[i], NULL, func, (void*)t[i]) != 0) {
              std::cerr << " Error on creation of thread " << i << "\n";
          }
      }

    // wait for termination of threads
      for (int i=0; i< N_THREADS; i++) {
          pthread_join(thread[i], NULL);
      }
    // ***********************************
  }
  else
  {
    std::cerr << "Invalid mode. Terminating" << std::endl;
    return 1;
  }

  // write filtered matrix to file
  write_filtered_image( filtered_image );

  return 0;
}
