#pragma once

#include "WCore/WCore.hpp"
#include "WCore/TRef.hpp"
#include "WObjectManager/WObjectManager.hpp"

#include <memory>

class WRender;
class WObjectManager;
class WImportersRegister;
class WImporter;

class WENGINE_API WEngine
{

public:

    virtual ~WEngine();

    WEngine(const WEngine & other) = delete;

    WEngine(WEngine && other) = default;

    WEngine & operator=(const WEngine & other) = delete;

    WEngine & operator=(WEngine && other) = default;

    void run();

    static WEngine Create();

    TRef<WImportersRegister> ImportersRegister() noexcept;

    TRef<WRender> Render() noexcept;


private:

    WEngine();    

    void InitializeObjectManager();

    void InitializeImporters();

    void InitializeRender();

    std::unique_ptr<WRender> render_{nullptr};

    std::unique_ptr<WObjectManager> object_manager_{nullptr};

    std::unique_ptr<WImportersRegister> importers_register_{nullptr};

    bool close{false};
};

