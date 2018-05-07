#pragma once
#include "StorageManager.h"

class OxyCRGStorageManagerPrivate;
class OxyCRGStorageManager : public StorageManager
{
public:
    static OxyCRGStorageManager &getInstance();
    ~OxyCRGStorageManager();



    void run();

private:
    Q_DECLARE_PRIVATE(OxyCRGStorageManager)
    OxyCRGStorageManager();
};
#define oxyCRGStorageManager (OxyCRGStorageManager::getInstance())
