#include <AnagameEnvironment.h>

int main()
{
	TrecPointer<BasicAnagameEnvironment> basicAnagameEnvironment = TrecPointerKey::GetNewTrecPointer< BasicAnagameEnvironment>();
	//TString source(L"anagame"), title(L"hierarchy"), name(L"files");
	basicAnagameEnvironment->GetResource(L"");
}