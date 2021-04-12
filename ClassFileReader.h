const unsigned long CONSTANT_Class = 7;
const unsigned long CONSTANT_Fieldref = 9;
const unsigned long CONSTANT_Methodref = 10;
const unsigned long CONSTANT_InterfaceMethodref = 11;
const unsigned long CONSTANT_String = 8;
const unsigned long CONSTANT_Integer = 3;
const unsigned long CONSTANT_Float = 4;
const unsigned long CONSTANT_Long = 5;
const unsigned long CONSTANT_Double = 6;
const unsigned long CONSTANT_NameAndType = 12;
const unsigned long CONSTANT_Utf8 = 1;

struct field_info;
struct method_info;
struct attribute;
struct exception_table_entry;
struct inner_class_entry;
struct local_variable_table_entry;
struct line_number_table_entry;

unsigned long readMagicNumber(fstream &stream);
unsigned long readMajorVersion(fstream &stream);
unsigned long readMinorVersion(fstream &stream);
unsigned long readU4(fstream &stream);
unsigned long readU2(fstream &stream);
unsigned long readU1(fstream &stream);
unsigned long readConstantPoolCount(fstream &stream);
unsigned long readAccessFlag(fstream &stream);
unsigned long readThisClass(fstream &stream);
unsigned long readSuperClass(fstream &stream);
unsigned long readInterfaceCount(fstream &stream);
unsigned long readInterface(fstream &stream);
unsigned long readFieldsCount(fstream &stream);
field_info* readField(fstream &stream);
unsigned long readMethodsCount(fstream &stream);
method_info* readMethod(fstream &stream);
unsigned long readAttributesCount(fstream &stream);
attribute* readAttribute(fstream &stream);
attribute* buildAttribute(string& name, fstream &stream);
attribute* createCodeAttribute(fstream &stream);
exception_table_entry readExceptionTableEntry(fstream &stream);
attribute* createExcetionAttribute(fstream &stream);
attribute* createInnerClassAttribute(fstream &stream);
attribute* createConstantValueAttribute(fstream &stream);
attribute* createSyntheticAttribute(fstream &stream);
attribute* createSourceFile(fstream &stream);
attribute* createLineNumberTable(fstream &stream);
attribute* createLocalVariableTable(fstream &stream);
attribute* createDeprecatedAttribute(fstream &stream);
attribute* createSignatureAttribute(fstream &stream);

void readUTF8String(fstream &stream);
void readConstantPoolEntries(fstream &stream);
void readClassInfo(fstream &stream);
void readFieldref(fstream &stream);
void readMethodref(fstream &stream);
void readInterfaceMethodref(fstream &stream);
void readString(fstream &stream);
void readInteger(fstream &stream);
void readFloat(fstream &stream);
void readLong(fstream &srteam);
void readDouble(fstream &stream);
void readNameAndType(fstream &stream);
void getNBytes(fstream &stream, unsigned long count, char *buffer);
string getUTF8String(unsigned long name_index);
string getClassNameFromConstantPool(unsigned long index);
inner_class_entry readInnerClassEntry(fstream &stream);
local_variable_table_entry readLocalVariableTableEntry(fstream &stream);
line_number_table_entry readLineNumberTableEntry(fstream &stream);



struct  cp_info //base structure to make the heterogeneous collection possible
{
  public:
virtual ~cp_info(){}
};


struct utf8_info:public cp_info
{
  string name; //it is assumes that only ASCII charachers will be there in the string, so basic_string<char> is used.
};

struct class_info:public cp_info
{
  unsigned long name_index;
};

struct fieldref_info:public cp_info
{
  unsigned long class_index;
  unsigned long name_and_type_index;
};

struct methodref_info:public cp_info
{
  unsigned long class_index;
  unsigned long name_and_type_index;
};

struct interfacemethodref_info:public cp_info
{
  unsigned long class_index;
  unsigned long name_and_type_index;
};

struct string_info:public cp_info
{
  unsigned long string_index;
};

struct integer_info:public cp_info
{
  unsigned long bytes;
};

struct float_info:public cp_info
{
  unsigned long bytes;
};

struct long_info:public cp_info
{
  unsigned long high_bytes;
  unsigned long low_bytes;
};

struct double_info:public cp_info
{
  unsigned long high_bytes;
  unsigned long low_bytes;
};

struct nameandtypeinfo:public cp_info
{
  unsigned long name_index;
  unsigned long descriptor_index;
};

class constant_pool
{
 private:
  /**I first used vector<auto_ptr<cp_info*> >, so that there will be no need for the use of deleting the cp_info structures explicitly.But auto_ptr can not be used with std library containers, because it does not satisfy copy constructibe requirenment of the library containers, which says that the objects should be 'equivalent'.The term rquivalent means that, all the public functions of the class should behave same on both the objects.This is not true with the destructors of auto_ptr.When we copy construct one auto_ptr from other, the ownership of containing pointer is given to the new object and the new object now deletes the containing object.This creates exceptional situation when library container or algorithm uses local copy of the element.
     So to avoid use of auto_ptr with containers.The copy constructor and copy assignment operators of auto_ptr take non-const arguments, so when used with containers, which expect const arguments to copy constructor and assignment operator, the code doesnt compile.(This is the change done in the standard and depends on the compiler version.)
  */
  
  vector<cp_info*> pool;
 public:
  constant_pool(){}
  void add(cp_info *pcp_info){
    pool.push_back(pcp_info);
  }
  cp_info* get(unsigned long index){
    return pool[index];
  }
  ~constant_pool(){
    vector<cp_info*>::iterator itor = pool.begin();
    while(itor != pool.end())
      {
	cp_info *item = *itor++;
	delete item;
      }
  }
 };

struct attribute
{
  string name;
};

struct method_info
{
  unsigned long access_flag;
  string name;
  string descriptor;
  vector <attribute*> attributes;
};


struct field_info
{
  unsigned long access_flag;  
  string name;
  string descriptor;
  vector<attribute*> attributes;
};

class ClassFile
{
 private:
  constant_pool pool;
  vector<field_info> fields;
  vector<method_info> methods;
  vector<attribute*> attributes;
 public:
  constant_pool& getPool(){
    return pool;
  }
};


struct constantValue:public attribute
{
  unsigned long constant_value_index;
};

struct exception_table_entry
{
  unsigned long start_pc;
  unsigned long end_pc;
  unsigned long handler_pc;
  unsigned long catch_type;
  
};

struct code:public attribute
{
  unsigned long max_stack;//maximum depth of operand stack
  unsigned long max_locals;//
  unsigned long code_length;
  unsigned char *_code;
  vector<exception_table_entry> exception_table;
  vector<attribute*> attributes;
};



struct exception_attribute:public attribute
{
  vector<string> exception_types;
};


struct inner_class_entry
{
  string inner_class_name;
  string  outer_class_name;
  string  inner_class_simplename;
  unsigned long inner_class_access_flags;

};

struct inner_class:public attribute
{
  vector<inner_class_entry> classes;
};

/**
   This attribute does not contain anything, 
   A class member that does not appear in the source code must be marked using    a Synthetic attribute. 
 */
struct synthetic:public attribute
{
};

struct source_file:public attribute
{
  string source_file_name;
};


struct line_number_table_entry
{
  unsigned long start_pc;
  unsigned long line_number;
};

struct line_number_table:public attribute
{
  vector<line_number_table_entry> table;
};

struct local_variable_table_entry
{
  unsigned long start_pc;
  unsigned long length;
  string  name;
  string  descriptor;
  unsigned long index;
};

struct local_variable_table:public attribute
{
  vector<local_variable_table_entry> table;
};

struct deprecated:public attribute
{

};


struct signature:public attribute
{
  string str;
};


















