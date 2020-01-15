#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_CLASS(UE4DuinoLog, Log, All);

class UE4DuinoModule : public IModuleInterface
{
private:

public:
	UE4DuinoModule();
	
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};