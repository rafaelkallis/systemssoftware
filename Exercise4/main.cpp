#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <math.h>

#include <omp.h>

#include "image_matrix.hpp"
#include "timetracker.h"

#define X (0)
#define Y (1)

float median_filter_pixel(const image_matrix& input_image_,
                          const int& r_,
                          const int& c_,
                          const int& window_size_ )
{
    std::vector<float> values;
    float median = 0;
    const int floor_window = floor(window_size_/2);
    const int window_min_row = std::max(0,                          r_ - floor_window);
    const int window_min_col = std::max(0,                          c_ - floor_window);
    const int window_max_row = std::min(input_image_.get_n_rows()-1,r_ + floor_window);
    const int window_max_col = std::min(input_image_.get_n_cols()-1,c_ + floor_window);
    
    for (int r = window_min_row; r <= window_max_row; r++) {
        for (int c = window_min_col; c <= window_max_col; c++) {
            values.push_back(input_image_.get_pixel(r, c));
        }
    }
    std::sort(values.begin(), values.end());
    const int size = values.size();
    if (size % 2 ==0)
    {
        median = (values[size/2-1]+values[size/2])/2;
    }
    else
    {
        median = values[values.size()/2];
    }
    return median;
}

void median_filter_image(const image_matrix & input_image_,
                               image_matrix & filtered_image_,
                         const int window_size_,
                         const int start_[2],
                         const int end_[2])
{
    for (int r = start_[Y]; r < end_[Y]; r++) {
        for (int c = start_[X]; c <end_[X]; c++) {
            filtered_image_.set_pixel(r, c, median_filter_pixel(input_image_, r, c, window_size_));
        }
    }
}


void median_filter_images( const std::vector< image_matrix >& input_images_,
			   std::vector< image_matrix >& output_images_,
			   const int window_size_,
                     int n_threads_,
			   const int mode_ )
{
    n_threads_ = mode_ == 0 ? 1 : (mode_ == 1 ? std::min((int)input_images_.size(), n_threads_) : n_threads_);
    if (mode_ == 0) {
        for (int i=0; i < input_images_.size(); i++) {
            int start[2] =  {0,0};
            int end[2]   =  {input_images_[i].get_n_cols()-1,
                input_images_[i].get_n_rows()-1};
            median_filter_image(input_images_.at(i),
                                output_images_.at(i),
                                window_size_,
                                start,
                                end);
            
        }
    }else if(mode_ == 1){
        #pragma omp parallel for default(shared) num_threads(n_threads_)
        for (int i=0; i < n_threads_; i++) {
            int start[2] =  {0,
                            0};
            int end[2]   =  {input_images_[i].get_n_cols(),
                            input_images_[i].get_n_rows()};
            
            median_filter_image(input_images_.at(i),
                                output_images_.at(i),
                                window_size_,
                                start,
                                end);
        }
    }else if (mode_ == 2){
        for (int i=0; i < input_images_.size(); i++) {
            #pragma omp parallel for default(shared) num_threads(n_threads_)
            for (int t=0; t < n_threads_; t++) {
                int start[2]=   {0,
                                t*(input_images_[i].get_n_rows()/n_threads_)};
                int end[2]=     {input_images_[i].get_n_cols(),
                                (t+1)*(input_images_[i].get_n_rows()/n_threads_)};

                median_filter_image(input_images_[i],
                                    output_images_.at(i),
                                    window_size_,
                                    start,
                                    end);
            }
        }
    }
}




bool read_input_image( const std::string& filename_, image_matrix& image_in_ )
{
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



bool write_filtered_image( const std::string& filename_, const image_matrix& image_out_ )
{
  bool ret = false;
  std::ofstream os( filename_.c_str() );
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
    const int window_size = atoi( argv[ 1 ] );
    const int n_threads = atoi( argv[ 2 ] );
    const int mode = atoi( argv[ 3 ] );

    const int input_images_count = argc - 4;
    std::vector< std::string > filenames;
    for( size_t f = 0; f < input_images_count; f++ )
    {
        filenames.push_back( argv[ 4 + f ] );
    }

    // input and filtered image matrices
    std::vector< image_matrix > input_images;
    std::vector< image_matrix > filtered_images;
    input_images.resize( input_images_count );
    filtered_images.resize( input_images_count );

    // read input matrices
    for( int i = 0; i < input_images_count; i++ )
    {
        read_input_image( filenames[ i ], input_images[ i ] );

        // resize output matrix
        int n_rows = input_images[ i ].get_n_rows();
        int n_cols = input_images[ i ].get_n_cols();

        filtered_images[ i ].resize( n_rows, n_cols );
    }

    // ***   start actual filtering   ***
  
    if( mode < 3 )             // serial, parallel at image level or parallel at pixel level
    {
        median_filter_images(input_images, filtered_images, window_size, n_threads, mode);
    
        for (int i = 0; i < input_images_count; i++) {
            write_filtered_image("OUT_" + filenames[i], filtered_images[i]);
        }
    }
    else if( mode == 3 )       // benchmark mode
    {
		 Timetracker serial,parallel_image,parallel_pixel;

		  std::cout << "    * * * Running Benchmark * * *\n /————————––———————————————————————\\\n";

		 // Serial Test
		 serial.start();
		 std::cout << " |Serial execution:         ";
		 median_filter_images(input_images, filtered_images, window_size, n_threads, 0);
		 std::cout << serial.stop() << " ms|\n";

		 // Parallel Image Test
		 parallel_image.start();
		 std::cout << " |Parallel Image execution: ";
		 median_filter_images(input_images, filtered_images, window_size, n_threads, 1);
		 std::cout << parallel_image.stop() << " ms|\n";

		 // Parallel Pixel Test
		 parallel_pixel.start();
		 std::cout << " |Parallel Pixel execution: ";
		 median_filter_images(input_images, filtered_images, window_size, n_threads, 2);
		 std::cout << parallel_pixel.stop() << " ms|\n";
		 std::cout << " |Total execution:          " << serial.check() + parallel_image.check() + parallel_pixel.check() <<" ms|\n";
		 std::cout << " \\————————––———————————————————————/\n";

    }
    else
    {
        std::cerr << "Invalid mode. Terminating" << std::endl;
        return 1;
    }

    return 0;
}
