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
#ifndef CROSS_SHEAR_VISCOSITY_MODEL_H
#define CROSS_SHEAR_VISCOSITY_MODEL_H

#include "unit_handler.h"
#include "parameter/material_parameter_models/shear_rate_material_parameter_model.h"

/**
 * @brief Class that provides the actual model implementation param = f(gamma_dot) following the Cross model. for a shear-rate based material parameter model to compute
 *        the shear viscosity in the field. The model is implemented based on \cite{Cross1965}.
 */
class CrossShearViscosityModel : public ShearRateMaterialParameterModel<CrossShearViscosityModel> {

   // Defines the friend to be used for the computation indicating the variables this model depends on (e.g., prime states)
   friend ShearRateMaterialParameterModel;

   // Definition of parameters needed in the base class for the computations
   static constexpr Parameter parameter_buffer_type_ = Parameter::ShearViscosity; // buffer that is modified by the model
   static constexpr DerivativeStencils derivative_stencil_ = viscous_fluxes_derivative_stencil_cell_center; // stencil required for the shear rate

   // member variables needed for the model calculation
   double const mu_infinite_shear_rates_;
   double const mu_zero_shear_rates_;
   double const power_law_exponent_;
   double const shear_rate_mu_half_;
   // derived model parameter (to avoid multiple computations)
   double const mu_zero_minus_infinite_;
   double const one_shear_rate_mu_half_;

   // Actual computation of shear viscosity
   double ComputeParameter( double const shear_rate ) const;

public:
   CrossShearViscosityModel() = delete;
   explicit CrossShearViscosityModel( std::unordered_map<std::string, double> const& dimensional_parameter_map,
                                      UnitHandler const& unit_handler );
   virtual ~CrossShearViscosityModel() = default;
   CrossShearViscosityModel( const CrossShearViscosityModel& ) = delete;
   CrossShearViscosityModel& operator=( const CrossShearViscosityModel& ) = delete;
   CrossShearViscosityModel( CrossShearViscosityModel&& ) = delete;
   CrossShearViscosityModel& operator=( CrossShearViscosityModel&& ) = delete;

   // logging function
   std::string GetLogData( unsigned int const indent, UnitHandler const& unit_handler ) const;
};

#endif // CROSS_SHEAR_VISCOSITY_MODEL_H