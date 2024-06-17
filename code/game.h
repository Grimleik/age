#if !defined(GAME_H)
/* ========================================================================
   Creator: Grimleik $
   ========================================================================*/
#define GAME_H

class AssetDatabase
{
public:
    int RegisterAsset(asset_t &asset)
    {
        assets.push_back(asset);
        // TODO(pf): Find a better ID system.
        asset.ID = (int)assets.size() - 1;
        return asset.ID;
    }

    asset_t &GetAsset(int index)
    {
        return assets[index];
    }

    std::vector<asset_t> assets;
};

struct gameState_t
{
    f32 accumulation;
    void *memory;
    size_t memorySz;
    bool isPaused;
    bool shouldStepOnce;
    AssetDatabase assetDB;
    int selectedAsset;

    void Initialize()
    {
        accumulation = 0.0f;
        memory = nullptr;
        memorySz = 0;
        isPaused = false;
        shouldStepOnce = false;
        new(&assetDB) AssetDatabase();
        selectedAsset = -1;
    }
};

#endif
