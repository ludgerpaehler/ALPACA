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
#include "interface_block.h"
#include <stdexcept>
#include "utilities/buffer_operations.h"

/**
 * @brief Constructor to create a interface block according to an already computed levelset field.
 * @param levelset_initial Reference to Array holding the levelset field to be in this interface block.
 */
InterfaceBlock::InterfaceBlock( double const (&levelset_initial)[CC::TCX()][CC::TCY()][CC::TCZ()] ) {

   // In the base buffer all values are set to zero
   BufferOperations::SetFieldBuffer( GetBaseBuffer(), 0.0 );
   // In the right-hand and reinitialized buffer the levelset is copied and volume fraction is set to zero
   BufferOperations::CopySingleBuffer( levelset_initial, GetRightHandSideBuffer( InterfaceDescription::Levelset ) );
   BufferOperations::CopySingleBuffer( levelset_initial, GetReinitializedBuffer( InterfaceDescription::Levelset ) );
   BufferOperations::SetSingleBuffer( GetRightHandSideBuffer( InterfaceDescription::VolumeFraction ), 0.0 );
   BufferOperations::SetSingleBuffer( GetReinitializedBuffer( InterfaceDescription::VolumeFraction ), 0.0 );
   // In the initial buffer all values are set to zero
   BufferOperations::SetFieldBuffer( GetInitialBuffer(), 0.0 );
   // In the interface state buffer all values are set to zero
   BufferOperations::SetFieldBuffer( GetInterfaceStateBuffer(), 0.0 );

   // Only set buffer of parameter to zero if they are present
   if constexpr(CC::InterfaceParameterModelActive()) {
      BufferOperations::SetFieldBuffer( GetInterfaceParameterBuffer(), 0.0 );
   }
}

/**
 * @brief Constructor to create an initial homogenous levelset field on the interface block.
 * @param levelset_initial The value to be imposed as levelset function.
 */
InterfaceBlock::InterfaceBlock( double const levelset_initial ) {

   // In the base buffer only the volume fraction is set to uniform values, the levelset is set to zero
   BufferOperations::SetSingleBuffer( GetBaseBuffer( InterfaceDescription::Levelset ), 0.0 );
   BufferOperations::SetSingleBuffer( GetBaseBuffer( InterfaceDescription::VolumeFraction ), levelset_initial > 0 ? 1.0 : 0.0 );
   // In the right-hand and reinitialized buffer the levelset is set to uniform given value and volume fraction is set to zero
   BufferOperations::SetSingleBuffer( GetRightHandSideBuffer( InterfaceDescription::Levelset ), levelset_initial );
   BufferOperations::SetSingleBuffer( GetReinitializedBuffer( InterfaceDescription::Levelset ), levelset_initial );
   BufferOperations::SetSingleBuffer( GetRightHandSideBuffer( InterfaceDescription::VolumeFraction ), 0.0 );
   BufferOperations::SetSingleBuffer( GetReinitializedBuffer( InterfaceDescription::VolumeFraction ), 0.0 );
   // In the initial buffer all values are set to zero
   BufferOperations::SetFieldBuffer( GetInitialBuffer(), 0.0 );
   // In the interface state buffer all values are set to zero
   BufferOperations::SetFieldBuffer( GetInterfaceStateBuffer(), 0.0 );

   // Only set buffer of parameter to zero if they are present
   if constexpr(CC::InterfaceParameterModelActive()) {
      BufferOperations::SetFieldBuffer( GetInterfaceParameterBuffer(), 0.0 );
   }
}

/**
 * @brief Gives a reference to the corresponding buffer.
 * @param field_type The interface field type of the buffer.
 * @param field_index The index of the field asked for.
 * @param buffer_type If a interface field is wanted, the interface buffer type. Defaults to InterfaceDescriptionBufferType::RightHandSide.
 * @return Reference to Array that is the requested buffer.
 */
auto InterfaceBlock::GetFieldBuffer( InterfaceFieldType const field_type, unsigned int const field_index, InterfaceDescriptionBufferType const buffer_type ) -> double (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   switch( field_type ) {
      case InterfaceFieldType::Description : {
         return GetInterfaceDescriptionBuffer( buffer_type )[field_index];
      }
      case InterfaceFieldType::Parameters : {
         return parameters_[field_index];
      }
      default : {// InterfaceFieldType::States:
         return states_[field_index];
      }
   }
}

/**
 * @brief Const overload.
 */
auto InterfaceBlock::GetFieldBuffer( InterfaceFieldType const field_type, unsigned int const field_index, InterfaceDescriptionBufferType const buffer_type ) const -> double const (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   switch( field_type ) {
      case InterfaceFieldType::Description : {
         return GetInterfaceDescriptionBuffer( buffer_type )[field_index];
      }
      case InterfaceFieldType::Parameters : {
         return parameters_[field_index];
      }
      default : { // InterfaceFieldType::States:
         return states_[field_index];
      }
   }
}

/**
 * @brief Gives a reference to the corresponding Base buffer.
 * @param interface_description Decider which buffer is to be returned.
 * @return Reference to the array, that is the requested buffer.
 */
auto InterfaceBlock::GetBaseBuffer( InterfaceDescription const interface_description ) -> double (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   return base_[interface_description];
}

/**
 * @brief Const overload.
 */
auto InterfaceBlock::GetBaseBuffer( InterfaceDescription const interface_description ) const -> double const (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   return base_[interface_description];
}

/**
 * @brief Gives a Reference to the corresponding right-hand side buffer.
 * @param interface_description Decider which buffer is to be returned.
 * @return Reference to Array that is the requested buffer.
 */
auto InterfaceBlock::GetRightHandSideBuffer( InterfaceDescription const interface_description ) -> double (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   return right_hand_side_[interface_description];
}

/**
 * @brief Const overload.
 */
auto InterfaceBlock::GetRightHandSideBuffer( InterfaceDescription const interface_description ) const -> double const (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   return right_hand_side_[interface_description];
}

/**
 * @brief Gives a Reference to the corresponding reinitialized buffer.
 * @param interface_description Decider which buffer is to be returned.
 * @return Reference to Array that is the requested buffer.
 */
auto InterfaceBlock::GetReinitializedBuffer( InterfaceDescription const interface_description ) -> double (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   return reinitialized_[interface_description];
}

/**
 * @brief Const overload.
 */
auto InterfaceBlock::GetReinitializedBuffer( InterfaceDescription const interface_description ) const -> double const (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   return reinitialized_[interface_description];
}

/**
 * @brief Gives a Reference to the corresponding initial buffer.
 * @param interface_description Decider which buffer is to be returned.
 * @return Reference to Array that is the requested buffer.
 */
auto InterfaceBlock::GetInitialBuffer( InterfaceDescription const interface_description ) -> double (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   return initial_[interface_description];
}

/**
 * @brief Const overload.
 */
auto InterfaceBlock::GetInitialBuffer( InterfaceDescription const interface_description ) const -> double const (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   return initial_[interface_description];
}

/**
 * @brief Wrapper function that returns an interface description buffer (base, right-hand side, reinitialized or initial). The decision is
 * made based on the template parameter.
 * Implementation for the average buffer.
 * @return base interface description struct
 */
template<>
InterfaceDescriptions& InterfaceBlock::GetInterfaceDescriptionBuffer<InterfaceDescriptionBufferType::Base>() {
   return GetBaseBuffer();
}

/**
 * @brief Const overload.
 */
template<>
InterfaceDescriptions const& InterfaceBlock::GetInterfaceDescriptionBuffer<InterfaceDescriptionBufferType::Base>() const {
   return GetBaseBuffer();
}

/**
 * @brief Wrapper function that returns a interface description buffer (base, right-hand side, reinitialized or initial). The decision is
 * made based on the template parameter.
 * Implementation for the average buffer.
 * @return right-hand side interface description struct
 */
template<>
InterfaceDescriptions& InterfaceBlock::GetInterfaceDescriptionBuffer<InterfaceDescriptionBufferType::RightHandSide>() {
   return GetRightHandSideBuffer();
}

/**
 * @brief Const overload.
 */
template<>
InterfaceDescriptions const& InterfaceBlock::GetInterfaceDescriptionBuffer<InterfaceDescriptionBufferType::RightHandSide>() const {
   return GetRightHandSideBuffer();
}

/**
 * @brief Wrapper function that returns a interface description buffer (base, right-hand side, reinitialized or initial). The decision is
 * made based on the template parameter.
 * Implementation for the average buffer.
 * @return reinitialized interface description struct
 */
template<>
InterfaceDescriptions& InterfaceBlock::GetInterfaceDescriptionBuffer<InterfaceDescriptionBufferType::Reinitialized>() {
   return GetReinitializedBuffer();
}

/**
 * @brief Const overload.
 */
template<>
InterfaceDescriptions const& InterfaceBlock::GetInterfaceDescriptionBuffer<InterfaceDescriptionBufferType::Reinitialized>() const {
   return GetReinitializedBuffer();
}

/**
 * @brief Wrapper function that returns a interface description buffer (base, right-hand side, reinitialized or initial). The decision is
 * made based on the template parameter.
 * Implementation for the average buffer.
 * @return initial interface description struct
 */
template<>
InterfaceDescriptions& InterfaceBlock::GetInterfaceDescriptionBuffer<InterfaceDescriptionBufferType::Initial>() {
   return GetInitialBuffer();
}

/**
 * @brief Const overload.
 */
template<>
InterfaceDescriptions const& InterfaceBlock::GetInterfaceDescriptionBuffer<InterfaceDescriptionBufferType::Initial>() const {
   return GetInitialBuffer();
}

/**
 * @brief Gives access to the base buffer.
 * @return base buffer struct.
 */
InterfaceDescriptions& InterfaceBlock::GetBaseBuffer() {
   return base_;
}

/**
 * @brief Const overload.
 */
InterfaceDescriptions const& InterfaceBlock::GetBaseBuffer() const {
   return base_;
}

/**
 * @brief Gives access to the right hand side buffer.
 * @return Right hand side buffer struct.
 */
InterfaceDescriptions& InterfaceBlock::GetRightHandSideBuffer() {
   return right_hand_side_;
}

/**
 * @brief Const overload.
 */
InterfaceDescriptions const& InterfaceBlock::GetRightHandSideBuffer() const {
   return right_hand_side_;
}

/**
 * @brief Gives access to the reinitialized buffer.
 * @return Reinitialized buffer struct.
 */
InterfaceDescriptions& InterfaceBlock::GetReinitializedBuffer() {
   return reinitialized_;
}

/**
 * @brief Const overload.
 */
InterfaceDescriptions const& InterfaceBlock::GetReinitializedBuffer() const {
   return reinitialized_;
}

/**
 * @brief Gives access to the initial buffer.
 * @return initial buffer struct.
 */
InterfaceDescriptions& InterfaceBlock::GetInitialBuffer() {
   return initial_;
}

/**
 * @brief Const overload
 */
InterfaceDescriptions const& InterfaceBlock::GetInitialBuffer() const {
   return initial_;
}

/**
 * @brief Gives access to the interface descritpion buffer of given type.
 * @param buffer_type InterfaceDescription type of the buffer asked for.
 * @return buffer struct of given type.
 */
InterfaceDescriptions& InterfaceBlock::GetInterfaceDescriptionBuffer( InterfaceDescriptionBufferType const buffer_type ) {
   switch( buffer_type ) {
      case InterfaceDescriptionBufferType::RightHandSide : {
         return right_hand_side_;
      }
      case InterfaceDescriptionBufferType::Reinitialized : {
         return reinitialized_;
      }
      case InterfaceDescriptionBufferType::Base : {
         return base_;
      }
      default : { // case InterfaceDescriptionBufferType::Initial:
         return initial_;
      }
   }
}

/**
 * @brief Const overload
 */
InterfaceDescriptions const& InterfaceBlock::GetInterfaceDescriptionBuffer( InterfaceDescriptionBufferType const buffer_type ) const {
   switch( buffer_type ) {
      case InterfaceDescriptionBufferType::RightHandSide : {
         return right_hand_side_;
      }
      case InterfaceDescriptionBufferType::Reinitialized : {
         return reinitialized_;
      }
      case InterfaceDescriptionBufferType::Base : {
         return base_;
      }
      default : { // case InterfaceDescriptionBufferType::Initial:
         return initial_;
      }
   }
}

/**
 * @brief Gives a Reference to the corresponding interface state buffer.
 * @param state_type Decider which buffer is to be returned.
 * @return Reference to Array that is the requested buffer.
 */
auto InterfaceBlock::GetInterfaceStateBuffer( InterfaceState const state_type ) -> double (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   return states_[state_type];
}

/**
 * @brief Const overload.
 */
auto InterfaceBlock::GetInterfaceStateBuffer( InterfaceState const state_type ) const -> double const (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   return states_[state_type];
}

/**
 * @brief Gives access to the interface state struct.
 * @return Interface states buffer struct.
 */
InterfaceStates& InterfaceBlock::GetInterfaceStateBuffer() {
   return states_;
}

/**
 * @brief Const overload.
 */
InterfaceStates const& InterfaceBlock::GetInterfaceStateBuffer() const {
   return states_;
}

/**
 * @brief Gives a Reference to the corresponding interface Parameter buffer.
 * @param parameter_type Decider which buffer is to be returned.
 * @return Reference to Array that is the requested buffer.
 */
auto InterfaceBlock::GetInterfaceParameterBuffer( InterfaceParameter const parameter_type ) -> double (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   return parameters_[parameter_type];
}

/**
 * @brief Const overload.
 */
auto InterfaceBlock::GetInterfaceParameterBuffer( InterfaceParameter const parameter_type ) const -> double const (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   return parameters_[parameter_type];
}

/**
 * @brief Gives access to the interface parameters struct.
 * @return Interface parameter buffer struct.
 */
InterfaceParameters& InterfaceBlock::GetInterfaceParameterBuffer() {
   return parameters_;
}

/**
 * @brief Const overload.
 */
InterfaceParameters const& InterfaceBlock::GetInterfaceParameterBuffer() const {
   return parameters_;
}

/**
 * @brief Gives the requested buffer of a specific single interface block buffer.
 * @param buffer_type type of the interface block buffer requested.
 */
auto InterfaceBlock::GetBuffer( InterfaceBlockBufferType const buffer_type ) -> double (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   switch( buffer_type ) {
      // interface descriptions
      case InterfaceBlockBufferType::LevelsetBase : {
         return base_[InterfaceDescription::Levelset];
      }
      case InterfaceBlockBufferType::VolumeFractionBase : {
         return base_[InterfaceDescription::VolumeFraction];
      }
      case InterfaceBlockBufferType::LevelsetRightHandSide : {
         return right_hand_side_[InterfaceDescription::Levelset];
      }
      case InterfaceBlockBufferType::VolumeFractionRightHandSide : {
         return right_hand_side_[InterfaceDescription::VolumeFraction];
      }
      case InterfaceBlockBufferType::LevelsetReinitialized : {
         return reinitialized_[InterfaceDescription::Levelset];
      }
      case InterfaceBlockBufferType::VolumeFractionReinitialized : {
         return reinitialized_[InterfaceDescription::VolumeFraction];
      }
      // interface states
      case InterfaceBlockBufferType::InterfaceStateVelocity : {
         return states_[InterfaceState::Velocity];
      }
      case InterfaceBlockBufferType::InterfaceStatePressurePositive : {
         return states_[InterfaceState::PressurePositive];
      }
      case InterfaceBlockBufferType::InterfaceStatePressureNegative : {
         return states_[InterfaceState::PressureNegative];
      }
      // interface parameters
#ifdef PERFORMANCE
      default : {
        return parameters_[InterfaceParameter::SurfaceTensionCoefficient];
      }
#else
      case InterfaceBlockBufferType::InterfaceParameterSurfaceTensionCoefficient : {
         return parameters_[InterfaceParameter::SurfaceTensionCoefficient];
      }
      default : {
         throw std::invalid_argument("Requested buffer does not exist (impossible error");
      }
      break;
#endif
   }
}

/**
 * @brief Const overload.
 */
auto InterfaceBlock::GetBuffer( InterfaceBlockBufferType const buffer_type) const -> double const (&)[CC::TCX()][CC::TCY()][CC::TCZ()] {
   switch( buffer_type ) {
      // interface description cases
      case InterfaceBlockBufferType::LevelsetBase : {
         return base_[InterfaceDescription::Levelset];
      }
      case InterfaceBlockBufferType::VolumeFractionBase : {
         return base_[InterfaceDescription::VolumeFraction];
      }
      case InterfaceBlockBufferType::LevelsetRightHandSide : {
         return right_hand_side_[InterfaceDescription::Levelset];
      }
      case InterfaceBlockBufferType::VolumeFractionRightHandSide : {
         return right_hand_side_[InterfaceDescription::VolumeFraction];
      }
      case InterfaceBlockBufferType::LevelsetReinitialized : {
         return reinitialized_[InterfaceDescription::Levelset];
      }
      case InterfaceBlockBufferType::VolumeFractionReinitialized : {
         return reinitialized_[InterfaceDescription::VolumeFraction];
      }
      // interface states
      case InterfaceBlockBufferType::InterfaceStateVelocity : {
         return states_[InterfaceState::Velocity];
      }
      case InterfaceBlockBufferType::InterfaceStatePressurePositive : {
         return states_[InterfaceState::PressurePositive];
      }
      case InterfaceBlockBufferType::InterfaceStatePressureNegative : {
         return states_[InterfaceState::PressureNegative];
      }
      // interface parameters
#ifdef PERFORMANCE
      default : {
        return parameters_[InterfaceParameter::SurfaceTensionCoefficient];
      }
#else
      case InterfaceBlockBufferType::InterfaceParameterSurfaceTensionCoefficient : {
         return parameters_[InterfaceParameter::SurfaceTensionCoefficient];
      }
      default : {
         throw std::invalid_argument("Requested buffer does not exist (impossible error");
      }
      break;
#endif
   }
}