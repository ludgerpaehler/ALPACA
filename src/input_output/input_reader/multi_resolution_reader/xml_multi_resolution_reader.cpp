/*****************************************************************************************
*                                                                                        *
* This file is part of ALPACA                                                            *
*                                                                                        *
******************************************************************************************
*                                                                                        *
*  \\                                                                                    *
*  l '>                                                                                  *
*  | |                                                                                   *
*  | |                                                                                   *
*  | alpaca~                                                                             *
*  ||    ||                                                                              *
*  ''    ''                                                                              *
*                                                                                        *
* ALPACA is a MPI-parallelized C++ code framework to simulate compressible multiphase    *
* flow physics. It allows for advanced high-resolution sharp-interface modeling          *
* empowered with efficient multiresolution compression. The modular code structure       *
* offers a broad flexibility to select among many most-recent numerical methods covering *
* WENO/T-ENO, Riemann solvers (complete/incomplete), strong-stability preserving Runge-  *
* Kutta time integration schemes, level set methods and many more.                       *
*                                                                                        *
* This code is developed by the 'Nanoshock group' at the Chair of Aerodynamics and       *
* Fluid Mechanics, Technical University of Munich.                                       *
*                                                                                        *
******************************************************************************************
*                                                                                        *
* LICENSE                                                                                *
*                                                                                        *
* ALPACA - Adaptive Level-set PArallel Code Alpaca                                       *
* Copyright (C) 2020 Nikolaus A. Adams and contributors (see AUTHORS list)               *
*                                                                                        *
* This program is free software: you can redistribute it and/or modify it under          *
* the terms of the GNU General Public License as published by the Free Software          *
* Foundation version 3.                                                                  *
*                                                                                        *
* This program is distributed in the hope that it will be useful, but WITHOUT ANY        *
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A        *
* PARTICULAR PURPOSE. See the GNU General Public License for more details.               *
*                                                                                        *
* You should have received a copy of the GNU General Public License along with           *
* this program (gpl-3.0.txt).  If not, see <https://www.gnu.org/licenses/gpl-3.0.html>   *
*                                                                                        *
******************************************************************************************
*                                                                                        *
* THIRD-PARTY tools                                                                      *
*                                                                                        *
* Please note, several third-party tools are used by ALPACA. These tools are not shipped *
* with ALPACA but available as git submodule (directing to their own repositories).      *
* All used third-party tools are released under open-source licences, see their own      *
* license agreement in 3rdParty/ for further details.                                    *
*                                                                                        *
* 1. tiny_xml           : See LICENSE_TINY_XML.txt for more information.                 *
* 2. expression_toolkit : See LICENSE_EXPRESSION_TOOLKIT.txt for more information.       *
* 3. FakeIt             : See LICENSE_FAKEIT.txt for more information                    *
* 4. Catch2             : See LICENSE_CATCH2.txt for more information                    *
*                                                                                        *
******************************************************************************************
*                                                                                        *
* CONTACT                                                                                *
*                                                                                        *
* nanoshock@aer.mw.tum.de                                                                *
*                                                                                        *
******************************************************************************************
*                                                                                        *
* Munich, July 1st, 2020                                                                 *
*                                                                                        *
*****************************************************************************************/
#include "input_output/input_reader/multi_resolution_reader/xml_multi_resolution_reader.h"

#include "user_specifications/compile_time_constants.h"
#include "input_output/input_reader/xml_utilities.h"

XmlMultiResolutionReader::XmlMultiResolutionReader( std::shared_ptr<tinyxml2::XMLDocument> inputfile ) :
   MultiResolutionReader(), 
   xml_input_file_( std::move( inputfile ) ) {
   /** Empty besides initializer list and base class constructor call */ 
}


/**
 * @brief Reads the size of a node on level zero
 * @return node size on level zero
 */
double XmlMultiResolutionReader::DoReadNodeSizeOnLevelZero() const {
   // Obtain correct node
   tinyxml2::XMLElement const* node = XmlUtilities::GetChild( *xml_input_file_, { "configuration", "domain", "nodeSize" } );    
   return XmlUtilities::ReadDouble( node );
}

/**
 * @brief Reads the number of nodes on level zero for a given direction
 * @param direction Direction identifier for which the number of nodes should be read
 * @return number of nodes 
 */
int XmlMultiResolutionReader::DoReadNumberOfNodes( Direction const direction ) const {
   // Get the correct component name 
   std::string const component = direction == Direction::X ? "x" : 
                                 direction == Direction::Y ? "y" :
                                                             "z";
   // obtain correct node
   tinyxml2::XMLElement const* node = XmlUtilities::GetChild( *xml_input_file_, { "configuration", "domain", "nodeRatio", component } );  
   return XmlUtilities::ReadInt( node );
}

/**
 * @brief Reads the maximum level used for the simulation
 * @return maximum level of the simulation
 */
int XmlMultiResolutionReader::DoReadMaximumLevel() const {
   // Obtain correct node
   tinyxml2::XMLElement const* level_node = XmlUtilities::GetChild( *xml_input_file_, { "configuration", "multiResolution", "maximumLevel" } );      
   return XmlUtilities::ReadInt( level_node );
}

/**
 * @brief Reads the reference epsilon value used for the refinement criterion
 * @return epsilon refernce value
 */
int XmlMultiResolutionReader::DoReadEpsilonLevelReference() const {
   // Obtain correct nodes
   tinyxml2::XMLElement const* level_node = XmlUtilities::GetChild( *xml_input_file_, { "configuration", "multiResolution", "refinementCriterion", "levelOfEpsilonReference" } );    
   return XmlUtilities::ReadInt( level_node );
}

/**
 * @brief Reads reference level where the epsilon reference is enforced
 * @return level of reference
 */
double XmlMultiResolutionReader::DoReadEpsilonReference() const {
   // Obtain correct node
   tinyxml2::XMLElement const* epsilon_node = XmlUtilities::GetChild( *xml_input_file_, { "configuration", "multiResolution", "refinementCriterion", "epsilonReference" } );    
   return XmlUtilities::ReadDouble( epsilon_node );
}