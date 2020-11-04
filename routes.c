
#include "header.h"

// ham thuc thi lenh binh thuong
void execArgv(char **parse)
{
    bool existAmpersand = isAmpersand(parse);

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        execvp(parse[0], parse);
        printf("%s", parse[1]);
        perror("exevcp failed");
        exit(EXIT_FAILURE);
    }
    //parent process
    else if (!existAmpersand)
    {
        waitpid(pid, NULL, 0);
    }

    return;
}

//ham chuyen huong output
void redirectOutput(char **command, char **fileName)
{
    bool existAmpersand = isAmpersand(fileName);
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        return;
    }
    if (pid == 0)
    {
        int fd = open(fileName[0], O_CREAT | O_WRONLY, 0666);
        if (fd < 0)
        {
            perror("open error");
            return;
        }
        if (dup2(fd, STDOUT_FILENO) < 0)
        {
            perror("dup2 error");
            return;
        }
        close(fd);
        execvp(command[0], command);
        perror("execvp failed.");
        exit(0);
    }
    if (!existAmpersand)
        waitpid(pid, NULL, 0);
}

//ham chuyen huong input
void redirectInput(char **command, char **fileName)
{
    bool existAmpersand = isAmpersand(fileName);
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        return;
    }
    if (pid == 0)
    {
        int fd = open(fileName[0], O_RDONLY, 0666);
        if (fd < 0)
        {
            perror("open failed");
            return;
        }
        if (dup2(fd, STDIN_FILENO) < 0)
        {
            perror("dup2 failed");
            return;
        }
        close(fd);
        execvp(command[0], command);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    if (!existAmpersand)
    {
        waitpid(pid, NULL, 0);
    }
}

//thuc thi pipe
void execPipe(char **argv1, char **argv2)
{
    bool existAmpersand = isAmpersand(argv2);
    int p[2];
    pid_t p1, p2;
    if (pipe(p) < 0)
    {
        perror("pipe failed");
        return;
    }

    p1 = fork();
    if (p1 < 0)
    {
        perror("fork failed");
        exit(0);
    }
    if (p1 == 0)
    {
        p2 = fork();

        if (p2 < 0)
        {
            perror("fork failed");
            exit(0);
        }
        if (p2 == 0) // p2 nhan dau vao tu p1, dong write end p[1]
        {
            close(p[1]);
            dup2(p[0], STDIN_FILENO);
            close(p[0]);
            if (execvp(argv2[0], argv2))
            {
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
        }
        //tien trinh p1, gui du lieu cho p2, dong read end p[0]
        close(p[0]);
        dup2(p[1], STDOUT_FILENO);
        close(p[1]);
        if (execvp(argv1[0], argv1) < 0)
        {
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    }

    //tien trinh cha

    close(p[1]);
    close(p[0]);

    if (!existAmpersand)
    {
        waitpid(p1, NULL, 0);
        waitpid(p2, NULL, 0);
    }
}

/* Ham kiem tra cau lenh nhap vao line
-neu cau lenh nhap vao la PIPE, argv1 se luu nhung token o ben trai cua |, argv2 luu token ben phai
-neu la REDIRECT, argv1 luu token o ben trai cua (</>), ben phai la argv2 (ten file)
-con neu cau lenh binh thuong, luu tat ca vao argv1, argv2 = null
*/
int checkInput(char *line, char ***argv1, char ***argv2)
{
    int type = isSimpleCommand;
    char **temp = NULL;
    for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] == '|')
        {
            type = isPipe;
            break;
        }

        else if (line[i] == '>')
        {
            type = isOutputRedirect;
            break;
        }

        else if (line[i] == '<')
        {
            type = isInputRedirect;
            break;
        }
    }

    if (type != isSimpleCommand)
    {
        temp = parsePipe(line);
        *argv1 = tokenizeInput(temp[0]);
        *argv2 = tokenizeInput(temp[1]);
    }
    else
    {
        *argv1 = tokenizeInput(line);
        *argv2 = NULL;
    }

    return type;
}

void loop()
{
    char *line;
    char **tokenCmd1 = NULL;
    char **tokenCmd2 = NULL;
    int status;
    char *intro = (char *)"SimpleShell > ";
    char *noHistory = (char *)"History is empty!!!\n";

    while (true)
    {
        
        printf("%s", intro);

        //take input
        line = takeInput();

        //check input (check nhung truong hop NULL, exit, !!)
        formatInput(line, '\0');

        // neu nhap cau rong
        if (line[0] == (char)'\0')
        {
            continue;
        }

        if (strcmp(line, "exit") == 0) // neu nhap exit
        {
            waitpid(-1, &status, 0);
            break;
        }

        if (strcmp(line, "!!") == 0) // neu nhap !! de truy cap lich su
        {
            if (history == NULL) //neu khong co lenh gan day trong lich su
            {
                printf("%s", noHistory);
                continue;
            }
            else
            {
                strcpy(line, history);
                
                printf("%s", intro);
                
                printf("%s\n", line);
            }
        }

        //kiem tra cau lenh nhap vao la loai nao trong (pipe, redirect hay cau lenh binh thuong)
        int type = checkInput(line, &tokenCmd1, &tokenCmd2);

        if (type == isOutputRedirect)
        {
            redirectOutput(tokenCmd1, tokenCmd2);
        }
        else if (type == isInputRedirect)
        {
            redirectInput(tokenCmd1, tokenCmd2);
        }
        else if (type == isPipe)
        {
            execPipe(tokenCmd1, tokenCmd2);
            usleep(20);
        }
        else
        {
            execArgv(tokenCmd1);
        }
    }
}