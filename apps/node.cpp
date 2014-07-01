// =============================================================================
/*!
 * \file       apps/node.cpp
 *
 * This file contains the implementation of the Node class that represents the
 * HAN-FUN Node on the example application.
 *
 * \author     Filipe Alves <filipe.alves@bithium.com>
 *
 * \version    0.3.0
 *
 * \copyright  Copyright &copy; &nbsp; 2014 Bithium S.A.
 */
// =============================================================================
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "node.h"

// =============================================================================
// Simple Light
// =============================================================================

void SimpleLight::on ()
{
   std::cout << "[HANFUN] >>>>>>>>>>>>> SimpleLight : ON <<<<<<<<<<<<<" << std::endl;
}

void SimpleLight::off ()
{
   std::cout << "[HANFUN] >>>>>>>>>>>>> SimpleLight : OFF <<<<<<<<<<<<<" << std::endl;
}

void SimpleLight::toggle ()
{
   std::cout << "[HANFUN] >>>>>>>>>>>>> SimpleLight : TOGGLE <<<<<<<<<<<<<" << std::endl;
}
