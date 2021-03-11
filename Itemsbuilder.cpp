#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>

int itemdathash;
int itemsDatSize = 0;
int player_hash;
int playerSize = 0;
typedef unsigned char BYTE;

BYTE *itemsDat;
BYTE *playerTri;

using std::string;

std::ifstream::pos_type filesize(const char *filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

uint32_t HashString(unsigned char *str, int len)
{
    if (!str)
        return 0;

    unsigned char *n = (unsigned char *)str;
    uint32_t acc = 0x55555555;

    if (len == 0)
    {
        while (*n)
        {
            acc = (acc >> 27) + (acc << 5) + *n++;
        }
    }
    else
    {
        for (int i = 0; i < len; i++)
        {
            acc = (acc >> 27) + (acc << 5) + *n++;
        }
    }
    return acc;
}

unsigned char *getA(string fileName, int *pSizeOut, bool bAddBasePath, bool bAutoDecompress)
{
    unsigned char *pData = NULL;
    FILE *fp = fopen(fileName.c_str(), "rb");
    if (!fp)
    {
        std::cout << "File not found" << std::endl;
        if (!fp)
            return NULL;
    }

    fseek(fp, 0, SEEK_END);
    *pSizeOut = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    pData = (unsigned char *)new unsigned char[((*pSizeOut) + 1)];
    if (!pData)
    {
        printf("Out of memory opening %s?", fileName.c_str());
        return 0;
    }
    pData[*pSizeOut] = 0;
    fread(pData, *pSizeOut, 1, fp);
    fclose(fp);

    return pData;
}

void itemsbuild()
{
    string secret = "PBG892FXX982ABC*";

    std::ifstream file("items.dat", std::ios::binary | std::ios::ate);
    int size = file.tellg();
    itemsDatSize = size;
    char *data = new char[size];
    file.seekg(0, std::ios::beg);

    std::ifstream file1("player_tribute.dat", std::ios::binary | std::ios::ate);
    int size1 = file1.tellg();
    playerSize = size1;
    char *data1 = new char[size1];
    file1.seekg(0, std::ios::beg);

    if (file1.read((char *)(data1), size1))
    {
        playerTri = new BYTE[60 + size1];
        int MessageType = 0x4;
        int PacketType = 0x10;
        int NetID = -1;
        int CharState = 0x8;

        memset(playerTri, 0, 60);
        memcpy(playerTri, &MessageType, 4);
        memcpy(playerTri + 4, &PacketType, 4);
        memcpy(playerTri + 8, &NetID, 4);
        memcpy(playerTri + 16, &CharState, 4);
        memcpy(playerTri + 56, &size, 4);
        file1.seekg(0, std::ios::beg);
        if (file1.read((char *)(playerTri + 60), size1))
        {
            uint8_t *pData;
            int size = 0;
            file1.close();
            const char filename1[] = "player_tribute.dat";
            size = filesize(filename1);
            pData = getA((string)filename1, &size, false, false);
            std::cout << "player_tribute hash: " << HashString((unsigned char *)pData, size) << std::endl;
            player_hash = HashString((unsigned char *)pData, size);
            delete[] pData;
        }
    }
    else
    {
        std::cout << "Updating player tribute failed!" << std::endl;
        exit(0);
    }

    if (file.read((char *)(data), size))
    {
        itemsDat = new BYTE[60 + size];
        int MessageType = 0x4;
        int PacketType = 0x10;
        int NetID = -1;
        int CharState = 0x8;

        memset(itemsDat, 0, 60);
        memcpy(itemsDat, &MessageType, 4);
        memcpy(itemsDat + 4, &PacketType, 4);
        memcpy(itemsDat + 8, &NetID, 4);
        memcpy(itemsDat + 16, &CharState, 4);
        memcpy(itemsDat + 56, &size, 4);
        file.seekg(0, std::ios::beg);
        if (file.read((char *)(itemsDat + 60), size))
        {
            uint8_t *pData;
            int size = 0;
            const char filename[] = "items.dat"; //items.dat
            size = filesize(filename);
            pData = getA((string)filename, &size, false, false);
            std::cout << "itemsdat hash: " << HashString((unsigned char *)pData, size) << std::endl;
            itemdathash = HashString((unsigned char *)pData, size);
            delete[] pData;
            file.close();
        }
    }
    else
    {
        std::cout << "Updating items data failed!" << std::endl;
        exit(0);
    }
    delete[] data;
    delete[] data1;
}