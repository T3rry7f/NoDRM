#include <iostream>
// #include <string>

//#include "Socket.h"
//#include <process.h>
#include "codelift.h"

using namespace std;

// int main(int argc,char **argv)
// {
//     // cout << "input:" << argv[1] << endl;
//     const char *result;
//     string flag = argv[1];
//     string input = argv[2];
//     const char *p = input.c_str();
//     // cout << "Hello, world!" << endl;
//     if (flag == "guessInput"){
//         result = guessInput(p);
//     }
//     else{
//         result = getDeoaep(p);
//     }
    
//     cout << result << endl;
//     // cout << "Hello, world!" << endl;
//     return 0;
// }

unsigned __stdcall Answer(void* a) {
    // Socket* s = (Socket*) a;
    // std::string flag = "guessInput";
    // while (1) {
    //     std::string msg = s->ReceiveLine();
    //     // 去除末尾的换行符
    //     msg = msg.substr(0, msg.size() - 1);
    //     // 以|分割得到要调用的方法名和具体参数
    //     int pos = msg.find_first_of('|');
    //     std::string func_name = msg.substr(0, pos);
    //     std::string func_arg = msg.substr(pos + 1);
    //     const char *result = "err";
    //     // 调用函数
    //     if(strcmp(func_name.c_str(), flag.c_str()) == 0){
    //         result = guessInput(func_arg.c_str());
    //     }
    //     else{
    //         result = getDeoaep(func_arg.c_str());
    //     }
    //     if (msg.empty()){
    //         break;
    //     }
    //     s->SendLine(result);
    // }
    // delete s;
    // return 0;
}

int main(int argc,char **argv)
{
    // std::string::size_type sz;
    // int port = std::stoi(argv[1], &sz);
    // cout << "listen at:" << port << endl;
    // SocketServer in(port, 10);
    // while (1) {
    //     Socket* s=in.Accept();
    //     unsigned ret;
    const char *result = "err";
     result = guessInput("xxxxxxxxxx");
     printf("%x",result);
    //     _beginthreadex(0,0,Answer,(void*) s,0,&ret);
    // }
    return 0;
}