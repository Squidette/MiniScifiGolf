// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultWidget.h"

#include "FieldGameMode.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UResultWidget::OnEndButtonClicked()
{
	if (UWorld* World = GetWorld())
	{
		APlayerController* PC = World->GetFirstPlayerController();
		UKismetSystemLibrary::QuitGame(World, PC, EQuitPreference::Quit, true);
	}
}

void UResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameModeBase = Cast<AFieldGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GameModeBase)
	{
		UE_LOG(LogTemp, Error, TEXT("UResultWidget가 게임모드베이스를 찾지 못했어요"));
	}

	if (EndButton)
	{
		EndButton->OnClicked.AddDynamic(this, &UResultWidget::OnEndButtonClicked);
	}
}

void UResultWidget::Show()
{
	// if (GameModeBase)
	// {
	// 	FString name = GameModeBase->GetName();
	// 	MapName->SetText(FText::FromString(name));
	//
	// 	FString par = FString::Printf(TEXT("PAR %d"), GameModeBase->GetFieldPar());
	// 	Par->SetText(FText::FromString(par));
	//
	// 	int32 result = GameModeBase->GetCurrentShots() - GameModeBase->GetFieldPar();
	// 	FString resultString = FString::Printf(TEXT("%d(%d)"), GameModeBase->GetCurrentShots(), result);
	// 	YourScore->SetText(FText::FromString(resultString));
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("UResultWidget가 게임모드베이스를 찾지 못함"));
	// }

	this->SetVisibility(ESlateVisibility::Visible);
}

void UResultWidget::UnShow()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}