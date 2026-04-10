#pragma once

#include "CoreMinimal.h"
#include "Editor/UnrealEdEngine.h"
#include "ExVitiumEdEngine.generated.h"

UCLASS()
class UExVitiumEdEngine : public UUnrealEdEngine
{
	GENERATED_BODY()
public:
	virtual void Init(IEngineLoop* InEngineLoop) override;
};
