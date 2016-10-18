////////////////////////////////////////////////////////////////////////////////
// The following FIT Protocol software provided may be used with FIT protocol
// devices only and remains the copyrighted property of Dynastream Innovations Inc.
// The software is being provided on an "as-is" basis and as an accommodation,
// and therefore all warranties, representations, or guarantees of any kind
// (whether express, implied or statutory) including, without limitation,
// warranties of merchantability, non-infringement, or fitness for a particular
// purpose, are specifically disclaimed.
//
// Copyright 2016 Dynastream Innovations Inc.
////////////////////////////////////////////////////////////////////////////////
// ****WARNING****  This file is auto-generated!  Do NOT edit this file.
// Profile Version = 20.14Release
// Tag = production/akw/20.14.00-0-g448ef52
////////////////////////////////////////////////////////////////////////////////


#include <cmath>
#include <sstream>
#include "fit_field.hpp"
#include "fit_mesg.hpp"
#include "fit_unicode.hpp"

namespace fit
{

Field::Field(void)
    : FieldBase()
    , profile(NULL)
{
}

Field::Field(const Field &field)
    : FieldBase(field)
    , profile(field.profile)
    , profileIndex(field.profileIndex)
    , fieldNum(field.fieldNum)
    , fieldType(field.fieldType)
{
}

Field::Field(const Profile::MESG_INDEX mesgIndex, const FIT_UINT16 fieldIndex)
    : FieldBase()
    , profile(&Profile::mesgs[mesgIndex])
    , profileIndex(fieldIndex)
    , fieldNum(FIT_UINT8_INVALID)
    , fieldType(FIT_UINT8_INVALID)
{
}

Field::Field(const FIT_UINT16 mesgNum, const FIT_UINT8 fieldNum, const FIT_UINT8 fieldType)
    : FieldBase()
    , profile(Profile::GetMesg(mesgNum))
    , profileIndex(Profile::GetFieldIndex(mesgNum, fieldNum))
    , fieldNum(fieldNum)
    , fieldType(fieldType)
{
}

Field::Field(const std::string& mesgName, const std::string& fieldName)
    : FieldBase()
    , profile(Profile::GetMesg(mesgName))
    , profileIndex(Profile::GetFieldIndex(mesgName, fieldName))
    , fieldNum(FIT_UINT8_INVALID)
    , fieldType(FIT_UINT8_INVALID)
{
}

FIT_BOOL Field::IsValid(void) const
{
    return true; // profileIndex != FIT_UINT16_INVALID;
}

FIT_BOOL Field::GetIsAccumulated() const
{
   if ((profile == NULL) || (profileIndex >= profile->numFields))
        return FIT_FALSE;
   return profile->fields[profileIndex].isAccumulated;
}

FIT_UINT16 Field::GetIndex(void) const
{
    return profileIndex;
}

std::string Field::GetName() const
{
    if ((profile == NULL) || (profileIndex >= profile->numFields))
        return "unknown";
    return profile->fields[profileIndex].name;
}

FIT_UINT8 Field::GetNum(void) const
{
    if ((profile == NULL) || (profileIndex >= profile->numFields))
        return fieldNum; // FIT_FIELD_NUM_INVALID;
    return profile->fields[profileIndex].num;
}

FIT_UINT8 Field::GetType() const
{
    if ((profile == NULL) || (profileIndex >= profile->numFields))
        return fieldType; // FIT_UINT8_INVALID;
    return profile->fields[profileIndex].type;
}

std::string Field::GetUnits() const
{
    if ((profile == NULL) || (profileIndex >= profile->numFields))
        return "";
    return profile->fields[profileIndex].units;
}

FIT_FLOAT64 Field::GetScale() const
{
    if ((profile == NULL) || (profileIndex >= profile->numFields))
        return 1;
    return profile->fields[profileIndex].scale;
}

FIT_FLOAT64 Field::GetOffset() const
{
    if ((profile == NULL) || (profileIndex >= profile->numFields))
        return 0;
    return profile->fields[profileIndex].offset;
}

FIT_UINT16 Field::GetNumComponents(void) const
{
    if ((profile == NULL) || (profileIndex >= profile->numFields))
        return 0;
    return profile->fields[profileIndex].numComponents;
}

FIT_UINT16 Field::GetNumSubFields(void) const
{
    if ((profile == NULL) || (profileIndex >= profile->numFields))
        return 0;
    return profile->fields[profileIndex].numSubFields;
}

const Profile::FIELD_COMPONENT* Field::GetComponent(const FIT_UINT16 component) const
{
    if (component >= GetNumComponents())
        return NULL;
    return &profile->fields[profileIndex].components[component];
}

const Profile::SUBFIELD* Field::GetSubField(const FIT_UINT16 subFieldIndex) const
{
    if (subFieldIndex >= GetNumSubFields())
        return NULL;
    return &profile->fields[profileIndex].subFields[subFieldIndex];
}
} // namespace fit
