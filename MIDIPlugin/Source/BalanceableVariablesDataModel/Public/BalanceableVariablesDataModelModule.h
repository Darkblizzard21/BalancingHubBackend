// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DataModelBackendLinked.h"
#include "IDataModel.h"
#include "Modules/ModuleManager.h"


class FBalanceableVariablesDataModelModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	BALANCEABLEVARIABLESDATAMODEL_API TWeakPtr<IIDataModel> GetDataModelInstance() const;
private:
	TSharedPtr<FDataModelBackendLinked> DataModel;
	bool bInit = false;
};
