//
// Created by Vovas on 07.12.2017.
//


#include <iostream>
#include <map>
#include <vector>
#include <pthread.h>
// #include <winsock2.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>


#define DEFAULT_PORT "27015"

// roles
#define MANAGER         "M"
#define USER            "U"
#define SERVER          "S"

// comands

#define COMMAND_LEN 4
#define ERROR           "eror"
#define EXIT            "exit"
#define LOGIN           "logi"
#define GETLIST         "getl"
#define RISE            "rise"
#define PASS            "pass" // &????????????????/   do we actually need it
#define SHUTDOWN        "shut"
#define STOP            "stop"
#define NEWITEM         "newi"
#define DELETEITEM      "deli"
#define SENDLIST        "sndl"
#define APPROVE         "aprv"
#define ACKNOWLEDGE     "ackn"
#define DENY            "deny"

// errors
#define ERR_MANAGER_ALREADY_EXISTS  "MAE"
#define ERR_MANAGER_WRONG_PSWD      "WMP"
#define ERR_USER_ALREADY_EXISTS     "UAE"


// #include "utils.h"

const int MAX_MESSAGE_SIZE = 256;
const int MAX_PASSWORD_SIZE = 100;
const int MAX_login_SIZE = 100;
const int MAX_IT_NAME_SIZE = 100;
const int MAX_IT_PRICE_SIZE = 4;

const int  COMMAND_NUM = 4;
const int ITEM_LOGIN_LEN = 100;
const int HOLDER_login_LEN = 100;
const char * manager_name  = "manager";
const char *manager_password = "1234";



// struct  item
// {
//     char login
// }

std::array<std :: string, COMMAND_NUM> command_list{{
        "exit",
        "rise",
        "getlist",
        "help - repeat the help list again"}
};

// std::array<std :: string, COMMAND_NUM> command_list_manager{{
//         "exit",
//         "additem",
//         "stop",
//         "getlist",
//         "help - repeat the help list again"}
// };




int readn(int socket, char *message, size_t length, int flags) {
    int received = 0;
    while (received < length) {
        char buffer[length];
        int size = recv(socket, buffer, length, flags);
        if (size <= 0) return size;
        for (int i = 0; i < size; i++)
            message[i + received] = buffer[i];
        received += size;
    }
    return received;
}

std::string create_message(const char * role, const char * command, std::string data) {
    std::string message = "";
    message += role;
    message += command;
    message += data;
    message += '\0';
    return message;
}

std::vector<std::string> split(std::string s, std::string delimiter) { //Разбивает строку по делиметру
    std::vector<std::string> list;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) { //пока не конец строки
        token = s.substr(0, pos);
        list.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    list.push_back(s);
    return list;
}


void user_dialogue(int id, int Socket){
    do {
        char Buffer[MAX_MESSAGE_SIZE];
        std::string request;
        std::cout << "Input your request:" << std::endl;
        // std::getline(std::cin, request);
        std::cin >> request;
        if(request.compare("exit") == 0){
            std::cout << "You are trying to logging out" << std::endl;
            std::string message = create_message(USER, EXIT, std::to_string(id));
            if(send(Socket, message.c_str(), MAX_MESSAGE_SIZE, MSG_NOSIGNAL) < MAX_MESSAGE_SIZE){
                std::cout << "error sending message" << std::endl;
                // return 1;
            }
            shutdown(Socket, SHUT_RDWR);
            close(Socket); 
            break;
        }
        else if(request.compare("rise") == 0){
            std::string lot_number;
            std::cout << "Input lot number" << std::endl;
            // std::getline(std::cin, lot_number);
            // send request for rise with this number 
        }
        else if(request.compare("getlist") == 0){
            std::string message = create_message(USER, GETLIST, " ");
            if(send(Socket, message.c_str(), MAX_MESSAGE_SIZE, MSG_NOSIGNAL) < MAX_MESSAGE_SIZE){
                std::cout << "error sending message" << std::endl;
                // return 1;
            }
            std::cout << "Waiting for the server" << std::endl;
            char response[MAX_MESSAGE_SIZE];
            if(readn(Socket, response, MAX_MESSAGE_SIZE, MSG_NOSIGNAL) == MAX_MESSAGE_SIZE) {
                std::string str_response = response;
                std::string size = str_response.substr(5, str_response.size());
                int size_of_list = atoi(size.c_str());
                std::cout << "size_of_list = " << size_of_list << std::endl;
                for(int i = 0; i < size_of_list; i++)
                {
                    char item_buf[MAX_MESSAGE_SIZE];
                    if(readn(Socket, item_buf, MAX_MESSAGE_SIZE, MSG_NOSIGNAL) == MAX_MESSAGE_SIZE) {
                        std::string str_item_buf = item_buf;
                        std::vector<std::string> item_vector = split(str_item_buf.substr(5, MAX_MESSAGE_SIZE), " ");

                        std::cout   << "msg # " << item_vector[0]
                                    << " id = " << item_vector[1]
                                    << " price =  " << item_vector[2]
                                    << " name = " << item_vector[3]
                                    << " holder =  " << item_vector[4]
                                    << std::endl;



                    }else{
                        std::cout << "Error while reading items from server" << std::endl;
                    }
                }
            }else{
                std::cout << "Error with message from server" <<std::endl;
            }
        }
        else if(request.compare("help") == 0){
            std::cout << "Here it is a list of commands" << std::endl;
        }
        else {
            std::cout << "Uknown request! Retry!" << std :: endl; 
        }
    } while (true);
}

void manager_dialogue(int Socket){
    do {
        char Buffer[MAX_MESSAGE_SIZE];
        std::string request;
        std::cout << "Input your request:" << std::endl;

        // std::getline(std::cin, request);
        std::cin >> request;
        if(request.compare("exit") == 0){
            std::cout << "You are trying to logging out" << std::endl;
            std::string message = create_message(MANAGER, EXIT,"");
            if(send(Socket, message.c_str(), MAX_MESSAGE_SIZE, MSG_NOSIGNAL) < MAX_MESSAGE_SIZE){
                std::cout << "error sending message" << std::endl;
                // return 1;
            }
            shutdown(Socket, SHUT_RDWR);
            close(Socket); 
            break;
        }
        else if(request.compare("additem") == 0){
            
            std::string item_name;
            std::string item_price;
            
            std::cout << "Input item name" << std::endl;
            std::cin >> item_name;
            if(item_name.size() > MAX_IT_NAME_SIZE) {
                std::cout << "Item Name should be not more than symbols" 
                          << MAX_IT_NAME_SIZE << std::endl;
                continue;
            }

            std::cout << "Input item begin price" << std::endl;
            std::cin >> item_price;
            if(item_name.size() > MAX_IT_NAME_SIZE){
                std::cout << "Item price should be not more than symbols" 
                          << MAX_IT_NAME_SIZE << std::endl;
                continue;  
            } 
            // std::string data;
            // data = item_name;
            // data += " ";
            // data += item_price;
            std::string message = create_message(MANAGER, NEWITEM, item_name + " " + item_price);
            if(send(Socket, message.c_str(), MAX_MESSAGE_SIZE, MSG_NOSIGNAL) < MAX_MESSAGE_SIZE){
                std::cout << "error sending message" << std::endl;
                // return 1;
            }
            // send request for rise with this number 
        }else if(request.compare("stop") == 0){
         
            std::cout << "Waiting for the server" << std::endl;
            
        }
        else if(request.compare("help") == 0){
            std::cout << "Here it is a list of commands" << std::endl;
            

        }
        else {
            std::cout << "Uknown request! Retry!" << std :: endl; 
        }
    } while (true);
}


int main(int argc, char **argv) {
    int Socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(27015);
    SockAddr.sin_addr.s_addr =  inet_addr("10.0.2.2");
    connect(Socket, (struct sockaddr *) (&SockAddr), sizeof(SockAddr));
    std::string s;
    long i;
    int tmp_size = 0;
    // logging in
    std::string login;
    std::string password;
    while(login.size() <= 0 && login.size() <= MAX_login_SIZE){
        std::cout << "Input your login:" <<std::endl;
        std::cin >> login;
    }
    while(password.size() <= 0 && password.size() <= MAX_PASSWORD_SIZE){
        std::cout << "Input your password:" << std::endl;
        std::cin >> password;
    }

    std::cout << "Checking on server..." << std::endl;
    // create message and send to the server
    std::string del = " ";
    std::string message;
    if(login.compare(manager_name) == 0){
        std::cout << "You are trying to logging as a manager" << std::endl;
        message = create_message(MANAGER, LOGIN, login + del + password);
    } else{
        message = create_message(USER, LOGIN, login + del + password);   
    }
    // std::cout << data << std::endl;
    // std::cout << message << std::endl;
    if(send(Socket, message.c_str(), MAX_MESSAGE_SIZE, MSG_NOSIGNAL) < MAX_MESSAGE_SIZE){
    // if(send(Socket, message, strlen(message) + 1, MSG_NOSIGNAL) < MAX_MESSAGE_SIZE)
        std::cout << "error sending message" << std::endl;
        return 1;
    }
    // get message from the server
    char response[MAX_MESSAGE_SIZE];
    // std:: cout << "len of message = " << strlen(message) << std::endl;
    if(readn(Socket, response, MAX_MESSAGE_SIZE, MSG_NOSIGNAL) == MAX_MESSAGE_SIZE) {
        std::string str_response = response;
        if(str_response.compare(1,4,ACKNOWLEDGE) == 0){
            if(str_response.compare(5, 1, USER) == 0){    
                // TODO: get user id  substr 
                std::cout << "str size = " << str_response.size() << std::endl;
                // size_t  l = str_response.size() - 6;
                std::string str_id= str_response.substr(6, str_response.size() - 6);//str_response.size() - 6);
                int id = atoi(str_id.c_str());
                std::cout << "You are connected as user with id = "<< id << std::endl;
                user_dialogue( id, Socket);
            }else{
            
                std::cout << "You are connected as MANAGER" << std::endl; 
                // TODO:
                manager_dialogue(Socket);
            }
        }
        else if(str_response.compare(1,4,ERROR) == 0){
            
            if(str_response.compare(5, 3, ERR_MANAGER_WRONG_PSWD) == 0)
                std::cout << "Wrong password for MANAGER" << std::endl;
            else if(str_response.compare(5, 3, ERR_MANAGER_ALREADY_EXISTS) == 0) 
                std::cout << "MANAGER is already logged in" << std::endl; 
            else if(str_response.compare(5, 3, ERR_USER_ALREADY_EXISTS) == 0) 
                std::cout << "USER is already logged in" << std::endl; 
            else
                std::cout << "Uknown error from server" << std::endl; 
        }
        else{
            std::cout << "Uknown error from server" << std::endl; 
        }
    }
    else{
        std::cout << "Server wrong response" <<  response << std::endl;        
    }


    // shutdown(Socket, SHUT_RDWR);
    // close(Socket); 

    std::cout << "Bye!" << std::endl;
    return 0;
}
