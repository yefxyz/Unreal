// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "NPCAIController.generated.h"

UCLASS()
/// <summary>
/// Class ANPCAIController.
/// </summary>
/// <seealso cref="AAIController" />
class VRTESTCPP_API ANPCAIController : public AAIController {
	GENERATED_BODY()

public:	
	/// <summary>
	/// Called when [move completed].
	/// </summary>
	/// <param name="RequestID">The request identifier.</param>
	/// <param name="Result">The result.</param>
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

};
