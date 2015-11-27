#include <iostream>

#include <sys/socket.h>
#include <sys/un.h>

void send(const int socket, int * board, int * status){
    int * buf = new int[10];
    memcpy(buf, board, 9*sizeof(int));
    memcpy(buf+9,status,sizeof(int));
    write(socket,buf,sizeof(buf)*10);
}

void play(const int socket, int * board, int player_number){
    int status = 1;
    int input;
    send(socket, board, &status);
    read(socket,&input,sizeof(input));
    board[input] = player_number;
}

void wait(const int socket,  int * board){
    int status = 2;

    send(socket, board, &status);
}

void won(const int socket,  int * board){
    int status = 3;

    send(socket, board, &status);
}

void lost(const int socket,  int * board){
    int status = 4;

    send(socket, board, &status);
}

void tie(const int socket,  int * board){
    int status = 5;

    send(socket, board, &status);
}

int check_for_victory( int * board){
    for(int p = 1; p < 3; p++){
        if((board[0] == p && board[1] == p && board[2] == p) || (board[3] == p && board[4] == p && board[5] == p) || (
                board[6] == p && board[7] == p && board[8] == p) || (board[0] == p && board[3] == p && board[6] == p) || (
                   board[1] == p && board[4] == p && board[7] == p) || (board[2] == p && board[5] == p && board[8] == p) || (
                   board[0] == p && board[4] == p && board[8] == p) || (board[6] == p && board[4] == p && board[2] == p)){
            return p;
        }
    }
    return 0;
}

void start_game(const int & socket1, const int & socket2){
    int board[9] = {0,0,0,0,0,0,0,0,0};
    int player_number;

    player_number = 1;
    write(socket1, &player_number, sizeof(player_number));
    player_number++;
    write(socket2, &player_number, sizeof(player_number));

    for(int round = 0;round < 6;round++ ){
        wait(socket2,board);
        play(socket1,board,1);

        if(check_for_victory(board)){
            won(socket1,board);
            lost(socket2,board);
            break;
        }

        if(round == 4){
            tie(socket1,board);
            tie(socket2,board);
            break;
        }

        wait(socket1,board);
        play(socket2,board,2);
        if(check_for_victory(board)){
            lost(socket1,board);
            won(socket2,board);
            break;
        }
    }


}

//TODO: socket location as arg
int main(int argc,char * argv[])
{
    unlink( argv[1] );

    // server socket
    int fd_s;
    struct sockaddr_un addr_s;
    socklen_t addr_s_len;

    // client sockets
    int fd_c1;
    struct sockaddr_un addr_c1;
    socklen_t addr_c1_len;

    int fd_c2;
    struct sockaddr_un addr_c2;
    socklen_t addr_c2_len;


    // ***   create socket   ***

    fd_s = socket( AF_UNIX, SOCK_STREAM, 0 );
    if( fd_s < 0 ){
    std::cerr << "Error in socket()" << std::endl;
    return -1;
    }

    // ***   set address and bind socket   ***

    addr_s.sun_family = AF_UNIX;
    strcpy( addr_s.sun_path, argv[1] );
    addr_s_len = offsetof( struct sockaddr_un, sun_path ) + strlen( addr_s.sun_path );

    if( bind( fd_s, ( struct sockaddr* )&addr_s, addr_s_len ) < 0 )
    {
    std::cerr << "Error in bind()" << std::endl;
    return -1;
    }


    // ***   listen for connections   ***

    if( listen( fd_s, 1 ) < 0 )
    {
    std::cerr << "Error in listen()" << std::endl;
    return -1;
    }


    // ***   accept an incoming connection   ***

    addr_c1_len = sizeof( struct sockaddr_un );
    fd_c1 = accept(fd_s, ( struct sockaddr* )&addr_c1, &addr_c1_len);

    addr_c2_len = sizeof( struct sockaddr_un );
    fd_c2 = accept(fd_s, (struct sockaddr*)&addr_c2, &addr_c2_len);

    if(fd_c1 < 0 || fd_c2 < 0)
    {
    std::cerr << "Error in accept()" << std::endl;
    return -1;
    }

    start_game(fd_c1,fd_c2);

    // ***   close sockets   ***

    close(fd_c1);
    close(fd_c2);
    close( fd_s );

    unlink( argv[1] );

    return 0;
}
