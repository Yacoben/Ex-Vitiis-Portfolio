#include "ExVitiumEdEngine.h"

#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"

void UExVitiumEdEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);

	const ISourceControlModule& SourceControlModule = ISourceControlModule::Get();
	ISourceControlProvider& SourceControlProvider = SourceControlModule.GetProvider();

	const TArray<FString> Branches{
		TEXT("origin/develop")
	};

	SourceControlProvider.RegisterStateBranches(Branches, TEXT("Content"));

	UE_LOG(LogTemp, Log, TEXT("UExVitiumEdEngine::Init - Status Branches registered"));
}
