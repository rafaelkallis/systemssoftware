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
                           const int& r_,
                           const int& c_,
                           const int& window_size_ )
{
    std::vector<float> values;
    const int max_row = input_image_.get_n_rows();
    const int max_col = input_image_.get_n_cols();
    
    for(int r = r_ - window_size_;r < r_ + window_size_;r++){
        for (int c = c_ - window_size_; c < c_ + window_size_; c++) {
            if (r > 0 && c > 0 && r < max_row && c < max_col) {
                values.push_back(input_image_.get_pixel(r, c));
            }
        }
    }
    std::nth_element(values.begin(), values.begin() + values.size()/2, values.end());
    return values[values.size()/2];
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
         image_matrix * filtered_image)
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
    image_matrix * get_p_filtered_image(){return filtered_image;}
private:
    int start_x,start_y,end_x,end_y;
    int window_size;
    image_matrix original_image, * filtered_image;

};


// function run by each thread
void* func( void* arg ) 
{
  task t_arg = *( task* )arg;
    image_matrix original_image = t_arg.get_original_image();
    image_matrix * filtered_image = t_arg.get_p_filtered_image();
    int window_size = t_arg.get_window_size();
    
    for (int r=t_arg.get_start_y(); r<t_arg.get_end_y(); r++) {
        for (int c=t_arg.get_start_x(); c<t_arg.get_end_x(); c++) {
            float f_px= median_filter_pixel(original_image, r, c, window_size);
            filtered_image->set_pixel(r,c,f_px);
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
    if(!read_input_image( input_filename, input_image )){
        std::cerr << input_filename << " not found.\n";
    }

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
          t[i]=new task(0,          i*(n_rows/n_threads),   n_cols,     (i+1)*(n_rows/n_threads),   window_size,input_image,&filtered_image);
      }
      
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
