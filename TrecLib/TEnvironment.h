#pragma once
#include "TString.h"
#include "TDataArray.h"
#include "TDataMap.h"
#include "TFile.h"

class _TREC_LIB_DLL TEnvironment :
    public TObject
{
    TDataArray<TrecPointer<TEnvironment>> childEnvironments;
protected:
    TDataMap<TrecPointer<TVariable>> properties;
    virtual TrecPointer<TObject> RetrieveResource(const TString& name) = 0;
    virtual void RetrieveResourceListSub(TDataArray<TrecPointer<TVariable>>& resources) = 0;
public:
    TEnvironment() = default;

    virtual void SaveProperties();

    ObjectType GetObjectType()final;

    void RetrieveResourceList(TDataArray<TrecPointer<TVariable>>& resources);

    void AddEnvironment(TrecPointer<TEnvironment> childEnvironment);

    virtual TrecPointer<TObject> GetResource(const TString& name);

    TrecPointer<TVariable> GetProperty(const TString& name)const ;

    void SetProperty(const TString& name, TrecPointer<TVariable> var, bool doOverride = false);

    TrecPointer<TEnvironment> GetChildEnvironmentAt(UINT index) const;

    UINT ChildEnvironmentCount()const;
};


class _TREC_LIB_DLL TEnvironmentBuilder : public TCoreObject
{
public:
    virtual TString GetName() = 0;
    /**
     * Reports the name of the Environment that can support the requested Resource type (empty string if the resource is not supported)
     */
    virtual TString SupportsResource(const TString& resource) = 0;
    /**
     * Report the name of the Environments, resource TYpes, and Resuorces that can manage that File Type (strings should use 2 semicolons 
     * as a separater i.e. '[Environment Name];[Resource Type];[Resource Name]'
     */
    virtual void SupportsFileType(TDataArray<TString>& envResources) = 0;

    virtual TrecPointer<TEnvironment> BuildEnvironment(TString& name) = 0;

    virtual TrecPointer<TEnvironment> BuildProjectEnvironment(TString& name, TString& projectName, TrecActivePointer<TFileShell> directory);

    virtual void GetProjectTypes(TDataArray<TString>& types) = 0;

    virtual void GetProjectTypes(TDataArray<TString>& types, TrecActivePointer<TDirectory> workspace) = 0;

    virtual void SupportsFileType(TDataArray<TString>& types, const TString& ext) = 0;
};
