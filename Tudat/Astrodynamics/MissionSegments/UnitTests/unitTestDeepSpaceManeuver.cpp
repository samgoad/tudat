/*    Copyright (c) 2010-2012 Delft University of Technology.
 *
 *    This software is protected by national and international copyright.
 *    Any unauthorized use, reproduction or modification is unlawful and
 *    will be prosecuted. Commercial and non-private application of the
 *    software in any form is strictly prohibited unless otherwise granted
 *    by the authors.
 *
 *    The code is provided without any warranty; without even the implied
 *    warranty of merchantibility or fitness for a particular purpose.
 *
 *    Changelog
 *      YYMMDD    Author            Comment
 *      110527    K. Kumar        First creation of the code.
 *
 *    References
 *
 */

// Temporary notes (move to class/function doxygen):
// Test runs code and verifies result against expected value.
// If the tested code is erroneous, the test function returns a boolean
// true; if the code is correct, the function returns a boolean false.
// 

#include <cmath>
#include <iostream>
#include <limits>
#include "Tudat/Astrodynamics/MissionSegments/deepSpaceManeuver.h"
#include "Tudat/Astrodynamics/States/cartesianElements.h"

//! Test deep space maneuver.
int main( )
{
    // Using declarations.
    using std::cerr;
    using std::endl;
    using namespace tudat;

    // Summary of tests.
    // Test 1: Test setTime( ) and getTime( ) functions.
    // Test 2: Test setState( ) and getState( ) functions with Cartesian elements state.

    // Test result initialised to false.
    bool isDeepSpaceManeuverErroneous_ = false;

    // Create DSM object.
    DeepSpaceManeuver deepSpaceManeuver_;

    // Test 1: Test setTime( ) and getTime( ) functions.
    // Declare and initialize time of DSM.
    double timeOfDeepSpaceManeuver_ = 123.3;

    // Set time of DSM.
    deepSpaceManeuver_.setTime( timeOfDeepSpaceManeuver_ );

    // Test if getTime( ) function results in set time for DSM.
    if ( std::fabs( timeOfDeepSpaceManeuver_ - deepSpaceManeuver_.getTime( ) )
         > std::numeric_limits< double >::epsilon( ) )
    {
        cerr << "The setTime( )/getTime( ) functions do not work properly." << endl;

        isDeepSpaceManeuverErroneous_ = true;
    }

    // Test 2: Test setState( ) and getState( ) functions with Cartesian elements
    //         state.
    // Declare and initialize Cartesian elements state for DSM.
    CartesianElements deepSpaceManeuverState_;
    deepSpaceManeuverState_.setCartesianElementX( 1000.3 );
    deepSpaceManeuverState_.setCartesianElementY( 0.0 );
    deepSpaceManeuverState_.setCartesianElementZ( 613.41 );
    deepSpaceManeuverState_.setCartesianElementXDot( 2300.5 );
    deepSpaceManeuverState_.setCartesianElementYDot( 100.34 );
    deepSpaceManeuverState_.setCartesianElementZ( 0.0 );

    // Set DSM state.
    deepSpaceManeuver_.setState( &deepSpaceManeuverState_ );

    // Test if getState( ) function results in set state for DSM.
    if ( std::fabs( deepSpaceManeuverState_.state.norm( )
                    - deepSpaceManeuver_.getState( )->state.norm( ) )
         / deepSpaceManeuverState_.state.norm( )
         > std::numeric_limits< double >::epsilon( ) )
    {
        cerr << "The setState( )/getState( ) functions do not work properly." << endl;
        isDeepSpaceManeuverErroneous_ = true;
    }

    // Test 1: Test setDeltaV( ) and getDeltaV( ) functions.
    // Declare and initialize deltaV for DSM.
    double deltaVOfDeepSpaceManeuver_ = 254.78;

    // Set deltaV of DSM.
    deepSpaceManeuver_.setDeltaV( deltaVOfDeepSpaceManeuver_ );

    // Test if getDeltaV( ) function results in set deltaV for DSM.
    if ( std::fabs( deltaVOfDeepSpaceManeuver_ - deepSpaceManeuver_.getDeltaV( ) )
         > std::numeric_limits< double >::epsilon( ) )
    {
        cerr << "The setDeltaV( )/getDeltaV( ) functions do not work properly." << endl;
        isDeepSpaceManeuverErroneous_ = true;
    }

    // Return test result.
    // If test is successful return false; if test fails, return true.
    if ( isDeepSpaceManeuverErroneous_ )
    {
        cerr << "testDeepSpaceManeuver failed!" << endl;
    }

    return isDeepSpaceManeuverErroneous_;
}