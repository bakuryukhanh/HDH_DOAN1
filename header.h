
#ifndef _header_h
#define _header_h

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>

#define isPipe 1
#define isOutputRedirect 2
#define isInputRedirect 3
#define isSimpleCommand 4
// bien toan cuc luu cau lenh truoc nhat
char *history;

//dem so phan tu trong mang con tro tokens

int numOfToken(char **tokens);

// xoa cac ky tu " \n\t\r\a" o dau hoac cuoi chuoi str
// viet chu yeu dung de xoa ki tu & cuoi cau trong chuoi (neu co gan c vao thi xoa ki tu do o cuoi chuoi)
void formatInput(char *str, char c);

// ham kiem tra co dau & o cuoi cau khong
bool isAmpersand(char **input);

// nhan vao va tra ve cau lanh cua nguoi dung nhap vao
// luu cau lenh vao history neu khong phai la "!!"
char* takeInput();

#define TOKEN_BUFSIZE 41
#define DELIM " \t\r\n\a"
//ham tach cau lenh thanh token theo khoang trang DELIM
char **tokenizeInput(char *line);

#define pipeRedirect "|<>"
//ham tach cau lenh ra 2 ve, ben trai va phai cua |<>
char **parsePipe(char *line);

// ham thuc thi lenh binh thuong
void execArgv(char **parse);

//ham thuc thi lenh chuyen huong output
void redirectOutput(char **command, char **fileName);

//ham thuc thi lenh chuyen huong input
void redirectInput(char **command, char **fileName);

//ham thuc thi pipe
void execPipe(char **argv1, char **argv2);

/* Ham kiem tra cau lenh nhap vao line
-neu cau lenh nhap vao la PIPE, argv1 se luu nhung token o ben trai cua |, argv2 luu token ben phai
-neu la REDIRECT, argv1 luu token o ben trai cua (</>), ben phai la argv2 (ten file)
-con neu cau lenh binh thuong, luu tat ca vao argv1, argv2 = null
*/
int checkInput(char *line, char ***argv1, char ***argv2);

//tien trinh chinh cua chuong trinh
void loop();

#endif // !header_h