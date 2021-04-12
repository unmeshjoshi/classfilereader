#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
using namespace std;

#include "ClassFileReader.h"


static ClassFile classObject;
static unsigned long code_length_static = 0;
int main(void)
{
  std::cout << "enter classname " << std::endl;
  string className;
  std::cin >> className;
  fstream classFile(className.c_str(), ios::in|ios::binary);
  unsigned long magicNumber = readMagicNumber(classFile);
  unsigned long minorVersion = readMinorVersion(classFile);
  unsigned long majorVersion = readMajorVersion(classFile);
  readConstantPoolEntries(classFile);
  unsigned long accessFlag =  readAccessFlag(classFile);
  std::cout << std::hex << accessFlag << std::endl;
  readThisClass(classFile);
  readSuperClass(classFile);
  unsigned long interface_count = readInterfaceCount(classFile);
  std::cout << "interface count: " << interface_count << std::endl;

  for(unsigned long i = 0; i < interface_count; i++)
    {
      unsigned long interface_index = readInterface(classFile);
    }
  unsigned long fields_count = readFieldsCount(classFile);
  std::cout << "Fields count: " << fields_count << std::endl;
  
  for(unsigned int i = 0; i < fields_count; i++)
    {
      readField(classFile);
    }

  unsigned long methods_count = readMethodsCount(classFile);
  std::cout << "method count: " << methods_count << std::endl;

  for(unsigned int i = 0; i < methods_count; i++)
    {
      readMethod(classFile);
    }
  unsigned long attributes_count = readAttributesCount(classFile);
  std::cout << "to attributes count: " << attributes_count << std::endl;
  for(unsigned int i = 0; i < attributes_count; i++)
    {
      readAttribute(classFile);
    }

   std::cout << "MAX LONG METHOD" << code_length_static;
   return 0;
}


unsigned long readConstantPoolCount(fstream &stream)
{
  readU2(stream);
}


unsigned long  readMagicNumber(fstream &stream)
{
  return readU4(stream);
}
unsigned long readMajorVersion(fstream &stream)
{
  return readU2(stream);
}
unsigned long readMinorVersion(fstream &stream)
{
  return readU2(stream);
}

unsigned long readU4(fstream &stream)
{
  /**
     unsigned char is important, because when unsigned char is promoted to int(or long ??)the higher order bits are all 0s, and if signed char is promoted to int (or long??)the higher order bits are all 1s, (so if char c = 0xca, is left shifted 16 bits, the upper 8 bytes will be all 1, i.e. result will be, ffca0000.And if unsigned char c = 0xca is left shifted 16 bits, the result will be 00ca0000 which we want. 
   */
        unsigned char pChar[4];
	if(!stream.read((char*)&pChar, sizeof(pChar)))
	{
	  std::cout << "errorororor";
	}
	unsigned long result = (pChar[0] << 24) + (pChar[1] << 16) + (pChar[2] << 8)+ pChar[3];
	return result;
}

unsigned long readU2(fstream &stream)
{
  unsigned char pChar[2];
  stream.read((char*)&pChar, sizeof(pChar));
  unsigned long result = (pChar[0] << 8) + pChar[1];
  return result;
}

unsigned long  readU1(fstream &stream)
{

  /*In ANSI C++ standard it is stated that 
    1. fundamental storage unit in the c++ object model is byte.
    byte is atleast large enough to hold any member of 'basic execution character set'.
    2.The basic execution character set and the basic execution wide-character set shall each contain all the members of the basic source character set, plus control characters representing alert, backspace, and carriage return, plus a null character (respectively, null wide character), whose representation has all zero bits.
    3. The basic source character set consists of 96 characters: the space character, the control characters representing horizontal tab, vertical tab, form feed, and new-line, plus the following 91 graphical characters:* 

[Footnote: The glyphs for the members of the basic source character set are intended to identify characters from the subset of ISO/IEC 10646 which corresponds to the ASCII character set. However, because the mapping from source file characters to the source character set (described in translation phase 1) is specified as implementation-defined, an implementation is required to document how the basic source characters are represented in source files. --- end foonote]
a b c d e f g h i j k l m n o p q r s t u v w x y z
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
0 1 2 3 4 5 6 7 8 9
_ { } [ ] # ( ) < > % : ; . ? * + - / ^ & | ~ ! = , \ " '

   4.The sizeof operator yields the number of bytes in the object representation of its operand.sizeof(char), sizeof(signed char) and sizeof(unsigned char) are 1.
   5.Objects declared as characters (char) shall be large enough to store any member of the implementation's basic character set.
   6.Unsigned integers, declared unsigned, shall obey the laws of arithmetic modulo 2n where n is the number of bits in the value representation of that particular size of integer.* 
[Footnote: This implies that unsigned arithmetic does not overflow because a result that cannot be represented by the resulting unsigned integer type is reduced modulo the number that is one greater than the largest value that can be represented by the resulting unsigned integer type. --- end foonote]

This implies that a char will always be 1 byte and where  byte is same as sequence of 8 bits.Because 8 bits are enought to hold any member of basic execution character set.

  */
  unsigned char pChar[1];
  stream.read((char*)&pChar, sizeof(pChar));
  unsigned long result = pChar[0];
  return result;
}

void getNBytes(fstream &stream, unsigned long count, char *buffer)
{
  stream.read(buffer, count);
}


void readConstantPoolEntries(fstream &stream)
{
  unsigned long constantPoolCount = readConstantPoolCount(stream);
  std::cout << "Constant Pool Count: " << constantPoolCount << std::endl;
  for(unsigned long i = 1; i < constantPoolCount; i++)
  {
    
    std::cout << "Constant pool entry: " << i << std::endl;


    unsigned long tag = readU1(stream);
  switch(tag)
    {
    case CONSTANT_Utf8:
      readUTF8String(stream);
      break;
    case CONSTANT_Class:
      readClassInfo(stream);
      break;
    case CONSTANT_Fieldref:
      readFieldref(stream);
      break;
    case CONSTANT_Methodref:
      readMethodref(stream);
      break;
    case CONSTANT_InterfaceMethodref:
      readInterfaceMethodref(stream);
      break;
    case CONSTANT_String:
      readString(stream);
      break;
    case CONSTANT_Integer:
      readInteger(stream);
      break;
    case CONSTANT_Float:
      readFloat(stream);
      break;
    case CONSTANT_Long:
      readLong(stream);
      i++;
      break;
    case CONSTANT_Double:
      readDouble(stream);
      i++;
      break;
    case CONSTANT_NameAndType:
      readNameAndType(stream);
      break;
    }
  }
}

void  readUTF8String(fstream &stream)
{

  /**Right now it is assumes that each of the entry will be in the ASCII char set, so no special treatment is required
   */
  unsigned long length = readU2(stream);
  char *buffer = new char[length + 1];
  getNBytes(stream, length, buffer);
  buffer[length] = '\0';

  utf8_info *info = new utf8_info();
  info->name = buffer;
  classObject.getPool().add(info);
  delete []buffer;
  std::cout << "read UTF8" << std::endl;
  std::cout << "string: " <<  info->name << std::endl;
 
}

void readClassInfo(fstream &stream)
{
  unsigned long name_index = readU2(stream);
  class_info *info = new class_info();
  info->name_index = name_index;
  classObject.getPool().add(info);


 std::cout << "readingClassInfo with name_index: " << std::dec << name_index << std::endl;
}

void readFieldref(fstream &stream)
{
  unsigned long class_index = readU2(stream);
  unsigned long name_and_type_index = readU2(stream);
  fieldref_info *info = new fieldref_info();
  info->class_index = class_index;
  info->name_and_type_index = name_and_type_index;
  classObject.getPool().add(info);

 std::cout << "reading Fieldref: " << endl;
  std::cout << "class_index: " << std::dec << class_index << endl;
  std::cout << "name_and_t ype_index: " << std::dec << name_and_type_index << std::endl;
 
}

void readMethodref(fstream &stream)
{
  unsigned long class_index = readU2(stream);
  unsigned long name_and_type_index = readU2(stream);
  methodref_info *info = new methodref_info();
  info->class_index = class_index;
  info->name_and_type_index = name_and_type_index;
  classObject.getPool().add(info);

  std::cout << "reading Methodref: "<< endl;
  std::cout << "class_index: " << std::dec << class_index << endl;
  std::cout << "name_and_t ype_index: " << std::dec << name_and_type_index << std::endl;
 
}
void readInterfaceMethodref(fstream &stream)
{
  unsigned long class_index = readU2(stream);
  unsigned long name_and_type_index = readU2(stream);
  interfacemethodref_info *info = new interfacemethodref_info();
  info->class_index = class_index;
  info->name_and_type_index = name_and_type_index;
  classObject.getPool().add(info);

  std::cout << "reading InterfaceMethodref: " << endl;
  std::cout << "class_index: " << std::dec << class_index << endl;
  std::cout << "name_and_t ype_index: " << std::dec << name_and_type_index << std::endl;
 
}
void readString(fstream &stream)
{
  unsigned long string_index = readU2(stream);
  string_info *info = new string_info();
  info->string_index = string_index;
  classObject.getPool().add(info);
  std::cout << "read String_Info " << std::endl;
  std::cout << "string_index: " << std::dec << string_index << std::endl;
}
void readInteger(fstream &stream)
{
  unsigned long bytes = readU4(stream);
  integer_info *info = new integer_info();
  info->bytes = bytes;
  classObject.getPool().add(info);
  std::cout << "read Integer_info" << std::endl;
  std::cout << "bytes: " << std::hex << bytes << std::endl;

}

void readFloat(fstream &stream)
{
  unsigned long bytes = readU4(stream);
  float_info *info = new float_info();
  info->bytes = bytes;
  classObject.getPool().add(info);
  std::cout << "read Float_info" << std::endl;
  std::cout << "bytes: " << std::hex << bytes << std::endl;

}

void readLong(fstream &stream)
{
  unsigned long high_bytes = readU4(stream);
  unsigned long low_bytes = readU4(stream);
  long_info *info = new long_info();
  info->high_bytes = high_bytes;
  info->low_bytes = low_bytes;
  classObject.getPool().add(info);
  long_info *hack_info = new long_info();
  *hack_info = *info;
  classObject.getPool().add(info);//Hack because for long and double the index increments by 2  
  std::cout << "read Long_info" << std::endl;
  std::cout << "high bytes: " << std::hex << high_bytes << std::endl;
  std::cout << "low bytes: " << std::hex << low_bytes << std::endl;
}

void readDouble(fstream &stream)
{
  unsigned long high_bytes = readU4(stream);
  unsigned long low_bytes = readU4(stream);
  double_info *info = new double_info();
  info->high_bytes = high_bytes;
  info->low_bytes = low_bytes;
  classObject.getPool().add(info);
  double_info *hack_info = new double_info();
  *hack_info = *info;
  classObject.getPool().add(hack_info);//Hack
  
  std::cout << "read Double_info" << std::endl;
  std::cout << "high bytes: " << std::hex << high_bytes << std::endl;
  std::cout << "low bytes: " << std::hex << low_bytes << std::endl;
}

void readNameAndType(fstream &stream)
{
  unsigned long name_index = readU2(stream);
  unsigned long descriptor_index = readU2(stream);
  nameandtypeinfo *info = new nameandtypeinfo();
  info->name_index = name_index;
  info->descriptor_index = descriptor_index;
  classObject.getPool().add(info);
  std::cout << "read NameAndTypeinfo " << std::endl;
  std::cout << "name_index: " << std::dec << name_index << std::endl;
  std::cout << "descriptor_index: " << std::dec << descriptor_index << std::endl;
}


unsigned long readAccessFlag(fstream &stream)
{
  return readU2(stream);
}
unsigned long readThisClass(fstream &stream)
{
  unsigned long index = readU2(stream);
  std::cout << "this  class index: " << index;
  getClassNameFromConstantPool(index);
  return index;

}

string getClassNameFromConstantPool(unsigned long index)
{
  constant_pool &pool = classObject.getPool();
  cp_info *p = pool.get(index - 1);
  class_info *pClassInfo = dynamic_cast<class_info*>(p);
  if(0 == pClassInfo)
    {
      return 0;
    }
    unsigned long name_index = pClassInfo->name_index;
    p = classObject.getPool().get(name_index - 1);
    utf8_info *putf8info = dynamic_cast<utf8_info*>(p);
    if(0 == putf8info)
      {
	return 0;
      }

    string name = putf8info->name;
    return name;
}
unsigned long readSuperClass(fstream &stream)
{
  unsigned long index = readU2(stream);
  std::cout << "super class index: " << index;
  getClassNameFromConstantPool(index);
  return index;

}

unsigned long readInterfaceCount(fstream &stream)
{
  return readU2(stream);
}

unsigned long  readInterface(fstream &stream)
{
  return readU2(stream);//Index to contant_pool to CONSTANT_Classinfo
}

unsigned long readFieldsCount(fstream &stream)
{
  return readU2(stream);
}

field_info* readField(fstream &stream)
{
  field_info *fieldinfo = new field_info();
  unsigned long access_flag = readU2(stream);
  unsigned long name_index = readU2(stream);
  string name = getUTF8String(name_index);
  fieldinfo->name = name;
  
  std::cout << "Reading Field Info: " << std::endl;
  std::cout << "Field Name: " << name << std::endl;
  
  unsigned long descriptor_index = readU2(stream);
  string descriptor = getUTF8String(descriptor_index);
  fieldinfo->descriptor = descriptor;
  std::cout << "Field Descriptor: " << descriptor << std::endl;

  unsigned long attributes_count = readU2(stream);
  std::cout << "Attribute Count: " << attributes_count << std::endl;

  fieldinfo->attributes.reserve(attributes_count);

  for(unsigned long i = 0; i < attributes_count; i++)
    {
      attribute *pAttribute = readAttribute(stream);
      fieldinfo->attributes.push_back(pAttribute);
    }
  return fieldinfo;
}


unsigned long readMethodsCount(fstream &stream)
{
  return readU2(stream);
}
method_info* readMethod(fstream &stream)
{
  method_info *methodinfo = new method_info();
  unsigned long access_flag = readU2(stream);

  unsigned long name_index = readU2(stream);
  string name = getUTF8String(name_index);
  methodinfo->name = name;

  unsigned long descriptor_index = readU2(stream);
  string descriptor = getUTF8String(descriptor_index);
  methodinfo->descriptor = descriptor;
std::cout << "read method , name: " << name << ", descriptor: " << descriptor << std::endl;
  unsigned long attributes_count = readU2(stream);

  methodinfo->attributes.reserve(attributes_count);

  for(unsigned long i = 0; i < attributes_count; i++)
    {
      attribute *pAttribute = readAttribute(stream);
      methodinfo->attributes.push_back(pAttribute);
    }
  return methodinfo;
}

unsigned long readAttributesCount(fstream &stream)
{
  return readU2(stream);
}

attribute* readAttribute(fstream &stream)
{
  attribute *pAttribute = new attribute();
  unsigned long attribute_name_index = readU2(stream);
  string name = getUTF8String(attribute_name_index);
  std::cout << "read attribute, name: " << name << std::endl;
  unsigned long attribute_length = readU4(stream);
  std::cout << "attribute_length:  " << attribute_length << std::endl;
  pAttribute =  buildAttribute(name, stream);
  if(0 == pAttribute)//If the attribute is unknown, skip it
  {
    char *buffer = new char[attribute_length];
    getNBytes(stream, attribute_length, buffer);
    delete [] buffer;
  }
 
  return pAttribute;
}

attribute* buildAttribute(string &name, fstream &stream)
{
  attribute *pAttribute = 0;
  if(name == "Code")
    {
      pAttribute = createCodeAttribute(stream);
    }
  else if(name == "Exceptions")
    {
      pAttribute = createExcetionAttribute(stream);
    }
  else if(name == "ConstantValue")
    {
      pAttribute = createConstantValueAttribute(stream);
    }
  else if(name == "InnerClasses")
    {
      pAttribute = createInnerClassAttribute(stream);
    }
  else if(name == "Synthetic")
    {
      pAttribute = createSyntheticAttribute(stream);
    }
  else if(name == "SourceFile")
    {
      pAttribute = createSourceFile(stream);
    }
  else if(name == "LineNumberTable")
    {
      pAttribute = createLineNumberTable(stream);
    } 
  else if(name == "LocalVariableTable")
    {
      pAttribute = createLocalVariableTable(stream);
    }
  else if(name == "Deprecated")
    {
      pAttribute = createDeprecatedAttribute(stream);
    }
  else if(name == "Signature")
    {
      pAttribute = createSignatureAttribute(stream);
    }
  return pAttribute;
}

attribute* createSignatureAttribute(fstream &stream)
{
  signature *p = new signature();
  unsigned long signature_index = readU2(stream);
  p->str = getUTF8String(signature_index);
  std::cout << "Read Signature Attribute: " << p->str << std::endl;
}

attribute* createCodeAttribute(fstream &stream)
{
  
  code *codeAttribute = new code();
  codeAttribute->max_stack = readU2(stream);
  std::cout << "max_stack: " <<  codeAttribute->max_stack << std::endl;
  codeAttribute->max_locals = readU2(stream);
  std::cout << "max_locals: " << codeAttribute->max_locals << std::endl;
  codeAttribute->code_length = readU4(stream);
  if(code_length_static < codeAttribute->code_length)
{
 	code_length_static = codeAttribute->code_length;
}		  
  std::cout << "code_length:  " << codeAttribute->code_length << std::endl;

  char *pCode = new char[codeAttribute->code_length];
  getNBytes(stream, codeAttribute->code_length, pCode);

  std::cout << "Code Bytecode" << string(pCode);

  //need to fix this, there should not be such cast should use char or unsigned char consistently

  unsigned long exception_table_length = readU2(stream);
  //  std::cout << "code  " << exception_table_length << std::endl;
  std::cout << "exception_table_length:  " << exception_table_length << std::endl;

  for(int i = 0; i < exception_table_length; i++)
    {
      exception_table_entry entry = readExceptionTableEntry(stream);
      codeAttribute->exception_table.push_back(entry);
    }
  unsigned long attributes_count = readU2(stream);
  std::cout << "attributes_cout:  " << attributes_count << std::endl;

  for(int i = 0; i < attributes_count; i++)
    {
      attribute *pAttribute = readAttribute(stream);
      codeAttribute->attributes.push_back(pAttribute);
    }

  return codeAttribute;
}

exception_table_entry readExceptionTableEntry(fstream &stream)
{
  exception_table_entry entry;
  entry.start_pc = readU2(stream);
  std::cout << "start_pc:  " << entry.start_pc << std::endl;

  entry.end_pc  = readU2(stream);
  std::cout << "end_pc:  " << entry.end_pc << std::endl;

  entry.handler_pc = readU2(stream);
  std::cout << "handler_pc:  " << entry.handler_pc << std::endl;

  entry.catch_type = readU2(stream);
  std::cout << "catch_type:  " << entry.catch_type << std::endl;
  return entry;
}

attribute* createExcetionAttribute(fstream &stream)
{
  exception_attribute *pExceptionAttribute = new exception_attribute();
  unsigned long number_of_exceptions = readU2(stream);
  for(unsigned int i = 0; i < number_of_exceptions; i++)
    {
      unsigned long exception_class_index = readU2(stream);
      string className = getClassNameFromConstantPool(exception_class_index);
      pExceptionAttribute->exception_types.push_back(className);
    }
  return pExceptionAttribute;
}

attribute* createInnerClassAttribute(fstream &stream)
{
  inner_class *pInnerClass = new inner_class();
  unsigned long number_of_classes = readU2(stream);
  pInnerClass->classes.reserve(number_of_classes);
  
  std::cout << "Numner of Inner Classes: " << number_of_classes;
  for(int i = 0; i < number_of_classes; i++)
    {
      inner_class_entry entry = readInnerClassEntry(stream);
      pInnerClass->classes.push_back(entry);
    }
  return pInnerClass;
}

/**
   Not good to return this by value because there are too many copies of string taking place.
   but for now let it be as it is.
 */
inner_class_entry readInnerClassEntry(fstream &stream)
{
  inner_class_entry entry;
  unsigned long inner_class_index = readU2(stream);
  entry.inner_class_name = getClassNameFromConstantPool(inner_class_index);
  
  std::cout << "Inner Class Name: " << entry.inner_class_name << std::endl;

  unsigned long outer_class_index = readU2(stream);
  entry.outer_class_name = getClassNameFromConstantPool(outer_class_index);
  std::cout << "Outer Class Name: " << entry.outer_class_name << std::endl;

  unsigned long inner_class_simplename_index = readU2(stream);
  std::cout << "Inner Class Simple Name Index:" <<  inner_class_simplename_index;
 // entry.inner_class_simplename = getClassNameFromConstantPool(inner_class_simplename_index);
//  std::cout << "Inner Class Simple Name: " << entry.inner_class_simplename << std::endl;

  unsigned long inner_class_access_flag = readU2(stream);
  entry.inner_class_access_flags = inner_class_access_flag;
  return entry;
  
}

attribute* createConstantValueAttribute(fstream &stream)
{
  constantValue *p = new constantValue();
  p->constant_value_index = readU2(stream);
  std::cout << "Constant value index: " << p->constant_value_index;
  return p;
}

attribute* createDeprecatedAttribute(fstream &stream)
{
  std::cout << "Read Deprecated " ;
  return new deprecated();
}

attribute* createSyntheticAttribute(fstream &stream)
{
  std::cout << "Read Synthetic " ;
  return new synthetic();
}

attribute* createSourceFile(fstream &stream)
{
  source_file *p = new source_file();
  unsigned long source_file_index = readU2(stream);
  std::cout << "source file index: " << source_file_index;
  p->source_file_name = getUTF8String(source_file_index);
  std::cout << "Read Source File " << p->source_file_name;
  return p;
}

attribute* createLineNumberTable(fstream &stream)
{
  line_number_table *p = new line_number_table();
  unsigned long line_number_table_length = readU2(stream);
  std::cout << "Line Number Table Length: " << std::dec << line_number_table_length << std::endl;
  for(unsigned long i = 0; i < line_number_table_length; i++)
    {
      line_number_table_entry entry = readLineNumberTableEntry(stream);
      p->table.push_back(entry);
    }
  return p;
}

line_number_table_entry readLineNumberTableEntry(fstream &stream)
{
  line_number_table_entry entry;
  entry.start_pc = readU2(stream);
  std::cout << "start_pc " << entry.start_pc << std::endl;

  entry.line_number = readU2(stream);

  std::cout << "line number " << entry.line_number << std::endl;
  return entry;
}

attribute* createLocalVariableTable(fstream &stream)
{
  local_variable_table *p = new local_variable_table();
  unsigned long local_variable_table_length = readU2(stream);
  std::cout << "Local Variable Table length:  " << local_variable_table_length;
  for(unsigned long i = 0; i < local_variable_table_length; i++)
    {
      local_variable_table_entry entry = readLocalVariableTableEntry(stream);
      p->table.push_back(entry);
    }
  return p;
}

/**
   Not good to return this by value because there are too many copies of string taking place.
   but for now let it be as it is.
 */
local_variable_table_entry readLocalVariableTableEntry(fstream &stream)
{
  local_variable_table_entry entry;
  entry.start_pc = readU2(stream);
  std::cout << "start_pc " << entry.start_pc ;

  entry.length = readU2(stream);
  std::cout << "length " << entry.length;

  unsigned long name_index = readU2(stream);
  entry.name = getUTF8String(name_index);
  std::cout << "variable name: " << entry.name;

  unsigned long descriptor_index = readU2(stream);
  entry.descriptor = getUTF8String(descriptor_index);
  std::cout << "descriptor " << entry.descriptor ;

  entry.index = readU2(stream);
  std::cout << "index:  " << entry.index;
  return entry;
}

string getUTF8String(unsigned long name_index)
{
    cp_info *p = classObject.getPool().get(name_index - 1);
    utf8_info *putf8info = dynamic_cast<utf8_info*>(p);
    if(0 == putf8info)
      {
	return 0;
      }

    string name = putf8info->name;
    return name;
}




