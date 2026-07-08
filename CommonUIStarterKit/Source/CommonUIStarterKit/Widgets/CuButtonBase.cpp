// CuButtonBase.cpp
#include "Widgets/CuButtonBase.h"

#include "CommonTextBlock.h"

void UCuButtonBase::SetButtonText(FText InText)
{
	ButtonText = InText;
	RefreshButtonText();
}

void UCuButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	RefreshButtonText();
}

void UCuButtonBase::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();
	RefreshButtonText();
}

void UCuButtonBase::RefreshButtonText()
{
	if (Text_Label)
	{
		Text_Label->SetText(ButtonText);
	}
}
