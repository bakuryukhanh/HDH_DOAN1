#include "header.h"

//dem so phan tu trong mang con tro tokens
int numOfToken(char **tokens)
{
    int i = 1;
    while (tokens[i] != NULL)
    {
        i++;
    }
    return i;
}


// xoa cac ky tu " \n\t\r\a" o dau hoac cuoi chuoi str
// viet chu yeu dung de xoa ki tu & cuoi cau trong chuoi (neu co gan c vao thi xoa ki tu do o cuoi chuoi)
void formatInput(char *str, char c)
{

    
    int begin = 0, end = strlen(str) - 1;
    if (c == '\0')
    {
        while ((str[begin] == ' ' || str[begin] == '\n' || str[begin] == '\t' || str[begin] == '\a' || str[begin] == '\r') && (begin < end))
        {
            begin++;
        }
        while ((end >= begin) && (str[end] == ' ' || str[end] == '\n' || str[end] == '\t' || str[end] == '\a' || str[end] == '\r'))
        {
            end--;
        }
    }
    else
        end--; //neu muon xoa ki tu c o cuoi chuoi

int i;
    for (i = begin; i <= end; i++)
    {
        str[i - begin] = str[i];
    }
    str[i - begin] = '\0';
}

// ham kiem tra co dau & o cuoi cau khong
bool isAmpersand(char **input)
{
    bool result = false;
    int n = numOfToken(input);
    if (strcmp(input[n - 1], "&") == 0)
    {
        result = true;
        input[n - 1] = NULL; //xoa dau & vi shell khong hieu
    }
    else // lo nguoi dung nhap dinh lien, vd ls&
    {
        int len = strlen(input[n - 1]);
        char temp = input[n - 1][len - 1];
        if (temp == '&')
        {
            result = true;
            formatInput(input[n - 1], temp);
        }
    }

    return result;
}

// nhan vao va tra ve cau lanh cua nguoi dung nhap vao
// luu cau lenh vao history neu khong phai la "!!"
char* takeInput()
{
    char *line = NULL;
    size_t buffSize = 0;

    int numChar = getline(&line, &buffSize, stdin);

    if (line[numChar - 1] == '\n')
    {
        line[numChar - 1] = '\0';
    }

    //history feature
    if (strcmp(line, "!!") != 0 && line[0] != '\0')
    {
        if (history != NULL)
            free(history);
        history = (char *)malloc(strlen(line) + 1);
        strcpy(history, line);
    }

    return line;
}

//ham tach cau lenh thanh token theo khoang trang DELIM
char **tokenizeInput(char *line)
{
    int buffSize = TOKEN_BUFSIZE;
    int pos = 0;
    char *token;
    char **tokenArr = (char **)malloc(sizeof(char *) * buffSize);

    if (!tokenArr)
    {
        perror("alloction error");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, DELIM);

    while (token != NULL)
    {
        tokenArr[pos] = token;
        pos++;

        if (pos >= buffSize)
        {
            buffSize += TOKEN_BUFSIZE;
            tokenArr = (char **)realloc(tokenArr, buffSize * sizeof(char *));
            if (!tokenArr)
            {
                perror("allocation error");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, DELIM);
    }
    tokenArr[pos] = NULL;

    return tokenArr;
}

//ham tach cau lenh ra 2 ve, ben trai va phai cua |<>
char **parsePipe(char *line)
{
    int buffSize = 2;
    char *pipe;
    char **pipeArr = (char **)malloc(buffSize * sizeof(char *) + 1);
    int pos = 0;

    pipe = strtok(line, pipeRedirect);
    formatInput(pipe, '\0');
    pipeArr[0] = pipe;

    pipe = strtok(NULL, "\n");
    formatInput(pipe, '\0');
    pipeArr[1] = pipe;

    pipeArr[2] = NULL;
    return pipeArr;
}