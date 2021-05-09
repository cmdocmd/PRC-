#pragma once

#include <iostream>
#include <vector>
#include <fstream>

#include "server.h"

typedef unsigned char BYTE;

extern int itemdathash;
extern int itemsDatSize;
extern int player_hash;
extern int playerSize;
extern BYTE *itemsDat;
extern BYTE *playerTri;

enum ClothTypes
{
    HAIR,
    SHIRT,
    PANTS,
    FEET,
    FACE,
    HAND,
    BACK,
    MASK,
    NECKLACE,
    ANCES,
    NONE
};

enum BlockTypes
{
    FOREGROUND,
    BACKGROUND,
    SEED,
    PAIN_BLOCK,
    BEDROCK,
    MAIN_DOOR,
    SIGN,
    DOOR,
    CLOTHING,
    FIST,
    CONSUMMABLE,
    CHECKPOINT,
    GATEWAY,
    LOCK,
    WEATHER_MACHINE,
    JAMMER,
    GEM,
    BOARD,
    UNKNOWN
};

struct ItemDefinition
{
    int id;

    unsigned char editableType = 0;
    unsigned char itemCategory = 0;
    unsigned char actionType = 0;
    unsigned char hitSoundType = 0;

    std::string name;

    std::string texture = "";
    int textureHash = 0;
    unsigned char itemKind = 0;
    int val1;
    unsigned char textureX = 0;
    unsigned char textureY = 0;
    unsigned char spreadType = 0;
    unsigned char isStripeyWallpaper = 0;
    unsigned char collisionType = 0;

    unsigned char breakHits = 0;

    int dropChance = 0;
    unsigned char clothingType = 0;
    BlockTypes blockType;
    int growTime;
    ClothTypes clothType;
    int16_t rarity = 0;
    unsigned char maxAmount = 0;
    std::string extraFile = "";
    int extraFileHash = 0;
    int audioVolume = 0;
    std::string petName = "";
    std::string petPrefix = "";
    std::string petSuffix = "";
    std::string petAbility = "";
    unsigned char seedBase = 0;
    unsigned char seedOverlay = 0;
    unsigned char treeBase = 0;
    unsigned char treeLeaves = 0;
    int seedColor = 0;
    int seedOverlayColor = 0;
    bool isMultiFace = false;
    short val2;
    short isRayman = 0;
    std::string extraOptions = "";
    std::string texture2 = "";
    std::string extraOptions2 = "";
    std::string punchOptions = "";
    std::string description = "Nothing to see.";
    bool updatedgem = false;
    int last_tile_broken = 0;
};

extern std::vector<ItemDefinition> itemDefs;

std::ifstream::pos_type filesize(const char *filename);
uint32_t HashString(unsigned char *str, int len);
unsigned char *getA(std::string fileName, int *pSizeOut);
void itemsbuild();
