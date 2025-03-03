// Copyright (c) 2024 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "CarlaRecorderHelpers.h"
#include <Containers/UnrealString.h>

#include <fstream>
#include <vector>

// create a temporal buffer to convert from and to FString and bytes
static std::vector<uint8_t> CarlaRecorderHelperBuffer;

// get the final path + filename
std::string GetRecorderFilename(std::string Filename)
{
  std::string Filename2;

  // check if a relative path was specified
  if (Filename.find("\\") != std::string::npos || Filename.find("/") != std::string::npos || Filename.find(":") != std::string::npos)
    Filename2 = Filename;
  else
  {
    FString Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
    Filename2 = TCHAR_TO_UTF8(*Path) + Filename;
  }

  return Filename2;
}

// ------
// write
// ------

// write binary data from FVector
void WriteFVector(std::ostream &OutFile, const FVector &InObj)
{
  WriteValue<double>(OutFile, InObj.X);
  WriteValue<double>(OutFile, InObj.Y);
  WriteValue<double>(OutFile, InObj.Z);
}

// write binary data from FTransform
void WriteFTransform(std::ofstream &OutFile, const FTransform &InObj)
{
  WriteFVector(OutFile, InObj.GetTranslation());
  WriteFVector(OutFile, InObj.GetRotation().Euler());
}

// write binary data from FString (length + text)
void WriteFString(std::ostream &OutFile, const FString &InObj)
{
  // encode the string to UTF8 to know the final length
  FTCHARToUTF8 EncodedString(*InObj);
  int16_t Length = EncodedString.Length();
  // write
  WriteValue<uint16_t>(OutFile, Length);
  OutFile.write(reinterpret_cast<char *>(TCHAR_TO_UTF8(*InObj)), Length);
}

// -----
// read
// -----

// read binary data to FVector
void ReadFVector(std::istream &InFile, FVector &OutObj)
{
  ReadValue<double>(InFile, OutObj.X);
  ReadValue<double>(InFile, OutObj.Y);
  ReadValue<double>(InFile, OutObj.Z);
}

// read binary data to FTransform
void ReadFTransform(std::ifstream &InFile, FTransform &OutObj)
{
  FVector Vec;
  ReadFVector(InFile, Vec);
  OutObj.SetTranslation(Vec);
  ReadFVector(InFile, Vec);
  OutObj.GetRotation().MakeFromEuler(Vec);
}

// read binary data to FString (length + text)
void ReadFString(std::istream &InFile, FString &OutObj)
{
  uint16_t Length;
  ReadValue<uint16_t>(InFile, Length);
  // make room in vector buffer
  if (CarlaRecorderHelperBuffer.capacity() < Length + 1)
  {
    CarlaRecorderHelperBuffer.reserve(Length + 1);
  }
  CarlaRecorderHelperBuffer.clear();
  // initialize the vector space with 0
  CarlaRecorderHelperBuffer.resize(Length + 1);
  // read
  InFile.read(reinterpret_cast<char *>(CarlaRecorderHelperBuffer.data()), Length);
  // convert from UTF8 to FString
  OutObj = FString(UTF8_TO_TCHAR(CarlaRecorderHelperBuffer.data()));
}
