#include <iostream>

#include <sys/socket.h>
#include <sys/un.h>

const char * get_s(int entry){
    switch(entry){
        case 1:
            return "()";
        case 2:
            return "><";
        default:
            return "  ";
    }
}

void print_ascii_board(int * board){
    std::cout<< "+--+--+--+\n";
    std::cout<< "|"<< get_s(board[0]) << "|" << get_s(board[1]) << "|" << get_s(board[2]) << "|\n";
    std::cout<< "+--+--+--+\n";
    std::cout<< "|"<< get_s(board[3]) << "|" << get_s(board[4]) << "|" << get_s(board[5]) << "|\n";
    std::cout<< "+--+--+--+\n";
    std::cout<< "|"<< get_s(board[6]) << "|" << get_s(board[7]) << "|" << get_s(board[8]) << "|\n";
    std::cout<< "+--+--+--+\n";

}

void print_message(int status, int player_number){
    switch(status){
        case 1:
            printf("\n\n\n\n\n\n\n\n\n\nYour turn!\nEnter your choice: \n  0  1  2\n  3  4  5\n  6  7  8\n");
            break;
        case 2:
            printf("\n\n\n\n\n\n\n\n\n\nYour opponent's turn.\n");
            break;
        case 3:
            printf("\n\n\n\n\n\n\n\n\n\nYou've Won!\n");
            break;
        case 4:
            printf("\n\n\n\n\n\n\n\n\n\nYou've Lost!\n");
            break;
        case 5:
            printf("\n\n\n\n\n\n\n\n\n\nIt's a Tie.\n");
            break;
    }
    switch(player_number){
        case 1:
            printf("You are ()\n");
            break;
        case 2:
            printf("You are ><\n");
            break;
    }
}

bool is_invalid_input(int *board, const int input){
    return board[input] != 0 ? true : false;
}

void start_game(const int & socket, const int & player_number){
    /*
     * start game
     * status flag:
     *  1: your turn
     *  2: other players turn
     *  3: won
     *  4: lost
     *  5: tie
     */
    int * buf = new int[10];
    do{
        read(socket,buf,sizeof(buf)*10);

        print_message(buf[9],player_number);
        print_ascii_board(buf);

        if(buf[9]==1){
            int input;
            scanf("%d",&input);
            while(is_invalid_input(buf, input)){
                printf("Please select an empty field!\n");
                scanf("%d",&input);
            }
            write(socket,&input,sizeof(input));
        }
    }while(buf[9] < 3);
}

int main(int argc,char * argv[])
{
    int fd_c;
    struct sockaddr_un addr_s;
    socklen_t addr_s_len; uint

    // ***   create socket   ***

    fd_c = socket( AF_UNIX, SOCK_STREAM, 0 );
    if( fd_c < 0 )
    {
        std::cerr << "Error in socket()" << std::endl;
        return -1;
    }


    // ***   establish connection   ***

    addr_s.sun_family = AF_UNIX;
    strcpy( addr_s.sun_path, argv[1] );
    addr_s_len = sizeof( addr_s );
    if( connect( fd_c, ( struct sockaddr* )&addr_s, addr_s_len )  <  0 ) 
    {
        std::cerr << "Error in connect()" << std::endl;
        return -1;
    }

    int player_number;

    read(fd_c,&player_number,sizeof(player_number));
    printf("My player number: %d\n",player_number);
    start_game(fd_c,player_number);

    close( fd_c );

    return 0;
}
