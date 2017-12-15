//
// Created by Vovas on 27.11.2017.
//

#include "utils.h"


//auto logger = spdlog::rotating_logger_mt("clients_logger", "logthreads", 1024*1024, 1);
//auto mainlog = spdlog::rotating_logger_mt("main_logger", "log", 1024*1024, 1);

std::vector<std::pair<pthread_t, long> > clients;

std::map<int, client> clients_map;
std::map<int, item> items_map;

pthread_mutex_t vector_clients_mutex;
pthread_mutex_t map_clients_mutex;
pthread_mutex_t map_items_mutex;

std::array<std::string, COMMAND_NUM> command_list{{
//        "SEND",
                                                          "kill - delete client by number",
                                                          "killall - shutdown server",
                                                          "ls - showClients the list of connected clients",
                                                          "help - repeat the help list again"}
};

bool isManagerLogged = false;


// Getting a unique 32-bit ID is intuitively simple: the next one.
// Works 4 billion times. Unique for 136 years if you need one a second
int generateUserID() {
    static int seed = 0;
    return ++seed;
//    return ++generator_id;
}

int generateItemID() {
    static int seed = 0;
    return ++seed;
}

bool kill_client(int id) {
    pthread_mutex_lock(&vector_clients_mutex);
//    logger->info("start to kill client # {}", id);
    if (clients.size() < id) {
//        logger->info("no client # {}", id);
        pthread_mutex_unlock(&vector_clients_mutex);
        return false;
    }
    auto it = clients.begin() + (id - 1);
//    std::cout << "it->first = "<< it->first << std::endl;
    if (shutdown(it->second, SD_BOTH) == 0)
        std::cout << "success sock off client # " << it->second << std::endl;
//        logger->info("success sock off client # {}", it->second);
    else {
//        logger->error("error sock off client # {} , error = ", it->second, errno);
//        pthread_mutex_unlock(&vector_clients_mutex);
        return false;
    }
    if (closesocket(it->second) == 0)
        std::cout << "kill client:  success sock close client # " << it->second << std::endl;
//        logger->info("success sock close client # {}", it->second);
    else {
        std::cout << "kill client: error sock close client  #" << id << "errno" << errno << std::endl;
//        logger->error("error sock close client  # {}, error = ", id, errno);
//        pthread_mutex_unlock(&vector_clients_mutex);
        return false;
    }
    if (pthread_join(it->first, NULL) == 0)
        std::cout << "kill client: success thread join client # " << it->second << std::endl;
//        logger->info("success thread join client  # {}", it->second);
    else {
        std::cout << "kill client: error thread join client # " << it->second << std::endl;
//        logger->error("error thread join client  # {}", it->second);
        pthread_mutex_unlock(&vector_clients_mutex);
        return false;
    }
    clients.erase(it);
    pthread_mutex_unlock(&vector_clients_mutex);
    return true;
}

void showCommands() {
    std::cout << "List of commands is:" << std::endl;
    for (auto it = command_list.begin();
         it != command_list.end(); it++) {
        std::cout << *it << "\n";
    }

}

void showClients() {
//    pthread_mutex_lock(&vector_clients_mutex);
    if (clients.size() != 0) {
        int i = 1;
        for (auto it : clients) {
//            i++;
//            TODO:
            std::cout << "client id" << i++ << " thread: " //<< it.first
                      << " number of socket: " << it.second << std::endl;
        }
    } else
        std::cout << "no connected clients" << std::endl;
//    pthread_mutex_unlock(&vector_clients_mutex);
}

int readn(int sock, char *buff, int size, int flag) {
    int recv_size = 0;
    while (recv_size < size) {
        char temp_buff[size];
        for (int j = 0; j < size; ++j) {
            temp_buff[j] = 0;
        }
//        TODO:
        int temp_recv = recv(sock, temp_buff, size, 0);
        if (temp_recv <= 0)
            return -1;
        for (int i = 0; i < temp_recv; i++)
            buff[i + recv_size] = temp_buff[i];
        recv_size += temp_recv;
    }
    return recv_size;
}

std::string getCommandFromMsg(const char *charmsg) {
    std::string msg(charmsg);
    return msg.substr(1, 4);
}

std::string create_message(std::string role, const char *command, std::string data) {
    std::string message = "";
//    char buf[30];
//    message += itoa(role, buf, 10);
    message += role;
    message += command;
    message += data;
    message += '\0';
    std::cout << "msg on creation : " << message << std::endl;
    return message;
}

//split function as python builts-in
std::vector<std::string> split(std::string s, std::string delimiter) {
    std::vector<std::string> list;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        list.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    list.push_back(s);
    return list;
}

bool isLogged(std::string login) {
    for (auto it : clients_map)
        if (it.second.login == login)
            return true;
    return false;

}

void print_clients() {
    if (isManagerLogged)
        std::cout << "Manager is logged in" << std::endl;
    else
        std::cout << "Manager is not logged in" << std::endl;

    if (clients_map.size() == 0) {
        std::cout << "No clients connected" << std::endl;
        return;
    }
    std::cout << "Clients:" << std::endl;
    for (auto it : clients_map)
        std::cout << "id = " << it.first
                  << " login = " << it.second.login
                  << " pswd = " << it.second.password << std::endl;
}

void print_items() {
    if (items_map.size() == 0) {
        std::cout << "No items in the list" << std::endl;
        return;
    }
    std::cout << "Items:" << std::endl;
    for (auto it : items_map)
        std::cout << "id = " << it.first
                  << " price =  " << it.second.price
                  << " login = " << it.second.name
                  << " holder =  " << it.second.holder
                  << std::endl;
}

void broadcastSend(int except_sock, std::string data){
    if(except_sock != BROADCAST_ALL)
        for (auto it :clients){
            if(it.second != except_sock){
                std::string response = create_message(SERVER, BROADCAST, data);
                if (send(it.second, response.c_str(), MAX_MESSAGE_SIZE, 0) != MAX_MESSAGE_SIZE)
                    std::cout << "error send to client with id = " <<  it.second <<std::endl;
            }
        }
    else
        for (auto it :clients){
                std::string response = create_message(SERVER, BROADCAST, data);
                if (send(it.second, response.c_str(), MAX_MESSAGE_SIZE, 0) != MAX_MESSAGE_SIZE)
                    std::cout << "error send to client with id = " <<  it.second <<std::endl;
        }
}

void *sons_start(void * param){

    sleep(20);
//    std::cout << "sons start " << std::endl;
    std::string data((char *) param);
//    std::cout << "sons start  char * ok " << std::endl;
    std::vector<std::string> str_vector = split(data, " ");
    const char * bufa = str_vector[str_vector.size() - 1 ].c_str();
    int user_id = atoi(bufa);
//    std::cout << user_id << "kekekek" << std::endl;
    str_vector.pop_back();
//    std::cout << user_id << "udalili one" << std::endl;
    const char * bufa1 = str_vector[str_vector.size() - 1 ].c_str();
    int item_id = atoi(bufa1);
//    std::cout << item_id << "kekekek" << std::endl;
//    str_vector.erase(str_vector.end());
    str_vector.pop_back();
//    std::cout << user_id << "udalili dva" << std::endl;

    data = "";
//    std::cout << "sons start " << user_id << std::endl;
    if(items_map[item_id].holder_id != user_id)
        pthread_exit(NULL);

    for(auto it =  str_vector.begin(); it != str_vector.end(); ++it) {
//        if(*it.compare())
        data += *it;
        data += " ";
    }
//    std::cout << "sons " << data << std::endl;
    data += " ";
    data += APPROVE;
    broadcastSend(BROADCAST_ALL,data);

    if(items_map.erase(item_id) != 1)
        std::cout << "Error: with deleting" << std::endl;
    else
        std::cout << "OK: with deleting" << std::endl;

    pthread_exit(NULL);
}

void *run_client(void *param) {
//    SOCKET sock = (SOCKET *)param;
    boolean isThisManager = false;
    const char *exit_str = "exit";
    int i = 0;
    long sock = (long) param;
    std::vector <pthread_t> sons;
    // TODO: should be 2 cycles for authentication and for working


    while (true) {
        char Buffer[MAX_MESSAGE_SIZE];
        for (int j = 0; j < MAX_MESSAGE_SIZE; ++j) {
            Buffer[j] = 0;
        }
//        logger->info("sock number {}", sock);
//        if(recv(sock, Buffer, MAX_MESSAGE_SIZE, MSG_NOSIGNAL) == -1)
        if (readn(sock, Buffer, MAX_MESSAGE_SIZE, 0) == -1) {
            break;
        }
        std::string msg(Buffer);
        std::cout << msg.length()<<std::endl;
        if (msg.compare(1, 4, LOGIN) == 0) {
            std::string pswd;
            std::string login;
            std::cout << "len msg = " << msg.length() << std::endl;
            std::vector<std::string> log_and_pqsswd = split(msg.substr(5, MAX_MESSAGE_SIZE), " ");
            login = log_and_pqsswd[0];
            pswd = log_and_pqsswd[1];
            std::cout << "detected packet contains " << std::endl;
            std::cout << "login = " << login << " pswd = " << pswd << std::endl;
            if (msg.compare(0, 1, MANAGER) == 0) {
                // manager_name checking is actuualy useless: login == manager_name &&
                if (!isManagerLogged) {
                    if (manager_password == pswd) {
                        isManagerLogged = true;
                        isThisManager = true;
                        std::string message = create_message(SERVER, ACKNOWLEDGE, MANAGER);
                        if (send(sock, message.c_str(), MAX_MESSAGE_SIZE, 0) != MAX_MESSAGE_SIZE)
                            std::cout << "error send" << std::endl;
                        std::string data = " ";
                        broadcastSend(sock, data  + START);
                    } else {
                        std::string message = create_message(SERVER, ERROR, ERR_MANAGER_WRONG_PSWD);
                        if (send(sock, message.c_str(), MAX_MESSAGE_SIZE, 0) != MAX_MESSAGE_SIZE)
                            std::cout << "error send" << message << std::endl;

                    }
                } else {
                    std::string message = create_message(SERVER, ERROR, ERR_MANAGER_ALREADY_EXISTS);
                    if (send(sock, message.c_str(), MAX_MESSAGE_SIZE, 0) != MAX_MESSAGE_SIZE)
                        std::cout << "error send" << std::endl;
                }
            } else if (isLogged(login)) {
                std::cout << "contains" << std::endl;
                std::string message = create_message(SERVER, ERROR, ERR_USER_ALREADY_EXISTS);
                if (send(sock, message.c_str(), MAX_MESSAGE_SIZE, 0) != MAX_MESSAGE_SIZE)
                    std::cout << "error send" << std::endl;
                break;
            } else {
                client newClient;
                newClient.login = login;
                newClient.id = generateUserID();
                newClient.password = pswd;
                clients_map.insert(std::pair<int, client>(newClient.id, newClient));
                std::string data = USER;
                char buf[30];
                data += itoa(newClient.id, buf, 10);
                std::string message = create_message(SERVER, ACKNOWLEDGE, data);
                if (send(sock, message.c_str(), MAX_MESSAGE_SIZE, 0) != MAX_MESSAGE_SIZE)
                    std::cout << "error send message to user with id =  " << newClient.id << std::endl;
            }
        } else if (msg.compare(1, 4, EXIT) == 0 && msg.compare(0, 1, MANAGER) != 0) {
            std::cout << "User going to logout" << std::endl;
            std::cout << "Received message = " << msg << std::endl;
            std::string str_id = msg.substr(5, msg.size() - 5);//str_response.size() - 6);
            int id = atoi(str_id.c_str());
            if (clients_map.erase(id) == 1)
                std::cout << "Successfully remove user with id = " << id << std::endl;
            else
                std::cout << "Error remove user with id = " << id << std::endl;

        } else if (msg.compare(1, 4, NEWITEM) == 0) {
//            MANAGER, NEWITEM, item_name + " " + item_price
            std::cout << "we have a message to create new item" << std::endl;
            std::vector<std::string> name_and_price = split(msg.substr(5, MAX_MESSAGE_SIZE), " ");
            item newItem;
            newItem.id = generateItemID();
            newItem.name = name_and_price[0];
            newItem.price = atoi(name_and_price[1].c_str());
            std::cout << "detected item name " << newItem.name
                      << " item price = " << newItem.price << std::endl;
            items_map.insert(std::pair<int, item>(newItem.id, newItem));
            // TODO: send acknowledge of adding item

        } else if (msg.compare(1, 4, GETLIST) == 0) {

            std::cout << "we have a message to send list of items" << std::endl;
//            SENDLIST
            char buf[30];
            std::string data = "";
            data += itoa(items_map.size(), buf, 10);
            std::string message = create_message(SERVER, SENDLIST, data);
//                std::cout << "MSG =  " << message << std::endl;
            if (send(sock, message.c_str(), MAX_MESSAGE_SIZE, 0) != MAX_MESSAGE_SIZE)
                std::cout << "error send" << std::endl;
            int msg_number = 1;

            pthread_mutex_lock(&map_items_mutex);
            for (auto it = items_map.begin(); it != items_map.end(); it++) {
                char buf[30];
                std::string data = "";
                data += itoa(msg_number, buf, 10);
                data += " ";
                data += itoa(it->first, buf, 10);
                data += " ";
                data += it->second.name + " ";
//                char buf1[30];
                data += itoa(it->second.price, buf, 10);
                data += " ";
                data += it->second.holder;
                std::string message = create_message(SERVER, SENDLIST, data);
                if (send(sock, message.c_str(), MAX_MESSAGE_SIZE, 0) != MAX_MESSAGE_SIZE)
                    std::cout << "error send" << std::endl;
                msg_number++;
            }
            pthread_mutex_unlock(&map_items_mutex);

        } else if (msg.compare(1, 4, RISE) == 0){
            std::cout << "we get a request to rise" << std::endl;
            std::vector<std::string> item_id_and_price = split(msg.substr(5, MAX_MESSAGE_SIZE), " ");
            int user_id = atoi(item_id_and_price[0].c_str());
            int item_id = atoi(item_id_and_price[1].c_str());
            int new_price = atoi(item_id_and_price[2].c_str());
            std::cout   << "user id = " << user_id
                        << " item id = " << item_id
                        << " new price = " << new_price
                        << std::endl;
            // check for such an id in items
            pthread_mutex_lock(&map_items_mutex);
            if(items_map.find(item_id) != items_map.end()){
                // check for such an price
                if(items_map.at(item_id).price < new_price) {
                    std::string response = create_message(SERVER, ACKNOWLEDGE, " ");
                    if (send(sock, response.c_str(), MAX_MESSAGE_SIZE, 0) != MAX_MESSAGE_SIZE)
                        std::cout << "error send" << std::endl;
                    else{
                        items_map.at(item_id).price = new_price;
                        items_map.at(item_id).holder = clients_map.at(user_id).login;
                        items_map.at(item_id).holder_id = user_id;
                    }
                }else{
                    std::string response = create_message(SERVER, ERROR, ERR_ITEM_PRICE_TOO_LOW);
                    if (send(sock, response.c_str(), MAX_MESSAGE_SIZE, 0) != MAX_MESSAGE_SIZE)
                        std::cout << "error send" << std::endl;
                }
                pthread_mutex_unlock(&map_items_mutex);
                // if items holder changes
                if(items_map[item_id].holder_id != user_id)
                    continue;
                char buf[30];
                std::string data = "";
                data += clients_map[user_id].login;
                data += " ";
                data += itoa(new_price, buf, 10);
                data += " ";
                data += items_map[item_id].name;
                data += " ";
                data += itoa(item_id, buf ,10);
                data += " ";
                data += itoa(user_id, buf ,10);
                broadcastSend(sock,data);
            }else{
                std::string response = create_message(SERVER, ERROR, ERR_ITEM_WRONG_ID);
                if (send(sock, response.c_str(), MAX_MESSAGE_SIZE, 0) != MAX_MESSAGE_SIZE)
                    std::cout << "error send" << std::endl;
                pthread_mutex_unlock(&map_items_mutex);
            }

        } else if (msg.compare(1, 4, DONE) == 0){
            std::cout << "we get a request to done" << std::endl;
            std::vector < std::string > item_split= split(msg.substr(5, MAX_MESSAGE_SIZE), " ");
            int item_id = atoi(item_split[1].c_str());
//                int new_price = atoi(item_id_and_price[2].c_str());
            std::cout   << "done  item id = " << item_id
                        << std::endl;
            // check for such an id in items
            pthread_mutex_lock(&map_items_mutex);
            if(items_map.find(item_id) != items_map.end()) {
                // check for such an price
                int  user_id = items_map[item_id].holder_id;
                char buf[30];
                std::string data = "";
                data += clients_map[user_id].login;
                data += " ";
                data += items_map[item_id].name;
                data += " ";
                data += itoa(item_id, buf ,10);
                data += " ";
                data += itoa(user_id, buf ,10);
                data += " ";
                data += APPROVE;
                if(items_map.erase(item_id) == 1)
                    std::cout << "item removed" << std::endl;
                broadcastSend(sock,data);
                pthread_mutex_unlock(&map_items_mutex);
            }
//            items_map.insert(std::pair<int, item>(newItem.id, newItem));
        }
//        else if (msg.compare(1, 4, STOP) == 0){
//
//        }

    }
    if(isThisManager){
        std::string data  = " ";
        broadcastSend(sock, data + STOP);
        isManagerLogged = false;
        pthread_mutex_lock(&map_items_mutex);
        items_map.erase(items_map.begin(), items_map.end());
        pthread_mutex_unlock(&map_items_mutex);
    }
//
//    for(auto i: sons){
//        pthread_join(i, NULL);
//    }
    std::cout << "success sock off client # " << sock << std::endl;
//        logger->info("success sock off client with sock # {}", sock);
    if (closesocket(sock) == 0) {
        auto it = clients.cbegin();
        for (; it != clients.cend(); it++) {
            if (it->second == sock) {
                std::cout << "success sock delete client with sock # " << sock << std::endl;
//                    logger->info("success delete client with sock # {}", sock);
                break;
            }
        }
        if (it != clients.cend())
            clients.erase(it);
        std::cout << "success sock close client sock # " << sock << std::endl;
//            logger->info("success sock close client with sock # {}", sock);
    } else {
        std::cout << "error sock close is already closed" << sock << std::endl;
        std::cout << "error = " << WSAGetLastError() << std::endl;
//            logger->info("error sock close client  with sock # {}", sock);
    }
    std::cout << "we finish " << sock << std::endl;
//    TODO: if socket off than client remove from clients_map

//    logger->info("we finish {}", sock);
    pthread_exit(NULL);
}

void *run_server(void *param) {
    // have to init array of clients id
//    initIDArr();
    if (pthread_mutex_init(&vector_clients_mutex, NULL) != 0) {
        std::cout << "mutex init failed" << std::endl;
        pthread_exit(NULL);
    }
    if (pthread_mutex_init(&map_clients_mutex, NULL) != 0) {
        std::cout << "mutex init failed" << std::endl;
        pthread_exit(NULL);
    }    if (pthread_mutex_init(&map_items_mutex, NULL) != 0) {
        std::cout << "mutex init failed" << std::endl;
        pthread_exit(NULL);
    }
    long MainSock = (long) param;
    while (1) {
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            std::cout << "WSAStartup failed with error:" << iResult << std::endl;
//        mainlog->info("WSAStartup failed with error: {} \n", iResult);
//            return 1;
        }
        pthread_t thrd_tmp;
        long tmp_sock = accept(MainSock, NULL, NULL);
        if (tmp_sock < 0) {
//            std::cout << "((((((((((((((((((((((((((((((("<< std::endl;
            break;
        }
//        SOCKET vsocket;
        if (pthread_create(&thrd_tmp, 0, run_client, (void *) (tmp_sock)) == 0)
//        if (pthread_create(&thrd_tmp, 0, run_client, (void *) (&vsocket)) == 0)
        {
            clients.push_back(std::pair<pthread_t, long>(thrd_tmp, tmp_sock));
//            std::cout << "success sock off client # " <<  it->second << std::endl;
//            mainlog->info("New thread num is {}", tmp_sock);
        } else {
//            mainlog->info("Did not create new thread");
            return NULL;
        }
    }

    pthread_mutex_lock(&vector_clients_mutex);
    for (auto it: clients) {
        if (shutdown(it.second, SD_BOTH) == 0)
            std::cout << "success sock shutdown " << it.second << std::endl;
//            mainlog->info("success shutdown sock {}", it.second);
        else {
            std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
            std::cout << "error shutdown sock " << it.second << std::endl;
//            mainlog->error("error shutdown sock {}", it.second);
        }
        if (closesocket(it.second) == 0)
            std::cout << "success sock close # " << it.second << std::endl;
//            mainlog->info("success close sock {}", it.second);
        else
            std::cout << "error close sock # " << it.second << std::endl;
//            mainlog->error("error close sock {}", it.second);

        if (pthread_join(it.first, NULL) == 0)
            std::cout << "success join client # " << it.second << std::endl;
//            mainlog->info("success join client thread: {}  number of socket: {}", it.first, it.second);
        else
            std::cout << "error join client # " << it.second << std::endl;
//            mainlog->error("error join client thread: {}  number of socket: {}", it.first, it.second);
//        WSACleanup();
    }
    clients.clear();
    pthread_mutex_unlock(&vector_clients_mutex);
    std::cout << "server clients thread is done" << std::endl;
//    mainlog->info( "server clients thread is done");
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    pthread_t server_init;

// windows init shit
    WSADATA wsaData;
    SOCKET MainSock = INVALID_SOCKET;
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0) {
        std::cout << "WSAStartup failed with error:" << iResult << std::endl;
//        mainlog->info("WSAStartup failed with error: {} \n", iResult);
        return 1;
    }

// AI_PASSIVE flag indicates the caller intends to use the returned socket address structure in
// a call to the bind function. When the AI_PASSIVE flag is set and nodename parameter to the getaddrinfo
// function is a NULL pointer, the IP address portion of the socket address structure is set to INADDR_ANY for IPv4
// addresses or IN6ADDR_ANY_INIT for IPv6 addresses.

// https://msdn.microsoft.com/ru-ru/library/windows/desktop/bb530742(v=vs.85).aspx


    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed with error: " << iResult << std::endl;

//        mainlog->info("getaddrinfo failed with error: {}\n", iResult);
        WSACleanup();
        return 1;
    }

    MainSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (MainSock == INVALID_SOCKET) {
        std::cout << "socket failed with error " << WSAGetLastError() << std::endl;

//        mainlog->info("socket failed with error: {} \n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    } else
        std::cout << "Successful creation of socket" << std::endl;
//        mainlog->info("Successful creation of socket\n");

    // Setup the TCP listening socket
    iResult = bind(MainSock, result->ai_addr, (int) result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind failed with error" << WSAGetLastError() << std::endl;

//        mainlog->info("bind failed with error: {} \n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(MainSock);
        WSACleanup();
        return 1;
    } else
        std::cout << "Server is bind" << std::endl;
//        mainlog->info("Server is bind");


    if (listen(MainSock, SOMAXCONN) == 0)
        std::cout << "Server is ready" << std::endl;
//        mainlog->info("Server is ready");
    else
        std::cout << "Server doesnt listen" << std::endl;
//        mainlog->info("Server does not listen");

    if (pthread_create(&server_init, NULL, run_server, (void *) (MainSock)) == 0)
        std::cout << "Server is started on port: " << DEFAULT_PORT << std::endl;
    else
        std::cout << "Error: can not create server_init thread" << std::endl;
    showCommands();
    while (true) {
        std::string cmd_string;
        std::cout << "Enter your command:" << std::endl;

//        std::getline(std::cin, cmd_string);
        std::cin >> cmd_string;
        if (cmd_string.compare("help") == 0) {
            showCommands();
        } else if (cmd_string.compare("kill") == 0) {
            int client_to_del;
            if (clients.size() > 0) {
//                while(true){
                std::cout << "Input number of client:" << std::endl;
                std::cin >> client_to_del;
                if (client_to_del <= 0)
                    std::cout << "WRONG INPUT: client number can not be negative or zero!" << std::endl;
                else if (client_to_del > clients.size())
                    std::cout << "WRONG INPUT: " << clients.size() << " clients are connected" << std::endl;
                else {
                    if (kill_client(client_to_del) == true) {
                        std::cout << "client removed succesfully" << std::endl;
                        continue;
                    } else
                        std::cout << "error remove client see logthread.txt" << std::endl;
                    break;
                }
            } else
                std::cout << "no clients to remove" << std::endl;

        } else if (cmd_string.compare("killall") == 0) {
            std::cout << "The server is turning off...\n";
//            if(shutdown(MainSock, NULL) == 0)
//                std::cout << "shutdown main sock\n";
//            else
//            {
//                printf("shutdown failed with error: %d\n", WSAGetLastError());
// //                closesocket(MainSock);
// //                return 1;
//            }
            if (closesocket(MainSock) != SOCKET_ERROR)
                std::cout << "close main sock\n";
            else
                std::cout << "error close main sock" << std::endl;
            WSACleanup();

            break;
        } else if (cmd_string.compare("ls") == 0) {
            std::cout << "All the clients connected to the server are:" << std::endl;
//            showClients();
            print_clients();
            print_items();
        } else
            std::cout << "No such a command:  " << cmd_string << "  ! Retry!" << std::endl;

    }

    if (pthread_join(server_init, NULL) == 0)
        std::cout << "Join is done # " << "\nBye!\n";

    WSACleanup();
    return 0;
}