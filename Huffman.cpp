#undef __DEPRECATED
#include <iostream>
#include <fstream>
#include <cstring>
#include <strstream>
#include <sstream>
#include <queue>
#include <stack>
#include <cstdio>
#include <cstdlib>
using namespace std;

struct file
{
    char *q;
    long length;
};

struct Node
{
    int leftChild;
    int rightChild;
};

class CharactorCode
{
public:
    long weight;
    unsigned char num;
    long leftChild;
    long rightChild;
    long parent;
    char bits[256];
    CharactorCode(long weight, int num, unsigned char leftChild, long rightChild) : parent(-1), weight(weight), num(num), leftChild(leftChild), rightChild(rightChild) {}
    CharactorCode() : weight(0), num(0), leftChild(-1), rightChild(-1), parent(-1) {}
    bool operator<(const CharactorCode &b) const
    {
        return weight < b.weight;
    }
};

class huffman_encoder
{
    Node tree[256];
    file File;
    int head;
    long sum;
    CharactorCode str[256];
    string filename;
    priority_queue<CharactorCode> huffman_queue;

    void encode_init(const char *filename)
    {
        ifstream fin(filename, ios::binary); // 以二进制方式打开文件
        if (!fin)
        {
            cerr << "Can not open the init_0 argv file!" << endl; // 输出错误信息并退出
            exit(0);
        }

        int m;
        fin.seekg(0, ios::end); //定位读指针位置为文件结尾
        m = fin.tellg();        //获取当前读指针位置(字节)
        fin.seekg(0, ios::beg);
        File.q = new (nothrow) char[m];
        File.length = m;
        if (File.q == NULL)
            cout << "memory  failed";
        fin.close();

        FILE *r;
        r = fopen(filename, "rb");
        if (r == NULL)
        {
            printf("can't open file1.txt\n");
            exit(0);
        }
        fread(File.q, sizeof(char) * m, 1, r);
        if (File.q != NULL)
            File.q[m - 1] = '\0';
        fclose(r);
    }

    void init_huffman_tree()
    {
        for (int i = 0; i < File.length; i++)
        {
            int x = int(File.q[i]); //角标是char的数值
            if (!x)
                continue;
            str[x].weight++;
            str[x].num = x; //num也是char的数值，便于在构建父节点的时候加入
        }
        for (int i = 0; i < 128; i++)
        {
            str[i].num = i;
            huffman_queue.push(str[i]);
        }
        /*	int i = 0;
	while (!huffman_queue.empty()) {
			char ch = huffman_queue.top().num;
			if (huffman_queue.top().weight) {
				cout << ch << huffman_queue.top().weight << endl;
				i++;
			}
			if (i == 20) break;
			huffman_queue.pop();
		}*/
    }

    void encode_huffman_tree()
    {
        //建树建的有问题，前面所有的结点都是叶子结点了
        //最大的结点没有孩子

        CharactorCode a, b;
        int temp1;

        int max = 0, i = 0;
        while (!huffman_queue.empty())
        {
            a = huffman_queue.top();
            huffman_queue.pop();
            str[i] = a;
            i++;
        }

        sum = 0;
        for (i = 0; i < 256; i++)
        {
            if (str[i].weight > max)
            {
                head = i;
                max = str[i].weight;
            }
            if (str[i].weight == 0)
                break;
        }
        sum = i;

        for (; i < 2 * sum - 1; i++)
        {
            int min1 = 2147483244;
            str[i].num = i;
            for (int j = 0; j < i; j++)
            {
                if (str[j].parent != -1)
                    continue;
                if (min1 > str[j].weight)
                {
                    temp1 = j;
                    min1 = str[j].weight;
                    continue;
                }
            }
            str[i].weight = str[temp1].weight;
            str[temp1].parent = i;
            str[i].leftChild = temp1;

            min1 = 2147483244;
            for (int j = 0; j < i; j++)
            {
                if (str[j].parent != -1)
                    continue;
                if (min1 > str[j].weight)
                {
                    temp1 = j;
                    min1 = str[j].weight;
                    continue;
                }
            }

            str[i].weight += str[temp1].weight;
            str[i].rightChild = temp1;
            str[temp1].parent = i;
        }
    }

    void encode_huffman_code()
    {
        int f = 0, j = 0;
        for (int i = 0; i < sum; i++)
        {
            f = i;
            str[i].bits[0] = 0;
            while (str[f].parent != -1)
            {
                j = f;
                f = str[f].parent;
                if (str[f].leftChild == j)
                {
                    j = strlen(str[i].bits);
                    memmove(str[i].bits + 1, str[i].bits, j + 1);
                    str[i].bits[0] = '0';
                }
                else
                {
                    j = strlen(str[i].bits);
                    memmove(str[i].bits + 1, str[i].bits, j + 1);
                    str[i].bits[0] = '1';
                }
            }
        }
    }

    void write_to_file()
    {
        FILE *ofp = fopen(filename.c_str(), "wb"); //打开压缩后存储信息的文件
        if (ofp == NULL)
        {
            printf("打开文件失败:%s\n", filename.c_str());
            return;
        }
        fseek(ofp, 8, SEEK_SET);
        char buf[512], c;
        buf[0] = 0;
        long f = 0, temp = 8, i = 0, j = 0;
        CharactorCode a;

        for (int k = 0; k < File.length; k++)
        {
            c = File.q[k];
            f++;
            for (i = 0; i < sum; i++)
            {
                if (c == str[i].num)
                    break;
            }
            strcat(buf, str[i].bits);
            j = strlen(buf);
            c = 0;
            while (j >= 8) //当剩余字符数量不小于8个时
            {
                for (i = 0; i < 8; i++) //按照八位二进制数转化成十进制ASCII码写入文件一次进行压缩
                {
                    if (buf[i] == '1')
                        c = (c << 1) | 1;
                    else
                        c = c << 1;
                }
                fwrite(&c, 1, 1, ofp);
                temp++;
                strcpy(buf, buf + 8);
                j = strlen(buf);
            }
            if (f == File.length)
                break;
        }

        if (j > 0) //当剩余字符数量少于8个时
        {
            strcat(buf, "00000000");
            for (i = 0; i < 8; i++)
            {
                if (buf[i] == '1')
                    c = (c << 1) | 1;
                else
                    c = c << 1; //对不足的位数进行补零
            }
            fwrite(&c, 1, 1, ofp);
            temp++;
        }

        fseek(ofp, 0, SEEK_SET); //将编码信息写入存储文件
        fwrite(&File.length, 1, sizeof(File.length), ofp);
        fwrite(&temp, sizeof(long), 1, ofp);
        fseek(ofp, temp, SEEK_SET);
        fwrite(&sum, sizeof(long), 1, ofp);
        for (i = 0; i < sum; i++)
        {
            a = str[i];
            fwrite(&(str[i].num), 1, 1, ofp);
            temp++;
            c = (unsigned char)strlen(str[i].bits);
            fwrite(&c, 1, 1, ofp);
            temp++;
            j = strlen(str[i].bits);

            if (j % 8 != 0) //当位数不满8时，对该数进行补零操作
            {
                for (f = j % 8; f < 8; f++)
                    strcat(str[i].bits, "0");
            }

            while (str[i].bits[0] != 0)
            {
                c = 0;
                for (j = 0; j < 8; j++)
                {
                    if (str[i].bits[j] == '1')
                        c = (c << 1) | 1;
                    else
                        c = c << 1;
                }
                strcpy(str[i].bits, str[i].bits + 8);
                fwrite(&c, 1, 1, ofp);
                temp++;
            }

            str[i] = a;
        }
        fclose(ofp);
    }

public:
    void encode(string __filename)
    {
        filename = __filename;
        encode_init(__filename.c_str());
        filename.erase(filename.find("."));
        // filename += ".log";
        filename="result.log";
        init_huffman_tree();
        encode_huffman_tree();
        encode_huffman_code(); //将weight为0的全部删除覆盖
        write_to_file();
        cout << sum;
        //cout << str[head].num << " " << str[head].weight << endl << sum;
        ofstream x("KeyValue.log");
        for (int i = 0; i < sum; i++)
        { //<< str[i].leftChild << " " << str[i].rightChild << " " << " " << str[i].parent
            x << int(str[i].num) << " " << str[i].weight << " " << str[i].parent << " " << str[i].leftChild << " " << str[i].rightChild << " " << str[i].bits << endl;
        }
        cout << "Compress Complete";
    }

    void decode(string __filename)
    {
        long f1 = 0, f2 = 0, f3 = 0;
        unsigned char c;
        char buf[256], buf2[256];
        CharactorCode a;
        filename = __filename;

        ifstream fin(__filename, ios::binary);
        if (!fin)
        {
            cerr << "Can not open the init_0 argv file!" << endl; // 输出错误信息并退出
            exit(0);
        }

        long m;
        fin.seekg(0, ios::end); //定位读指针位置为文件结尾
        m = fin.tellg();        //获取当前读指针位置(字节)
        fin.seekg(0, ios::beg);
        fin.close();
        File.q = new (nothrow) char[m];
        File.length = m;
        cout << m;
        if (File.q == NULL)
            cout << "memory  failed";

        FILE *ifp = fopen((const char *)filename.c_str(), "rb");
        filename.erase(filename.find("."));
        // filename += "1.txt";
        filename="result1.txt";

        FILE *ofp = fopen((const char *)filename.c_str(), "wb");

        fseek(ifp, 0, SEEK_SET);
        fread(&File.length, sizeof(long), 1, ifp); //读取原文件长
        fread(&f1, sizeof(long), 1, ifp);
        fseek(ifp, f1, SEEK_SET);
        fread(&f2, sizeof(long), 1, ifp);

        m = 0;
        for (int i = 0; i < f2; i++) //读取压缩文件内容并转换成二进制码
        {
            fread(&str[i].num, 1, 1, ifp);
            fread(&c, 1, 1, ifp);
            f3 = (long)c;
            str[i].weight = f3;
            str[i].bits[0] = 0;
            if (f3 % 8 > 0)
                m = f3 / 8 + 1;
            else
                m = f3 / 8;
            for (int j = 0; j < m; j++)
            {
                fread(&c, 1, 1, ifp);
                f1 = c;
                _itoa(f1, buf, 2);
                f1 = strlen(buf);
                for (int l = 8; l > f1; l--)
                {
                    strcat(str[i].bits, "0"); //位数不足，执行补零操作
                }
                strcat(str[i].bits, buf);
            }
            str[i].bits[f3] = 0;
        }

        for (int i = 0; i < f2; i++)
        {
            for (int j = i + 1; j < f2; j++)
            {
                if (strlen(str[i].bits) > strlen(str[j].bits))
                {
                    a = str[i];
                    str[i] = str[j];
                    str[j] = a;
                }
            }
        }

        f3 = strlen(str[f2 - 1].bits);
        fseek(ifp, 8, SEEK_SET);
        m = 0;
        buf2[0] = 0;

        while (1)
        {
            while (strlen(buf2) < (unsigned int)f2)
            {
                fread(&c, 1, 1, ifp);
                f1 = c;
                _itoa(f1, buf, 2);
                f1 = strlen(buf);
                for (int l = 8; l > f1; l--)
                {
                    strcat(buf2, "0");
                }
                strcat(buf2, buf);
            }
            int i = 0;
            for (i = 0; i < f2; i++)
            {
                if (memcmp(str[i].bits, buf2, str[i].weight) == 0)
                    break;
            }
            strcpy(buf2, buf2 + str[i].weight);
            c = str[i].num;
            fwrite(&c, 1, 1, ofp);
            m++;
            if (m == File.length)
                break;
        }
        fclose(ofp);
        fclose(ifp);
        cout << "Decompress Complete";
        exit(0);
    }
};

bool huffman_compression(string order1, string filename)
{
    huffman_encoder coder;
label:
    if (order1 == "encode")
    {
        coder.encode(filename);
        return 1;
    }
    else if (order1 == "decode")
    {
        coder.decode(filename);
        return 1;
    }
    else
    {
        return 0;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Please make sure the number of parameters is correct." << argc<<endl;
        return -1;
    }
    // 0:程序地址；1：encode、decode指令；2：源文件
    huffman_compression(argv[1], argv[2]);
    return 0;
}