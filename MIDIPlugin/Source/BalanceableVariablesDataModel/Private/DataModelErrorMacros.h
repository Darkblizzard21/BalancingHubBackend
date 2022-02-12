#pragma once

/** A collection of error feedback generation macros.
 * Used by the Data model to give consistent error messages for the same error occuring in different functions.
 * TDataModelFeedback<T> is a type struct therefore all macros take in a FeedbackType used in the current function.
 * Error Feedbacks do not allow excess to the feedbackType so no value for it is needed.
 */

// Begin: Composition errors
#define COMPOSITION_NOT_EXISTS_ERROR(FeedbackType) TDataModelFeedback<FeedbackType>::CreateErrorFeedback(\
		EDataModelError::IdentifierNotPresentInData,\
		"Composition \"" + CompositionName + "\" dose not exists.")

#define COMPOSITION_ALREADY_EXISTS_ERROR(FeedbackType) TDataModelFeedback<FeedbackType>::CreateErrorFeedback(\
		EDataModelError::IdentifierAlreadyExists,\
		"Composition \"" + CompositionName + "\" already exists")
// End: Composition errors

// Begin: Balancing errors
#define BALANCING_NOT_EXISTS_ERROR(FeedbackType) TDataModelFeedback<FeedbackType>::CreateErrorFeedback(\
		EDataModelError::IdentifierNotPresentInData,\
		"Balancing \"" + BalancingName + "\"dose not exists.")


#define BALANCING_ALREADY_EXISTS_ERROR(FeedbackType) TDataModelFeedback<FeedbackType>::CreateErrorFeedback(\
		EDataModelError::IdentifierAlreadyExists,\
		"Balancing \"" + BalancingName + "\" already exists.")

#define CANT_REMOVE_LAST_BALANCING(FeedbackType) TDataModelFeedback<FeedbackType>::CreateErrorFeedback(\
		EDataModelError::CantRemoveElement,\
		"Balancing \"" + BalancingName + "\" is the last balancing of the Composition.\"" + CompositionName +\
		"\". \n Remove the composition instead.")

#define CANT_REMOVE_ACTIVE_BALANCING(FeedbackType) TDataModelFeedback<FeedbackType>::CreateErrorFeedback(\
	EDataModelError::CantRemoveElement,\
	"Balancing \"" + BalancingName + "\" is the active balancing of the Composition.\"" + CompositionName +\
	"\". \n Set Another as active before removal.")
// End: Balancing errors

// Begin: Variable errors
#define VARIABLE_NOT_AVAILABLE(FeedbackType) TDataModelFeedback<FeedbackType>::CreateErrorFeedback(\
		EDataModelError::IdentifierNotPresentInData,\
		"The Variable \"" + DefaultData.GetName() + "\" is not available for binding.")


#define VARIABLE_NOT_PRESENT_ERROR(FeedbackType)  TDataModelFeedback<FeedbackType>::CreateErrorFeedback(\
		EDataModelError::IdentifierNotPresentInData,\
		"Variable \"" + Id.GetName() + "\" is not present in Balancing \"" + BalancingName + "\"")

#define VARIABLE_NOT_BOUND_TO_COMPOSITION_ERROR(FeedbackType) TDataModelFeedback<FeedbackType>::CreateErrorFeedback(\
		EDataModelError::IdentifierNotPresentInData,\
		"Variable \"" + Id.GetName() + "\" is not bound to Composition \"" + CompositionName + "\"")
// End: Variable errors

