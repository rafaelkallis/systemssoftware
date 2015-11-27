#include <iostream>
#include <vector>

pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

bool run = true;

void alloc_request_print(const int & _id, const int & _n_blocks){
    pthread_mutex_lock(&print_lock);
    std::cout << "Process " << _id << " has requested " << _n_blocks << " blocks"<< std::endl;
    pthread_mutex_unlock(&print_lock);
}

void alloc_result_print(const int & _id, const bool & _succ){
    pthread_mutex_lock(&print_lock);
    std::cout << "Process " << _id << " allocation was " << (_succ ? "successful" : "rejected") << std::endl;
    pthread_mutex_unlock(&print_lock);
}

void sleep_msg(const int & _id, const timespec & _ts){
    pthread_mutex_lock(&print_lock);
    std::cout << "Process " << _id << " is going to sleep for " << _ts.tv_sec << " s and " << _ts.tv_nsec / 1000000 << " ms" << std::endl;
    pthread_mutex_unlock(&print_lock);
}

void wakeup_msg(const int & _id){
    pthread_mutex_lock(&print_lock);
    std::cout << "Process " << _id << " wope up" << std::endl;
    pthread_mutex_unlock(&print_lock);
}

void termination_msg(){
    pthread_mutex_lock(&print_lock);
    std::cout << " * * * Waiting for all processes to finish and then terminating * * *" << std::endl;
    pthread_mutex_unlock(&print_lock);
}

class memory{
protected:
    std::vector<int> blocks;
    pthread_mutex_t lock;
    int blocks_cap;
    int blocks_used;

public:
    memory(int _blocks_cap){
        blocks = std::vector<int>((unsigned long)_blocks_cap,-1);
        this->lock = PTHREAD_MUTEX_INITIALIZER;
        this->blocks_cap = _blocks_cap;
        this->blocks_used = 0;
    }

    ~memory(){
      pthread_mutex_destroy(&lock);
    }

    /*
     * @returns true if allocation was successful
     */
    bool allocate( int n_blocks, const int & thread_id){
        bool result = false;
        pthread_mutex_lock(&lock);
        alloc_request_print(thread_id,n_blocks);
        if(blocks_cap - blocks_used >= n_blocks){
            for(int idx = 0; idx < blocks_cap && n_blocks > 0; idx++){
                if(blocks[idx] == -1){
                    blocks[idx] = thread_id;
                    n_blocks--;
                    blocks_used++;
                }
            }
            result = true;
        }
        alloc_result_print(thread_id,result);
        pthread_mutex_unlock(&lock);
        return result;
    }

    bool release(const int & thread_id){
        bool return_value = false;
        pthread_mutex_lock(&lock);
        for(int idx = 0; idx < blocks_cap; idx++){
            if(blocks[idx] == thread_id){
                blocks[idx] = -1;
                blocks_used--;
                return_value = true;
            }
        }
        pthread_mutex_unlock(&lock);
        return return_value;
    }
};

struct thread_param
{
    int id;
    int t_min;
    int t_max;
    int w_min;
    int w_max;
    memory * mem = NULL;

    thread_param(int _id, int _t_min, int _t_max, int _w_min, int _w_max, memory * _mem){
        this->id    = _id;
        this->t_min = _t_min;
        this->t_max = _t_max;
        this->mem   = _mem;
        this->w_min = _w_min;
        this->w_max = _w_max;
    }
};


int generate_random_int(int _min, int _max){
    return _min + rand() % ( _max - _min);
}

void sleep(int _t_min, int _t_max, int _id){
    int t =generate_random_int(_t_min,_t_max);
    struct timespec ts;
    ts.tv_sec = t / 1000;
    ts.tv_nsec = ( t % 1000 ) * 1000000;

    sleep_msg(_id,ts);

    nanosleep( &ts, NULL );

    wakeup_msg(_id);
}

void sleep_w(int _t_min, int _t_max, int w, int _id){
    int w_t_max = w * _t_max;
    sleep(_t_min,w_t_max,_id);
}

void* thread_func( void* arg ) 
{
    thread_param tp = *( thread_param* )arg;
    do
    {
        int w = generate_random_int(tp.w_min,tp.w_max);
        if (tp.mem->allocate(w,tp.id)){
            sleep_w(tp.t_min,tp.t_max,w,tp.id);
            tp.mem->release(tp.id);
        }
        sleep(tp.t_min,tp.t_max,tp.id);
    }
    while( run );

    return NULL;
}

int main( int argc, char* argv[] )
{
    if( argc < 7 )
    {
      std::cerr << "Not enough arguments provided. Terminating." << std::endl;
      exit( 1 );
    }

    srand( time( NULL ) );   // uncomment to actually get pseudo-random numbers

    // get command-line parameters
    int p = std::stoi( argv[ 1 ] ); // P threads
    int b = std::stoi( argv[ 2 ] ); // B blocks
    int w_min = std::stoi( argv[ 3 ] ); // W blocks of memory
    int w_max = std::stoi( argv[ 4 ] );
    int t_min = std::stoi( argv[ 5 ] ); // T miliseconds
    int t_max = std::stoi( argv[ 6 ] );

    memory mem(b);
    int id_count = 1;
    thread_param * param[p];

    pthread_t threads[p];

    for(int param_idx = 0; param_idx < p; param_idx++){
        thread_param * new_param = new thread_param(id_count++,t_min,t_max, w_min, w_max, &mem);
        param[param_idx] = new_param;
    }

    for(int param_idx = 0; param_idx < p; param_idx++){
        pthread_create(&threads[param_idx],NULL,thread_func,(void *)param[param_idx]);
    }

    // wait for 'e' key
    char c = '1';
    do
    {
        std::cin >> c;
    }
    while( c != 'e' );


    // stop threads and exit
    run = false;
    termination_msg();
    for(int idx = 0; idx < p; idx++){
        pthread_join(threads[idx],NULL);
    }

    return 0;
}
