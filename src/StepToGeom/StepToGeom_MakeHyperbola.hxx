// Created on: 1994-09-08
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _StepToGeom_MakeHyperbola_HeaderFile
#define _StepToGeom_MakeHyperbola_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class StepGeom_Hyperbola;
class Geom_Hyperbola;


//! This class implements the mapping between classes
//! Hyperbola from StepGeom which describes a Hyperbola from
//! Prostep and Hyperbola from Geom.
class StepToGeom_MakeHyperbola 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Boolean Convert (const Handle(StepGeom_Hyperbola)& SC, Handle(Geom_Hyperbola)& CC);




protected:





private:





};







#endif // _StepToGeom_MakeHyperbola_HeaderFile