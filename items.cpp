#include <iostream>
#include <fstream>
#include <vector>
#include "string.h"
#include "items.h"

typedef unsigned char BYTE;

int itemdathash;
int itemsDatSize = 0;
int player_hash;
int playerSize = 0;
BYTE *itemsDat;
BYTE *playerTri;

std::vector<ItemDefinition> itemDefs;

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

unsigned char *getA(std::string fileName, int *pSizeOut)
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
    std::string secret = "PBG892FXX982ABC*";

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
            pData = getA((std::string)filename1, &size);
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
            pData = getA((std::string)filename, &size);
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
    int itemCount;
    int memPos = 0;
    int16_t itemsdatVersion = 0;
    memcpy(&itemsdatVersion, data + memPos, 2);
    memPos += 2;
    memcpy(&itemCount, data + memPos, 4);
    memPos += 4;
    for (int i = 0; i < itemCount; i++)
    {
        ItemDefinition tile;

        {
            memcpy(&tile.id, data + memPos, 4);
            memPos += 4;
        }
        {
            tile.editableType = data[memPos];
            memPos += 1;
        }
        {
            tile.itemCategory = data[memPos];
            memPos += 1;
        }
        {
            tile.actionType = data[memPos];
            memPos += 1;
        }
        {
            tile.hitSoundType = data[memPos];
            memPos += 1;
        }
        {
            int16_t strLen = *(int16_t *)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++)
            {
                tile.name += data[memPos] ^ (secret[(j + tile.id) % secret.length()]);

                memPos++;
            }
        }
        {
            int16_t strLen = *(int16_t *)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++)
            {
                tile.texture += data[memPos];
                memPos++;
            }
        }
        memcpy(&tile.textureHash, data + memPos, 4);
        memPos += 4;
        tile.itemKind = memPos[data];
        memPos += 1;
        memcpy(&tile.val1, data + memPos, 4);
        memPos += 4;
        tile.textureX = data[memPos];
        memPos += 1;
        tile.textureY = data[memPos];
        memPos += 1;
        tile.spreadType = data[memPos];
        memPos += 1;
        tile.isStripeyWallpaper = data[memPos];
        memPos += 1;
        tile.collisionType = data[memPos];
        memPos += 1;
        tile.breakHits = data[memPos] / 6;
        memPos += 1;
        memcpy(&tile.dropChance, data + memPos, 4);
        memPos += 4;
        tile.clothingType = data[memPos];
        memPos += 1;
        memcpy(&tile.rarity, data + memPos, 2);
        memPos += 2;
        tile.maxAmount = data[memPos];
        memPos += 1;
        {
            int16_t strLen = *(int16_t *)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++)
            {
                tile.extraFile += data[memPos];
                memPos++;
            }
        }
        memcpy(&tile.extraFileHash, data + memPos, 4);
        memPos += 4;
        memcpy(&tile.audioVolume, data + memPos, 4);
        memPos += 4;
        {
            int16_t strLen = *(int16_t *)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++)
            {
                tile.petName += data[memPos];
                memPos++;
            }
        }
        {
            int16_t strLen = *(int16_t *)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++)
            {
                tile.petPrefix += data[memPos];
                memPos++;
            }
        }
        {
            int16_t strLen = *(int16_t *)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++)
            {
                tile.petSuffix += data[memPos];
                memPos++;
            }
        }
        {
            int16_t strLen = *(int16_t *)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++)
            {
                tile.petAbility += data[memPos];
                memPos++;
            }
        }
        {
            tile.seedBase = data[memPos];
            memPos += 1;
        }
        {
            tile.seedOverlay = data[memPos];
            memPos += 1;
        }
        {
            tile.treeBase = data[memPos];
            memPos += 1;
        }
        {
            tile.treeLeaves = data[memPos];
            memPos += 1;
        }
        {
            memcpy(&tile.seedColor, data + memPos, 4);
            memPos += 4;
        }
        {
            memcpy(&tile.seedOverlayColor, data + memPos, 4);
            memPos += 4;
        }
        memPos += 4; // deleted ingredients
        {
            memcpy(&tile.growTime, data + memPos, 4);
            memPos += 4;
        }
        memcpy(&tile.val2, data + memPos, 2);
        memPos += 2;
        memcpy(&tile.isRayman, data + memPos, 2);
        memPos += 2;
        {
            int16_t strLen = *(int16_t *)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++)
            {
                tile.extraOptions += data[memPos];
                memPos++;
            }
        }
        {
            int16_t strLen = *(int16_t *)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++)
            {
                tile.texture2 += data[memPos];
                memPos++;
            }
        }
        {
            int16_t strLen = *(int16_t *)&data[memPos];
            memPos += 2;
            for (int j = 0; j < strLen; j++)
            {
                tile.extraOptions2 += data[memPos];
                memPos++;
            }
        }
        memPos += 80;
        if (itemsdatVersion >= 11)
        {
            {
                int16_t strLen = *(int16_t *)&data[memPos];
                memPos += 2;
                for (int j = 0; j < strLen; j++)
                {
                    tile.punchOptions += data[memPos];
                    memPos++;
                }
            }
        }
        if (itemsdatVersion >= 12)
        {

            memPos += 13;
        }
        if (i != tile.id)
        {
            std::cout << "Item are unordered!" << i << "/" << tile.id << std::endl;
        }

        switch (tile.actionType)
        {
        case 0:
            tile.blockType = BlockTypes::FIST;
            break;
        case 1:
            break;
        case 2:
            tile.blockType = BlockTypes::DOOR;
            break;
        case 3:
            tile.blockType = BlockTypes::LOCK;
            break;
        case 4:
            tile.blockType = BlockTypes::GEM;
            break;
        case 8:
            tile.blockType = BlockTypes::CONSUMMABLE;
            break;
        case 9:
            tile.blockType = BlockTypes::GATEWAY;
            break;
        case 10:
            tile.blockType = BlockTypes::SIGN;
            break;
        case 13:
            tile.blockType = BlockTypes::MAIN_DOOR;
            break;
        case 15:
            tile.blockType = BlockTypes::BEDROCK;
            break;
        case 17:
            tile.blockType = BlockTypes::FOREGROUND;
            break;
        case 18:
            tile.blockType = BlockTypes::BACKGROUND;
            break;
        case 19:
            tile.blockType = BlockTypes::SEED;
            break;
        case 20:
            tile.blockType = BlockTypes::CLOTHING;
            switch (tile.clothingType)
            {
            case 0:
                tile.clothType = ClothTypes::HAIR;
                break;
            case 1:
                tile.clothType = ClothTypes::SHIRT;
                break;
            case 2:
                tile.clothType = ClothTypes::PANTS;
                break;
            case 3:
                tile.clothType = ClothTypes::FEET;
                break;
            case 4:
                tile.clothType = ClothTypes::FACE;
                break;
            case 5:
                tile.clothType = ClothTypes::HAND;
                break;
            case 6:
                tile.clothType = ClothTypes::BACK;
                break;
            case 7:
                tile.clothType = ClothTypes::MASK;
                break;
            case 8:
                tile.clothType = ClothTypes::NECKLACE;
                break;
            }
            break;
        case 21:
            tile.blockType = BlockTypes::FOREGROUND;
            break;
        case 24:
            tile.blockType = BlockTypes::FOREGROUND;
            break;
        case 26: // portal
            tile.blockType = BlockTypes::DOOR;
            break;
        case 27:
            tile.blockType = BlockTypes::CHECKPOINT;
            break;
        case 28: // piano note
            tile.blockType = BlockTypes::BACKGROUND;
            break;
        case 41:
            tile.blockType = BlockTypes::WEATHER_MACHINE;
            break;
        case 34: // bulletin boardd
            tile.blockType = BlockTypes::BOARD;
            break;
        case 61:
            tile.blockType = BlockTypes::FOREGROUND;
            break;
        case 107: // ances
            tile.blockType = BlockTypes::CLOTHING;
            tile.clothType = ClothTypes::ANCES;
            break;
        default:
            break;
        }
        itemDefs.push_back(tile);
    }
    delete[] data;
    delete[] data1;
}