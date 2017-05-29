#include "UE4DuinoModule.h"

IMPLEMENT_MODULE(UE4DuinoModule, UE4Duino);

#define LOCTEXT_NAMESPACE "UE4Duino"

UE4DuinoModule::UE4DuinoModule()
{
}

void UE4DuinoModule::StartupModule()
{
	// Startup LOG MSG
	UE_LOG(UE4DuinoLog, Warning, TEXT("UE4Duino: Log Started"));
}

void UE4DuinoModule::ShutdownModule()
{
	// Shutdown LOG MSG
	UE_LOG(UE4DuinoLog, Warning, TEXT("UE4Duino: Log Ended"));
}