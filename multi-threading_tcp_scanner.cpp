#include <iostream>
#include <winsock.h>
#include <cstring>
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>
 
 
std::string GetIpByHost(std::string ip){
   struct hostent* host;
   struct in_addr** addr;
   host = gethostbyname(ip.c_str());
   addr = (struct in_addr**)host->h_addr_list;
   return inet_ntoa(*addr[0]);
}
 
std::map<short unsigned int, std::string> Port{
 {7,"ICMP (Ping)"},
 {80,"HTTP"},
 {21,"FTP"},
 {22,"SSH"},
 {23,"Telnet"},
 {25,"SMTP"},
 {8080,"HTTP"},
 {443,"HTTPS"},
 {123,"IMP"},
 {445,"Microsoft-DS (SMB)"},
 {139,"Windows SMB"},
 {138,"Windows SMB"},
 {137,"Windows SMB"},
 {465,"SMTP"},
 {587,"SMTP"},
 {108,"POP2"},
 {109,"POP3"},
 {156,"SQL Server"},
 {546,"DHCP Client"},
 {547,"DHCP Server"},
 {1080,"Socks"},
 {3398,"RDP"},
 {3306,"MySQL"},
 {3390,"WAN RDP"},
 {5900,"VNC"},
 {5800,"VNC (Java applet)"}
};
 
std::mutex display_mutex;
 
std::map<short unsigned int, std::string>::iterator iter;
 
void* PortScan(SOCKET sock,struct sockaddr_in structure){
    int Porte = ntohs(structure.sin_port);
    if(connect(sock,(sockaddr*)&structure,sizeof(structure)) == SOCKET_ERROR){
            if(WSAGetLastError() != WSAEWOULDBLOCK){
                 WSACleanup();
                 closesocket(sock);
                 exit(0);
            }
            fd_set fd,fdE;
            FD_ZERO(&fd);
            FD_ZERO(&fdE);
            FD_SET(sock,&fd);
            FD_SET(sock,&fdE);
            timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            if(!select(sock,0,&fd,&fdE,&tv) <= 0){
               if(Port.find(Porte) != Port.end()){
                 std::cout << "*) Port " << Porte << " is open [" << Port[Porte] << "]\n" << std::endl;
               }
               else{
                 std::cout << "*) Port " << Porte << " is open [Protocol Unknown]\n" << std::endl;
               }
            }
    }
}
 
int main(int argc, char** argv){
  if(argc != 2){
    exit(0);
  }
  std::cout << std::endl;
  std::thread ThreadStock[Port.size()];
  WSADATA ws;
  WORD ver = MAKEWORD(2,2);
  WSAStartup(ver,&ws);
  int x = 0;
  for(iter= Port.begin();iter != Port.end();iter++){
       SOCKET sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
       unsigned long Bio = 1;
       ioctlsocket(sock,FIONBIO,&Bio);
       struct sockaddr_in serv;
       memset(&serv,0,sizeof(sockaddr_in));
       serv.sin_port = htons(iter->first);
       serv.sin_family = AF_INET;
       serv.sin_addr.s_addr = inet_addr(std::string(GetIpByHost(argv[1])).c_str());
       ThreadStock[x] = std::thread(PortScan,sock,serv);
       x++;
  }
  for(int y = 0;y < Port.size();y++){
              std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(50));
              ThreadStock[y].join();
  }
 
 
}