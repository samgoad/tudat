/*    Copyright (c) 2010-2017, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 *
 */

#ifndef TUDAT_NUMERICAL_INTEGRATOR_H
#define TUDAT_NUMERICAL_INTEGRATOR_H

#include <iostream>
#include <limits>

#include <boost/function.hpp>

#include <Eigen/Core>

namespace tudat
{
namespace numerical_integrators
{

//! Base class for the numerical integrators.
/*!
 * Base class for numerical integrators.
 * \tparam StateType The type of the state. This type should support addition,
 *          subtraction and assignment operators.
 * \tparam StateDerivativeType The type of the state derivative. This type should support
 *          multiplication with IndependentVariableType and doubles.
 * \tparam IndependentVariableType The type of the independent variable.
 */
template < typename IndependentVariableType = double, typename StateType = Eigen::VectorXd,
           typename StateDerivativeType = StateType, typename TimeStepType = IndependentVariableType >
class NumericalIntegrator
{
public:

    //! Typedef to the state derivative function.
    /*!
     * Typedef to the state derivative function. This should be a pointer to a function or a boost
     * function.
     */
    typedef boost::function< StateDerivativeType(
            const IndependentVariableType, const StateType& ) > StateDerivativeFunction;

    //! Default constructor.
    /*!
     * Default constructor, taking a state derivative function as argument.
     * \param stateDerivativeFunction State derivative function.
     */
    NumericalIntegrator( const StateDerivativeFunction& stateDerivativeFunction ) :
        stateDerivativeFunction_( stateDerivativeFunction ) { }

    //! Default virtual destructor.
    /*!
     * Default virtual destructor.
     */
    virtual ~NumericalIntegrator( ) { }

    //! Get step size of the next step.
    /*!
     * Returns the step size of the next step. Derived classes should override this and provide the
     * last step size that was computed or passed to performIntegrationStep( ).
     * \return Step size to be used for the next step.
     */
    virtual TimeStepType getNextStepSize( ) const = 0;

    //! Get current state.
    /*!
     * Returns the current state of the integrator. Derived classes should override this and
     * provide the computed state by performIntegrationStep( ).
     * \return Current integrated state.
     */
    virtual StateType getCurrentState( ) const = 0;

    //! Get current independent variable.
    /*!
     * Returns the current value of the independent variable of the integrator. Derived classes
     * should override this and provide the computed independent variable by
     * performIntegrationStep().
     * \return Current independent variable.
     */
    virtual IndependentVariableType getCurrentIndependentVariable( ) const = 0;

    //! Rollback internal state to the step performed by performIntegrationStep()
    /*!
     * Performs rollback of internal state to the step performed by performIntegrationStep(). This
     * is not necessarily equal the start of the integration interval after integrateTo() has been.
     * called. This function can only be called once after calling or performIntegrationStep()
     * unless specified otherwise by implementations, and can not be called before integrateTo()
     * has been called.
     * Will return true if the rollback was successful, and false otherwise.
     * \return True if the rollback was successful.
     */
    virtual bool rollbackToPreviousState( ) = 0;

    //! Perform an integration to a specified independent variable value.
    /*!
     * Performs an integration to independentVariableEnd with initial state and initial independent
     * variable value specified by the current state of the integrator and the current independent
     * variable value. This implementation of integrateTo chooses the final step size such that it
     * exactly coincides with the given independentVariableEnd.
     * \param intervalEnd The value of the independent variable at the end of the interval to
     *          integrate over.
     * \param initialStepSize The initial step size to use.
     * \param finalTimeTolerance Tolerance to within which the final time should be reached.
     * \return The state at independentVariableEnd.
     */
    virtual StateType integrateTo(
            const IndependentVariableType intervalEnd,
            const TimeStepType initialStepSize,
            const TimeStepType finalTimeTolerance = std::numeric_limits< TimeStepType >::epsilon( )  );

    //! Perform a single integration step.
    /*!
     * Performs a single integration step from current independent variable and state as specified
     * by initialStateHistory and specified stepSize. This function should determine the next step
     * and make it available to getNextStepSize(), return the current state and store it for
     * getCurrentState(), and store the current independent variable value for
     * getCurrentIndependentVariable().
     * \param stepSize The step size of this step.
     * \return The state at the end of the interval.
     */
    virtual StateType performIntegrationStep( const TimeStepType stepSize ) = 0;

    //! Function to return the function that computes and returns the state derivative
    /*!
     * Function to return the function that computes and returns the state derivative
     * \return Function that returns the state derivative
     */
    StateDerivativeFunction getStateDerivativeFunction( )
    {
        return stateDerivativeFunction_;
    }

protected:

    //! Function that returns the state derivative.
    /*!
     * Function that returns the state derivative, as passed to the constructor.
     */
    StateDerivativeFunction stateDerivativeFunction_;

};

//! Perform an integration to a specified independent variable value.
template < typename IndependentVariableType, typename StateType, typename StateDerivativeType, typename TimeStepType >
StateType NumericalIntegrator< IndependentVariableType, StateType, StateDerivativeType, TimeStepType >::integrateTo(
        const IndependentVariableType intervalEnd,
        const TimeStepType initialStepSize,
        const TimeStepType finalTimeTolerance )
{
    TimeStepType stepSize = initialStepSize;

    // Flag to indicate that the integration end value of the independent variable has been
    // reached.
    bool atIntegrationIntervalEnd = static_cast< TimeStepType >( intervalEnd - getCurrentIndependentVariable( ) )
            * stepSize / std::fabs( stepSize )
            <= finalTimeTolerance;

    int loopCounter = 0;
    while ( !atIntegrationIntervalEnd )
    {
        // Check if the remaining interval is smaller than the step size.
        if ( std::fabs( static_cast< TimeStepType >( intervalEnd - getCurrentIndependentVariable( ) ) )
             <= std::fabs( stepSize ) *
             ( 1.0 + finalTimeTolerance ) )
        {
            // The next step is beyond the end of the integration interval, so adjust the
            // step size accordingly.
            stepSize = intervalEnd - getCurrentIndependentVariable( );

            // Explicitly flag that the integration interval end is reached. Due to rounding
            // off errors, it may not be possible to use
            // ( currentIndependentVariable >= independentVariableEnd ) // in the while condition.
            atIntegrationIntervalEnd = true;

        }

        // Perform the step.
        performIntegrationStep( stepSize );
        stepSize = getNextStepSize( );

        // Only applicable to adaptive step size methods:
        // Perform additional step(s) to reach intervalEnd exactly, in case the last step was rejected by
        // the variable step size routine and a new step was used that was too small to get to intervalEnd.
        if ( atIntegrationIntervalEnd )
        {
            // As long as intervalEnd is not reached, perform additional steps with the remaining time
            // as suggested step size for the variable step size routine.
            if( std::fabs( static_cast< TimeStepType >( intervalEnd - getCurrentIndependentVariable( ) ) ) >
                    finalTimeTolerance )
            {
                // Ensure that integrateTo function does not get stuck in a loop.
                if( loopCounter < 1000 )
                {
                    atIntegrationIntervalEnd = false;
                    loopCounter++;
                }
                else
                {
                    std::cerr<<"Warning, integrateTo function has failed to converge to final time to within tolerances, difference between true and requested final time is "<<
                        intervalEnd - getCurrentIndependentVariable( ) <<", final time is: "<<
                               getCurrentIndependentVariable( )<<std::endl;
                }
            }
        }
    }

    return getCurrentState( );
}

//! Typedef for shared-pointer to default numerical integrator.
/*!
 * Typedef for shared-pointer to a default numerical integrator (IndependentVariableType = double,
 * StateType = Eigen::VectorXd, StateDerivativeType = Eigen::VectorXd).
 */
typedef boost::shared_ptr< NumericalIntegrator< > > NumericalIntegratorXdPointer;

//! Typedef for a shared-pointer to a scalar numerical integrator.
/*!
 * Typedef for shared-pointer to a scalar numerical integrator (IndependentVariableType = double,
 * StateType = double, StateDerivativeType = double).
 */
typedef boost::shared_ptr< NumericalIntegrator< double, double, double > >
NumericalIntegratordPointer;

} // namespace numerical_integrators
} // namespace tudat

#endif // TUDAT_NUMERICAL_INTEGRATOR_H