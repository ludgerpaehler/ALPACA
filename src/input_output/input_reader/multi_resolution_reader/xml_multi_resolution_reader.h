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
#ifndef XML_MULTI_RESOLUTION_READER_H
#define XML_MULTI_RESOLUTION_READER_H

#include <memory>

#include <tinyxml2.h>
#include "input_output/input_reader/multi_resolution_reader/multi_resolution_reader.h"

/**
 * @brief Class that implements the actual reading procedure of multiresolution data from input files of xml-type.
 *        Here, no consistency checks of the read parameter are done. Only the validity of the correct variable type 
 *        (double, int, string) is done.  
 */
class XmlMultiResolutionReader : public MultiResolutionReader {

   // The already openend xml input file (must be shared pointer to distribute input file on different readers)
   std::shared_ptr<tinyxml2::XMLDocument const> const xml_input_file_;

   // Functions that are required from base class 
   double DoReadNodeSizeOnLevelZero() const override;
   int DoReadNumberOfNodes( Direction const direction ) const override;
   int DoReadMaximumLevel() const override;
   double DoReadEpsilonReference() const override;
   int DoReadEpsilonLevelReference() const override;

public:
   XmlMultiResolutionReader() = delete;
   explicit XmlMultiResolutionReader( std::shared_ptr<tinyxml2::XMLDocument> inputfile );
   ~XmlMultiResolutionReader() = default;
   XmlMultiResolutionReader( XmlMultiResolutionReader const& ) = delete;
   XmlMultiResolutionReader& operator=( XmlMultiResolutionReader const& ) = delete;
   XmlMultiResolutionReader( XmlMultiResolutionReader&& ) = delete;
   XmlMultiResolutionReader& operator=( XmlMultiResolutionReader&& ) = delete;
};

#endif // XML_MULTI_RESOLUTION_READER_H