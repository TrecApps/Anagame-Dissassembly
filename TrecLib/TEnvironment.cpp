
#include "TEnvironment.h"

TObject::ObjectType TEnvironment::GetObjectType()
{
    return ObjectType::ot_environment;
}

void TEnvironment::SaveProperties()
{

}

void TEnvironment::RetrieveResourceList(TDataArray<TrecPointer<TVariable>>& resources)
{
    RetrieveResourceListSub(resources);
    for (UINT Rust = 0; Rust < childEnvironments.Size(); Rust++)
        childEnvironments[Rust]->RetrieveResourceList(resources);
}

void TEnvironment::AddEnvironment(TrecPointer<TEnvironment> childEnvironment)
{
    if (childEnvironment.Get())
        this->childEnvironments.push_back(childEnvironment);
}

TrecPointer<TObject> TEnvironment::GetResource(const TString& name)
{
    TrecPointer<TObject> ret = RetrieveResource(name);
    for (UINT Rust = 0; !ret.Get() && Rust < childEnvironments.Size(); Rust++)
        ret = childEnvironments[Rust]->GetResource(name);
    return ret;
}

TrecPointer<TVariable> TEnvironment::GetProperty(const TString& name) const
{
    TrecPointer<TVariable> ret;
    if (!properties.retrieveEntry(name, ret))
    {
        for (UINT Rust = 0; !ret.Get() && Rust < childEnvironments.Size(); Rust++)
            ret = childEnvironments[Rust]->GetProperty(name);
    }
    return ret;
}

void TEnvironment::SetProperty(const TString& name, TrecPointer<TVariable> var, bool doOverride)
{
    TrecPointer<TVariable> entry;
    if (!doOverride && properties.retrieveEntry(name, entry))
        return;

    properties.setEntry(name, var);
}

TrecPointer<TEnvironment> TEnvironment::GetChildEnvironmentAt(UINT index) const
{
    if(index >= childEnvironments.Size())
        return TrecPointer<TEnvironment>();
    return childEnvironments[index];
}

UINT TEnvironment::ChildEnvironmentCount() const
{
    return childEnvironments.Size();
}

TrecPointer<TEnvironment> TEnvironmentBuilder::BuildProjectEnvironment(TString& name, TString& projectName, TrecActivePointer<TFileShell> directory)
{
    return TrecPointer<TEnvironment>();
}
