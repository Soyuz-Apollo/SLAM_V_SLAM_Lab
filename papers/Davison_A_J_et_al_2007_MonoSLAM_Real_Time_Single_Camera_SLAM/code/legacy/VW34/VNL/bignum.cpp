// This is vxl/VNL/bignum.cxx
#include <VNL/VC6WarningsFix.h>
#include "bignum.h"
/**
* \file
*/

#include <cstdlib>   // Include standard c library support
#include <cctype>    // Include character macros
#include <cstring>   // for strlen
#include <cmath>     // for fmod
#include <algorithm> // for std::copy
#include <vector>
#include <assert.h>

typedef unsigned short Counter;
typedef unsigned short Data;


/** Creates a zero VNL::BigNum.
*/

VNL::BigNum::BigNum ()
: count(0), sign(1), data(0) {
}

/** Creates a VNL::BigNum from a long integer.
*/

VNL::BigNum::BigNum (long l) {
  if (l >= 0)                   // Get correct sign
    this->sign = 1;
  else {
    l = -l;                     // Get absolute value of l
    this->sign = -1;
  }
  Data buf[sizeof(l)];          // Temp buffer to store l in
  Counter i = 0;                // buffer index
  while (l) {                   // While more bits in l
    assert(i < sizeof(l));      // no more buffer space
    buf[i] = Data(l);           // Peel off lower order bits
    l >>= 16;   // Shift next bits into place
    i++;
  }
  this->data = ((this->count = i) > 0 ? new Data[i] : 0); // Allocate permanent data

  i = 0;
  while (i < this->count) {     // Save buffer into perm. data
    this->data[i] = buf[i];
    i++;
  }
}

/** Creates a VNL::BigNum from an integer.
*/

VNL::BigNum::BigNum (int l) {
  if (l >= 0)                   // Get correct sign
    this->sign = 1;
  else {
    l = -l;                     // Get absolute value of l
    this->sign = -1;
  }
  Data buf[sizeof(l)];          // Temp buffer to store l in
  Counter i = 0;                // buffer index
  while (l) {                   // While more bits in l
    assert(i < sizeof(l));      // no more buffer space
    buf[i] = Data(l);           // Peel off lower order bits
    l >>= 16;   // Shift next bits into place
    i++;
  }
  this->data = ((this->count = i) > 0 ? new Data[i] : 0); // Allocate permanent data

  i = 0;
  while (i < this->count) {     // Save buffer into perm. data
    this->data[i] = buf[i];
    i++;
  }
}

/** Creates a VNL::BigNum from an unsigned long integer.
*/

VNL::BigNum::BigNum (unsigned long l) {
  this->sign = 1;
  Data buf[sizeof(l)];          // Temp buffer to store l in
  Counter i = 0;                // buffer index
  while (l) {                   // While more bits in l
    assert(i < sizeof(l));      // no more buffer space
    buf[i] = Data(l);           // Peel off lower order bits
    l >>= 16;   // Shift next bits into place
    i++;
  }
  this->data = ((this->count = i) > 0 ? new Data[i] : 0); // Allocate permanent data

  i = 0;
  while (i < this->count) {     // Save buffer into perm. data
    this->data[i] = buf[i];
    i++;
  }
}

/** Creates a VNL::BigNum from an unsigned integer.
*/

VNL::BigNum::BigNum (unsigned int l) {
  this->sign = 1;
  Data buf[sizeof(l)];          // Temp buffer to store l in
  Counter i = 0;                // buffer index
  while (l) {                   // While more bits in l
    assert(i < sizeof(l));      // no more buffer space
    buf[i] = Data(l);           // Peel off lower order bits
    l >>= 16;   // Shift next bits into place
    i++;
  }
  this->data = ((this->count = i) > 0 ? new Data[i] : 0); // Allocate permanent data

  i = 0;
  while (i < this->count) {     // Save buffer into perm. data
    this->data[i] = buf[i];
    i++;
  }
}

/** Creates a VNL::BigNum from a double floating point number.
*/

VNL::BigNum::BigNum (double d) {
  if (d >= 0)                   // Get sign of d
    this->sign = 1;
  else {
    d = -d;                     // Get absolute value of d
    this->sign = -1;
  }

  // Note: 0x10000L == 1 >> 16: the (assumed) size of unsigned short is 16 bits.
  std::vector<Data> buf;
  while (d >= 1.0) {
    buf.push_back( Data(fmod(d,0x10000L)) );  // Get next data "digit" from d
    d /= 0x10000L;                                // Shift d right 1 data "digit"
  }
  // Allocate and copy into permanent buffer
  this->data = (buf.size()>0 ? new Data[buf.size()] : 0);
  this->count = static_cast<unsigned short>(buf.size());
  std::copy( buf.begin(), buf.end(), data );
}


/** Creates a VNL::BigNum from a "long double" floating point number.
*/

VNL::BigNum::BigNum (long double d) {
  if (d >= 0)                   // Get sign of d
    this->sign = 1;
  else {
    d = -d;                     // Get absolute value of d
    this->sign = -1;
  }

  // Note: 0x10000L == 1 >> 16: the (assumed) size of unsigned short is 16 bits.
  std::vector<Data> buf;
  while (d >= 1.0) {
    buf.push_back( Data(fmod(d,0x10000L)) );  // Get next data "digit" from d
    d /= 0x10000L;                                // Shift d right 1 data "digit"
  }
  // Allocate and copy into permanent buffer
  this->data = (buf.size()>0 ? new Data[buf.size()] : 0);
  this->count = buf.size();
  std::copy( buf.begin(), buf.end(), data );
}


#if 0 // original implementation - PVr
static bool is_decimal(const char *s)
{
  if (*s == '+' || *s == '-') ++s;
  if (*s < '1' || *s > '9') return false;
  while (*s >= '0' && *s <= '9') ++s;
  if (*s == 'l' || *s == 'L') ++s;
  return *s == '\0';
}
static bool is_exponential(const char *s)
{
  if (*s == '+' || *s == '-') ++s;
  if (*s < '1' || *s > '9') return false;
  while (*s >= '0' && *s <= '9') ++s;
  if (*s != 'e' && *s != 'E') return false;
  ++s;
  if (*s < '1' || *s > '9') return false;
  while (*s >= '0' && *s <= '9') ++s;
  return *s == '\0';
}
static bool is_hexadecimal(const char *s)
{
  if (*s == '+' || *s == '-') ++s;
  if (*s != '0') return false;
  ++s;
  if (*s != 'x' && *s != 'X') return false;
  ++s;
  if ((*s < '0' || *s > '9') &&
      (*s < 'a' || *s > 'f') &&
      (*s < 'A' || *s > 'F')) return false;
  while ((*s >= '0' && *s <= '9') ||
         (*s >= 'a' && *s <= 'f') ||
         (*s >= 'A' && *s <= 'F')) ++s;
  if (*s == 'l' || *s == 'L') ++s;
  return *s == '\0';
}
static bool is_octal(const char *s)
{
  if (*s == '+' || *s == '-') ++s;
  if (*s != '0') return false;
  while (*s >= '0' && *s <= '7') ++s;
  if (*s == 'l' || *s == 'L') ++s;
  return *s == '\0';
}
#else // new implementation, also to be used for operator>>

static char rt[4096];
static int rt_pos = 0;

static char next(const char*& s, std::istream** is)
{
  if (!is || *s) { char c = *s; if (c) ++rt_pos, ++s; return c; }
  if (rt_pos == 4096) return '\0';
  (*is)->get(rt[rt_pos]); // read a single byte from istream
  if (*s) ++s; // in case s == rt+rt_pos
  rt[++rt_pos] = '\0'; return rt[rt_pos-1];
}

static bool is_decimal(const char* s, std::istream** is = 0)
{
  rt_pos = 0;
  char c = next(s,is);
  while (c == ' ' || c == '\t' || c == '\n' || c == '\r') c = next(s,is);
  if (c == '+' || c == '-') c = next(s,is);
  if (c < '1' || c > '9') return false;
  while (c >= '0' && c <= '9') c = next(s,is);
  if (c == 'l' || c == 'L') c = next(s,is);
  if (rt_pos > 0) rt[++rt_pos] = '\0';
  return is ? true : c == '\0';
}
static bool is_exponential(const char* s, std::istream** is = 0)
{
  rt_pos = 0;
  char c = next(s,is);
  while (c == ' ' || c == '\t' || c == '\n' || c == '\r') c = next(s,is);
  if (c == '+' || c == '-') c = next(s,is);
  if (c < '1' || c > '9') return false;
  while (c >= '0' && c <= '9') c = next(s,is);
  if (c != 'e' && c != 'E') return false;
  c = next(s,is);
  if (c == '+') c = next(s,is); // no negative exponent!
  if (c < '0' || c > '9') return false;
  while (c >= '0' && c <= '9') c = next(s,is);
  if (rt_pos > 0) rt[++rt_pos] = '\0';
  return is ? true : c == '\0';
}
static bool is_hexadecimal(const char* s, std::istream** is = 0)
{
  rt_pos = 0;
  char c = next(s,is);
  while (c == ' ' || c == '\t' || c == '\n' || c == '\r') c = next(s,is);
  if (c == '+' || c == '-') c = next(s,is);
  if (c != '0') return false;
  c = next(s,is);
  if (c != 'x' && c != 'X') return false;
  c = next(s,is);
  if ((c < '0' || c > '9') &&
      (c < 'a' || c > 'f') &&
      (c < 'A' || c > 'F')) return false;
  while ((c >= '0' && c <= '9') ||
         (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F')) c = next(s,is);
  if (c == 'l' || c == 'L') c = next(s,is);
  if (rt_pos > 0) rt[++rt_pos] = '\0';
  return is ? true : c == '\0';
}
static bool is_octal(const char* s, std::istream** is = 0)
{
  rt_pos = 0;
  char c = next(s,is);
  while (c == ' ' || c == '\t' || c == '\n' || c == '\r') c = next(s,is);
  if (c == '+' || c == '-') c = next(s,is);
  if (c != '0') return false;
  while (c >= '0' && c <= '7') c = next(s,is);
  if (c == 'l' || c == 'L') c = next(s,is);
  if (rt_pos > 0) rt[++rt_pos] = '\0';
  return is ? true : c == '\0';
}
#endif

/** Creates a VNL::BigNum from the character string representation.
*/

VNL::BigNum::BigNum (const char *s)
: count(0), sign(1), data(0) {
  // decimal:     "^ *[-+]?[1-9][0-9]*$"
  // exponential: "^ *[-+]?[1-9][0-9]*[eE][+]?[0-9]+$"
  // hexadecimal: "^ *[-+]?0[xX][0-9a-fA-F]+$"
  // octal:       "^ *[-+]?0[0-7]*$"

  if (is_decimal(s))                    // If string is decimal
    this->DToBigNum(s);                 // convert decimal to VNL::BigNum
  else if (is_exponential(s))           // If string is exponential
    this->ExpToBigNum(s);               // convert exp. to VNL::BigNum
  else if (is_hexadecimal(s))           // If string is hex,
    this->XToBigNum(s);                 // convert hex to VNL::BigNum
  else if (is_octal(s))                 // If string is octal
    this->OToBigNum(s);                 // convert octal to VNL::BigNum
  else {                                // Otherwise
    std::cerr << "Cannot convert string " << s << " to VNL::BigNum\n";
  }
}

/** Reads a VNL::BigNum from a stream.
*/

std::istream& VNL::operator>> (std::istream& is, VNL::BigNum& x)
{
  // decimal:     "^ *[-+]?[1-9][0-9]*$"
  // exponential: "^ *[-+]?[1-9][0-9]*[eE][+]?[0-9]+$"
  // hexadecimal: "^ *[-+]?0[xX][0-9a-fA-F]+$"
  // octal:       "^ *[-+]?0[0-7]*$"
  std::istream* isp = &is;
  rt[0] = '\0';

  if (is_exponential(rt,&isp))          // If input stream string is exponential
    x.ExpToBigNum(rt);                  // convert exp. to VNL::BigNum
  else if (is_decimal(rt,&isp))         // If string is decimal
    x.DToBigNum(rt);                    // convert decimal to VNL::BigNum
  else if (is_hexadecimal(rt,&isp))     // If string is hex,
    x.XToBigNum(rt);                    // convert hex to VNL::BigNum
  else if (is_octal(rt,&isp))           // If string is octal
    x.OToBigNum(rt);                    // convert octal to VNL::BigNum
  else {                                // Otherwise
    std::cerr << "Cannot convert string " << rt << " to VNL::BigNum\n";
    x = 0L;
  }
  return is; // FIXME - should probably push back read characters to istream
}

/** Copies the contents of VNL::BigNum b.
*/

VNL::BigNum::BigNum (const VNL::BigNum& b)
: count(b.count), sign(b.sign) {
  this->data =                                  // Allocate data if necessary
    (b.count > 0 ? new Data[b.count] : 0);
  for (Counter i = 0; i < this->count; i++)     // Copy b data
    this->data[i] = b.data[i];
}


/** Frees space for VNL::BigNum.
*/

VNL::BigNum::~BigNum () {
  delete [] this->data;                         // Delete any allocated data
}

/** Copies rhs VNL::BigNum to lhs VNL::BigNum.
*/

VNL::BigNum& VNL::BigNum::operator= (const VNL::BigNum& rhs) {
  if (this != &rhs) {                           // Avoid self-assignment
    delete [] this->data;                       // Delete existing data
    this->count = rhs.count;                    // Copy rhs's count
    this->data =                                // Allocate data if necessary
      (this->count > 0 ? new Data[this->count] : 0);
    for (Counter i = 0; i < this->count; i++)   // Copy rhs's data
      this->data[i] = rhs.data[i];
    this->sign = rhs.sign;                      // Copy rhs's sign
  }
  return *this;                                 // Return reference
}

/** Returns the negation of a VNL::BigNum.
*/

VNL::BigNum VNL::BigNum::operator- () const {
  VNL::BigNum neg(*this);
  if (neg.count)                // So long as this is non-zero
    neg.sign *= -1;             // Flip its sign
  return neg;
}


/** Prefix increment.\ Increments a VNL::BigNum by 1, and returns it.
*/

VNL::BigNum& VNL::BigNum::operator++ () {
  return *this = *this + 1L;
}


/** Prefix decrement.\ Decrements a VNL::BigNum by 1, and returns it.
*/

VNL::BigNum& VNL::BigNum::operator-- () {
  return *this = *this - 1L;
}

/** Adds two VNL::BigNums, and returns new sum.
*/

VNL::BigNum VNL::BigNum::operator+(const VNL::BigNum& b) const {
  VNL::BigNum sum;                       // Init sum to zero
  if (this->sign == b.sign) {           // If both have same sign
    Add(*this,b,sum);                   //   Do simple addition
    sum.sign = this->sign;              // Attach proper sign
  }
  else {                                // Else different signs
    int mag = MagnitudeCmp(*this,b);   // Determine relative sizes
    if (mag > 0) {                      // If abs(*this) > abs(b)
      Subtract(*this,b,sum);            //   sum = *this - b
      sum.sign = this->sign;            // Sign of sum follows *this
    }
    else if (mag < 0) {                 // Else if abs(*this) < abs(b)
      Subtract(b,*this,sum);            //   sum = b - *this
      sum.sign = b.sign;                // Sign of sum follows b
    }                                   // (Else abs(*this) == abs(b)
  }                                     //   so sum must be zero)
  return sum;                           // shallow swap on return
}


/** Multiplies this with a VNL::BigNum.
*/

VNL::BigNum& VNL::BigNum::operator*= (const VNL::BigNum& b) {
  if (b.count == 0 || this->count == 0)
    return (*this)=0L;
  VNL::BigNum prod;
  prod.data = new Data[prod.count = this->count + b.count]; // allocate data for product
  for (Counter i = 0; i < b.count; i++)         //   multiply each b "digit"
    MultiplyAux(*this, b.data[i], prod, i);    //   times b1 and add to total
  prod.sign = this->sign * b.sign;              //   determine correct sign
  prod.Trim();                                  //   trim excess data and ret.
  return (*this)=prod;
}


/** Divides this by a VNL::BigNum.
*/

VNL::BigNum& VNL::BigNum::operator/= (const VNL::BigNum& b) {
  VNL::BigNum quot;                      // Quotient
  if (b.count == 0) {                   // If b is zero
    if (this->count == 0) {             // If this == zero
      assert(!"division 0/0");
    }
    else if (this->sign > 0) {          // If this > zero
      assert(!"division +1/0");
    }
    else if (this->sign < 0) {          // If this < zero
      assert(!"division -1/0");
    }
  }
  else {
    VNL::BigNum r;                       // Remainder
    Divide(*this,b,quot,r);             // Call divide fn
  }
  return (*this)=quot;
}

// Divides this by a VNL::BigNum and replaces this by remainder.

VNL::BigNum& VNL::BigNum::operator%= (const VNL::BigNum& b) {
  VNL::BigNum remain,q;                  // Quotient and remainder
  if (b.count == 0) {                   // if b == 0
    assert(!"division 1/0");
  }
  else {                                // otherwise,
    Divide(*this,b,q,remain);           // divide b1 by b and save remainder
  }
  return (*this)=remain;                // shallow swap on return
}


/**
*/

VNL::BigNum VNL::BigNum::operator<< (int l) const {
  if (l == 0 || *this == 0L)            // if either arg is zero
    return VNL::BigNum(*this);
  if (l < 0)                            // if shift amt is negative
    return RightShift(*this,-l);       //   do an actual right shift
  else                                  // otherwise
    return LeftShift(*this,l);         //   do a left shift
}


/** Shifts bignum to the left/right l digits.
*/

VNL::BigNum VNL::BigNum::operator>> (int l) const {
  if (l == 0 || *this == 0L)            // if either arg is zero
    return VNL::BigNum(*this);
  if (l < 0)                            // if shift amt is negative
    return LeftShift(*this,-l);        //   do an actual left shift
  else                                  // else
    return RightShift(*this,l);        //   do a right shift
}


/** Two VNL::BigNums are equal if and only if they have the same integer representation.
*/

bool VNL::BigNum::operator== (const VNL::BigNum& rhs) const {
  if (this != &rhs) {                           // Check address
    if (this->sign != rhs.sign) return false;   // Different sign implies !=
    if (this->count != rhs.count) return false; // Different size implies !=
    for (Counter i = 0; i < this->count; i++)   // Each data element the same?
      if (this->data[i] != rhs.data[i]) return false; // No. Return !=
  }
  return true;                                    // Yes. Return ==
}


/** Compares two VNL::BigNums.
*/

bool VNL::BigNum::operator< (const VNL::BigNum& rhs) const {
  if (this->sign < rhs.sign) return true;       // Different signs?
  if (this->sign > rhs.sign) return false;
  if (this->sign == 1)                          // Both signs == 1
    return MagnitudeCmp(*this,rhs) < 0;        // this must be smaller
  else                                          // Both signs == -1
    return MagnitudeCmp(*this,rhs) > 0;        // this must be larger
}


/** Formatted output for bignum.
*/

std::ostream& VNL::operator<< (std::ostream& os, const VNL::BigNum& b) {
  VNL::BigNum d = b;                     // Copy the input VNL::BigNum
  if (d.sign == -1) {                   // If it's negative
    os.put('-');                  //   Output leading minus sign
    d.sign = 1;                         //   Make d positive for divide
  }
  VNL::BigNum q,r;                       // Temp quotient and remainder
  char *cbuf = new char[5 * (b.count+1)];   // Temp character buffer
  Counter i = 0;
  do {                                  // repeat:
    Divide(d,10L,q,r);                  //   Divide VNL::BigNum by ten
    cbuf[i++] = char(long(r) + '0');    //   Get one's digit
    d = q;                              //   Then discard one's digit
    q = r = 0L;                         //   Prep for next divide
  } while (d != 0L);                    // until no more one's digits
  do {                                  // repeat;
    os.put(cbuf[--i]);                  //   output char buf in reverse
  } while (i);                          // until no more chars
  delete [] cbuf;                       // delete temp char buf
  return os;                            // return output stream
}

/** Convert the number to a decimal representation in a string.
*/
std::string& VNL::BigNumToString (std::string& s, const VNL::BigNum& b)
{
  s.erase();
  std::string::size_type insert_point = 0; // keep record of location of first number.

  VNL::BigNum d = b;                     // Copy the input VNL::BigNum
  if (d.sign == -1) {                   // If it's negative
    s.insert(insert_point,"-");         //   Output leading minus sign
    d.sign = 1;                         //   Make d positive for divide
    ++insert_point;                     // keep record of location of first number.
  }
  VNL::BigNum q,r;                       // Temp quotient and remainder
  do {                                  // repeat:
    Divide(d,10L,q,r);                  //   Divide VNL::BigNum by ten
    s.insert(insert_point, 1, char('0'+long(r))); //   Get one's digit, and insert it at head.
    d = q;                              //   Then discard one's digit
    q = r = 0L;                         //   Prep for next divide
  } while (d != 0L);                    // until no more one's digits
  return s;
}

/** Convert the number from a decimal representation in a string.
*/
VNL::BigNum& VNL::BigNumFromString (VNL::BigNum& b, const std::string& s)
{
  // decimal:     "^ *[-+]?[1-9][0-9]*$"

  b.DToBigNum(s.c_str());                    // convert decimal to VNL::BigNum
  return b; 
}


/** Implicit conversion from a VNL::BigNum to a short.
*/
VNL::BigNum::operator short () const {
  short s = 0;
  for (Counter i = this->count; i > 0; )
    s = short(s*0x10000 + this->data[--i]);
  return this->sign*s;
}


/** Implicit conversion from a VNL::BigNum to an int.
*/
VNL::BigNum::operator int () const {
  int j = 0;
  for (Counter i = this->count; i > 0; )
    j = int(j*0x10000 + this->data[--i]);
  return this->sign*j;
}


/** Implicit conversion from a VNL::BigNum to a long.
*/
VNL::BigNum::operator long () const {
  long l = 0;
  for (Counter i = this->count; i > 0; )
    l = l*0x10000L + this->data[--i];
  return this->sign*l;
}


/** Implicit conversion from a VNL::BigNum to a float.
*/
VNL::BigNum::operator float () const {
  float f = 0.0f;
  for (Counter i = this->count; i > 0; )
    f = f*0x10000 + this->data[--i];
  return this->sign*f;
}


/** Implicit conversion from a VNL::BigNum to a double.
*/

VNL::BigNum::operator double () const {
  double d = 0.0;
  for (Counter i = this->count; i > 0; )
    d = d*0x10000 + this->data[--i];
  return this->sign*d;
}

/** Implicit conversion from a VNL::BigNum to a long double.
*/

VNL::BigNum::operator long double () const {
  long double d = 0.0;
  for (Counter i = this->count; i > 0; )
    d = d*0x10000 + this->data[--i];
  return this->sign*d;
}

/** dump the contents of a VNL::BigNum to a stream, default cout.
*/

void VNL::BigNum::Dump (std::ostream& os) const {
  os << "{count=" << this->count       // output count field
     << ", sign=" << this->sign        // output sign field
     << ", data=" << this->data        // output data pointer
     << ", value=" << *this
     << ", {";
// format string == "%04X%s" or "%02X%s", etc.
//  static char format_str[10] =
//    {'%','0',char(2*2 + '0'),'X','%','s'};
//  format_str[2] = char(2*2 + '0');
  if (this->count > 0) { // output data array
    for (Counter i = this->count; i > 1; i--)
      os << (this->data[i - 1]) << ",";
    os << (this->data[0]);
  }
  os << "}}\n";                         // close brackets
}


// DToBigNum // Converts decimal string to a VNL::BigNum.

int VNL::BigNum::DToBigNum (const char *s) {
  Counter len = 0;                      // No chars converted yet
  while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') ++s; // skip whitespace
  if (s[0] == '-' || s[0] == '+') len++;// Skip over leading +,-
  while (isdigit(s[len])) {         // If current char is digit
    (*this) = ((*this) * 10L) +         // Shift VNL::BigNum left a decimal
      VNL::BigNum(long(s[len++] - '0')); // digit and add new digit
  }
  if (s[0] == '-') this->sign = -1;     // If s had leading -, note it
  return len;                           // Return # of chars processed
}

// ExpToBigNum // convert exponential string to a VNL::BigNum

void VNL::BigNum::ExpToBigNum (const char *s) {
  while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') ++s; // skip whitespace
  Counter pos = this->DToBigNum(s) + 1; // Convert the base, skip [eE]
  long pow = atol(s + pos);             // Convert the exponent to long
  while (pow-- > 0)                     // Raise VNL::BigNum to the given
    *this = (*this) * 10L;              // power
}


// ctox // convert hex character to integer hex value (ASCII or EBCDIC)
// Inputs:  character representation of a hex number
// Outputs:  integer value of the hex number

unsigned int ctox (int c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  return c - 'A' + 10;
}

// XToBigNum // convert hex string to VNL::BigNum

void VNL::BigNum::XToBigNum (const char *s) {
  while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') ++s; // skip whitespace
  Counter size = strlen(s);
  Counter len = 2;                      // skip leading "0x"
  while (len < size) {                  // While there are more chars
    (*this) = ((*this) * 16L) +         // Shift VNL::BigNum left one hex
      VNL::BigNum(long(ctox(s[len++]))); //   digit and add next digit
  }
}


// OToBigNum // convert octal string to VNL::BigNum

void VNL::BigNum::OToBigNum (const char *s) {
  while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') ++s; // skip whitespace
  Counter size = strlen(s);
  Counter len = 0;                      // No chars converted yet
  while (len < size) {                  // While there are more chars
    (*this) = ((*this) * 8L) +          // Shift VNL::BigNum left 1 oct dig.
      VNL::BigNum(long(s[len++] - '0')); // Add next character value
  }
}

/** change the data allotment for a VNL::BigNum.
*/

void VNL::BigNum::Resize (short new_count) {
  assert(new_count >= 0);
  if (new_count == this->count) return;
  Data *new_data = (new_count > 0 ? new Data[new_count] : 0); // Allocate data if necessary

  if (this->count <= new_count) {       // Copy old data into new
    short i = 0;
    for (; i < this->count; i++)
      new_data[i] = this->data[i];
    for (; i < new_count; i++)
      new_data[i] = 0;
  }
  else {
    for (short i = 0; i < new_count; i++)
      new_data[i] = this->data[i];
  }

  delete [] this->data;                 // Get rid of old data
  this->data = new_data;                // Point to new data
  this->count = new_count;              // Save new count
}


// trim // trim VNL::BigNum of excess data allotment

VNL::BigNum& VNL::BigNum::Trim () {
  Counter i = this->count;
  for (; i > 0; i--)                    // Skip over high-order words
    if (this->data[i - 1] != 0) break;  //   that are zero
  if (i < this->count) {                // If there are some such words
    this->count = i;                    // Update the count
    Data *new_data = (i > 0 ? new Data[i] : 0); // Allocate data if necessary
    for (; i > 0; i--)                  // Copy old data into new
      new_data[i - 1] = this->data[i - 1];
    delete [] this->data;               // Delete old data
    this->data = new_data;              // Point to new data
  }
  return *this;                         // return reference to VNL::BigNum
}


// add // add two VNL::BigNum values and save their sum

void VNL::Add (const VNL::BigNum& b1, const VNL::BigNum& b2, VNL::BigNum& sum) {
  const VNL::BigNum *bmax, *bmin;        // Determine which of the two
  if (b1.count >= b2.count) {           //   addends has the most
    bmax = &b1;                         //   data.
    bmin = &b2;
  }
  else {
    bmax = &b2;
    bmin = &b1;
  }
  sum.data = ((sum.count = bmax->count) > 0 ?   // Allocate data for their sum
              new Data[sum.count] : 0);
  unsigned long temp, carry = 0;
  Counter i = 0;
  while (i < bmin->count) {             // Add, element by element.
    // Add both elements and carry
    temp = (unsigned long)b1.data[i] + (unsigned long)b2.data[i] + carry;
    carry = temp/0x10000L;              // keep track of the carry
    sum.data[i] = Data(temp);           // store sum
    i++;                                // go to next element
  }
  while (i < bmax->count) {             // bmin has no more elements
    temp = bmax->data[i] + carry;       // propagate the carry through
    carry = temp/0x10000L;              // the rest of bmax's elements
    sum.data[i] = Data(temp);           // store sum
    i++;
  }
  if (carry) {                          // if carry left over
    sum.Resize(bmax->count + 1);        //   allocate another word
    sum.data[bmax->count] = 1;          //   save the carry in it
  }
}


// subtract // subtract min VNL::BigNum from max VNL::BigNum (unsigned), result in diff

void VNL::Subtract (const VNL::BigNum& bmax, const VNL::BigNum& bmin, VNL::BigNum& diff) {
  diff.data = new Data[diff.count = bmax.count];// Allocate data for difference
  unsigned long temp;
  int borrow = 0;
  Counter i = 0;
  for (; i < bmin.count; i++) {                 // Subtract word by word.
    temp = (unsigned long)bmax.data[i] + 0x10000L - borrow; // Add radix to bmax's data
    temp -= (unsigned long)bmin.data[i];        // Subtract off bmin's data
    borrow = (temp/0x10000L == 0);              // Did we have to borrow?
    diff.data[i] = (Data) temp;                 // Reduce modulo radix and save
  }
  for (; i < bmax.count; i++) {                 // No more data for bmin
    temp = (unsigned long)bmax.data[i] + 0x10000L - borrow; // Propagate the borrow through
    borrow = (temp/0x10000L == 0);              //   rest of bmax's data
    diff.data[i] = (Data) temp;
  }
  diff.Trim();                                  // Done. Now trim excess data
}


// magnitude_cmp // compare absolute values of two VNL::BigNums
// Outputs:  result of comparison:  -1 if abs(b1) < abs(b2)
//                                   0 if abs(b1) == abs(b2)
//                                  +1 if abs(b1) > abs(b2)

int VNL::MagnitudeCmp (const VNL::BigNum& b1, const VNL::BigNum& b2) {
  if (b1.count > b2.count) return 1;    // If one has more data than
  if (b2.count > b1.count) return -1;   //   the other, it wins
  Counter i = b1.count;                 // Else same number of elmts
  while (i > 0) {                       // Do lexicographic comparison
    if (b1.data[i - 1] > b2.data[i - 1])
      return 1;
    else if (b1.data[i - 1] < b2.data[i - 1])
      return -1;
    i--;
  }                                     // No data, or all elmts same
  return 0;                             //  so must be equal
}


// MultiplyAux // multiply a VNL::BigNum by a "single digit"
// Inputs:  VNL::BigNum reference, single word multiplier, reference to the product,
//          and index of starting storage location to use in product

void VNL::MultiplyAux (const VNL::BigNum& b, Data d, VNL::BigNum& prod, Counter i) {
  // this function works just like normal multiplication by hand, in that the
  // top number is multiplied by the first digit of the bottom number, then the
  // second digit, and so on.  The only difference is that instead of doing all
  // of the multiplication before adding the rows, addition is done
  // concurrently.
  if (i == 0) {                         // if index is zero
    Counter j = 0;                      //   then zero out all of
    while (j < prod.count)              //   prod's data elements
      prod.data[j++] = 0;
  }
  if (d != 0) {                         // if d == 0, nothing to do
    unsigned long temp;
    Data carry = 0;

    Counter j = 0;
    for (; j < b.count; j++) {
      // for each of b's data elmts, multiply times d and add running product
      temp = (unsigned long)b.data[j] * (unsigned long)d
           + (unsigned long)prod.data[i + j] + carry;
      prod.data[i + j] = Data(temp % 0x10000L); //   store result in product
      carry = Data(temp/0x10000L);              //   keep track of carry
    }
    if (i+j < prod.count)
      prod.data[i + j] = carry;                 // Done.  Store the final carry
  }
}

// normalize // normalize two VNL::BigNums  (Refer to Knuth, V.2, Section 4.3.1,
//              Algorithm D for details.  A digit here is one data element in
//              the radix 2**2.)
// Inputs:  references to two VNL::BigNums b1, b2, and their normalized counterparts
// Outputs:  the integral normalization factor used

Data VNL::Normalize (const VNL::BigNum& b1, const VNL::BigNum& b2, VNL::BigNum& u, VNL::BigNum& v) {
  Data d = Data(0x10000L/(long(b2.data[b2.count - 1]) + 1)); // Calculate normalization factor.
  u.data = new Data[u.count = b1.count + 1];    // Get data for u (plus extra)
  v.data = new Data[v.count = b2.count];        // Get data for v
  u.data[b1.count] = 0;                         // Set u's leading digit to 0
  MultiplyAux(b1,d,u,0);                       // u = b1 * d
  MultiplyAux(b2,d,v,0);                       // v = b2 * d
  return d;                                     // return normalization factor
}


// divide_aux // divide a VNL::BigNum by a "single digit" (Refer to Knuth, V.2,
//               Section 4.3.2, exercise 16 for details.  A digit here is one
//               data element in the radix 2**2.)
// Inputs:  reference to VNL::BigNum dividend, single digit divisor d, VNL::BigNum
//          quotient, and single digit remainder r

void VNL::DivideAux (const VNL::BigNum& b1, Data d, VNL::BigNum& q, Data& r) {
  r = 0;                                        // init remainder to zero
  unsigned long temp;
  for (Counter j = b1.count; j > 0; j--) {
    temp = (unsigned long)r*0x10000L + (unsigned long)b1.data[j - 1]; // get remainder, append next
    if (j < 1 + q.count)
      q.data[j - 1] = Data(temp/d);             //   digit, then divide
    r = Data(temp % d);                         // calculate new remainder
  }
}


/** estimate next dividend (Refer to Knuth, V.2, Section.
*  4.3.1, Algorithm D for details.  This function estimates
*  how many times v goes into u, starting at u's jth digit.
*  A digit here is actually a data element, thought of as
*  being in the radix 2**2.)
*
* Inputs:  reference to VNL::BigNum dividend and divisor and starting digit j
*
* Outputs:  estimated number of times v goes into u
*/

Data VNL::EstimateQHat (const VNL::BigNum& u, const VNL::BigNum& v, Counter j) {
  Data q_hat,
       v1 = v.data[v.count - 1],                // localize frequent data
       v2 = v.data[v.count - 2],
       u0 = u.data[u.count - 1 - j],
       u1 = u.data[u.count - 2 - j],
       u2 = u.data[u.count - 3 - j];

  // Initial Knuth estimate, usually correct
  q_hat = (u0 == v1 ? Data(0xffff) : Data(long(u0 * 0x10000L + u1) / long(v1)));

  // high speed test to determine most of the cases in which initial
  // estimate is too large.  Eliminates most cases in which q_hat is one too
  // large.  Eliminates all cases in which q_hat is two too large.  The test
  // looks hairy because we have to watch out for overflow.  In the book, this
  // test is the simple inequality:
  //     v2*q_hat > (u0*0x10000L + u1 - q_hat*v1)*0x10000L + u2.
  // If the inequality is true, decrease q_hat by 1.  If inequality is still
  // true, decrease q_hat again.
  unsigned long lhs, rhs;               // lhs, rhs of Knuth inequality
  for (Counter i = 0; i < 2; i++) {     // loop at most twice
    lhs = v2 * q_hat;                   // Calculate left-hand side of ineq.
    rhs = u0 * 0x10000L + u1;// Calculate part of right-hand side
    rhs -= (q_hat * v1);                // Now subtract off part

    // DML:  My attempt to fix the overflow testing bug..
    double temp_rhs = double(rhs);
    double temp_radix_s = double(0x10000);
    // OLD WAY: if (rhs > rhs * 0x10000)// if multiplication causes overflow
    // NEW WAY: see if result won't fit into a long.
    if ( temp_rhs * temp_radix_s > double(0x7fffffffL) )
      break;                            //   then rhs > lhs, so test fails
    rhs *= 0x10000L;                    // No overflow:  ok to multiply

    temp_rhs = double(rhs);
    double temp_u2 = double(u2);
    // OLD WAY: if (rhs > rhs + u2)     // if addition yields overflow
    if ( temp_rhs + temp_u2 > double(0x7fffffffL) ) // NEW WAY.
      break;                            //   then rhs > lhs, so test fails
    rhs += u2;                          // No overflow: ok to add.
    if (lhs <= rhs)                     // if lhs <= rhs
      break;                            //   test fails
    q_hat--;                            // Test passes:  decrement q_hat
  }                                     // Loop again
  return q_hat;                         // Return estimate
}


/** calculate u - v*q_hat (Refer to Knuth, V.\ 2, Section.
*  4.3.1, Algorithm D for details.  A digit here is a
*  data element, thought of as being in the radix
*  2**2.)
*
* Inputs:  reference to VNL::BigNum dividend, divisor, estimated result, and index
*          into jth digit of dividend
*
* Outputs:  true number of times v goes into u
*/

Data VNL::MultiplySubtract (VNL::BigNum& u, const VNL::BigNum& v, Data q_hat, Counter j) {
  // At this point it has been estimated that v goes into the jth and higher
  // digits of u about q_hat times, and in fact that q_hat is either the
  // correct number of times or one too large.

  if (q_hat == 0) return q_hat;         // if q_hat 0, nothing to do
  VNL::BigNum rslt;                      // create a temporary VNL::BigNum
  Counter tmpcnt;
  rslt.data =                           // allocate data for it
     new Data[rslt.count = v.count + 1];

  // simultaneous computation of u - v*q_hat
  unsigned long prod, diff;
  Data carry = 0, borrow = 0;
  Counter i = 0;
  for (; i < v.count; i++) {
    // for each digit of v, multiply it by q_hat and subtract the result
    prod = (unsigned long)v.data[i] * (unsigned long)q_hat + carry;
    diff = (unsigned long)u.data[u.count - v.count - 1 - j + i] + 0x10000L - borrow;
    diff -= (unsigned long)Data(prod);  //   form proper digit of u
    rslt.data[i] = Data(diff);          //   save the result
    borrow = (diff/0x10000L == 0);      //   keep track of any borrows
    carry = Data(prod/0x10000L);        //   keep track of carries
  }
  tmpcnt = u.count - v.count - 1 - j + i;
  diff = (unsigned long)u.data[tmpcnt]  //  special case for the last
         + 0x10000L - borrow;           //  digit
  diff -= carry;
  rslt.data[i] = Data(diff);
  borrow = (diff/0x10000L == 0);

  // A leftover borrow indicates that u - v*q_hat is negative, i.e., that
  // q_hat was one too large.  So to get correct result, decrement q_hat and
  // add back one multiple of v
  if (borrow) {
    q_hat--;
    carry = 0;
    unsigned long sum;
    for (i = 0; i < v.count; i++) {
      sum = (unsigned long)rslt.data[i] + (unsigned long)v.data[i] + carry;
      carry = Data(sum/0x10000L);
      u.data[u.count - v.count - 1 - j + i] = Data(sum);
    }
    u.data[u.count - v.count - 1 - j + i] = rslt.data[i] + carry;
  }
  else {                                // otherwise, result is ok
    for (i = 0; i < rslt.count; i++)    // store result back into u
      u.data[u.count - v.count - 1 - j + i] = rslt.data[i];
  }
  return q_hat;                         // return corrected q_hat
}


// divide // divide b2 into b1, getting quotient q and remainder r.  (Refer to
//          Knuth, V.2, Seciton 4.3.1, Algorithm D for details.  This function
//          implements Algorithm D.)
// Inputs:  references to a VNL::BigNum dividend b1, divisor b2, quotient q, and
//          remainder r.

void VNL::Divide (const VNL::BigNum& b1, const VNL::BigNum& b2, VNL::BigNum& q, VNL::BigNum& r) {
  q = r = 0L;
  if (b1 == 0L)                      // If divisor is zero
    return;                          //   return zero quotient and remainder
  int mag = MagnitudeCmp(b1,b2);    // Compare magnitudes
  if (mag < 0)                       // if abs(b1) < abs(b2)
    r = b1;                          //   return zero quotient, b1 remainder
  else if (mag == 0)                 // if abs(b1) == abs(b2)
    q = 1L;                          //   quotient is 1, remainder is 0
  else {                             // otherwise abs(b1) > abs(b2), so divide
    q.data = new Data[q.count = b1.count - b2.count + 1]; // Allocate quotient
    r.data = new Data[r.count = b2.count];                // Allocate remainder
    if (b2.count == 1) {                        // Single digit divisor?
      VNL::DivideAux(b1,b2.data[0],q,r.data[0]);    // Do single digit divide
    }
    else {                                      // Else full-blown divide
      VNL::BigNum u,v;
      Data d = Normalize(b1,b2,u,v);            // Set u = b1/d, v = b2/d
      Data q_hat;                               // Multiplier
      Counter j = 0;
      while (j <= b1.count - b2.count) {        // Main division loop
        q_hat = VNL::EstimateQHat(u,v,j);          // Estimate # times v divides
        q.data[q.count - 1 - j] =               // Do division, get true answ.
          VNL::MultiplySubtract(u,v,q_hat,j);
        j++;
      }
      static Data dufus;                // dummy variable
      VNL::DivideAux(u,d,r,dufus);          // Unnormalize u for remainder
    }
    q.Trim();                           // Trim leading zeros of quot.
    r.Trim();                           // Trim leading zeros of rem.
  }
  q.sign = r.sign = b1.sign * b2.sign;  // Calculate signs
}


// left_shift // left shift (arithmetic) VNL::BigNum by positive number.
// Inputs:  reference to VNL::BigNum, positive shift value
// Outputs:  VNL::BigNum, multiplied by the corresponding power of two

VNL::BigNum VNL::LeftShift (const VNL::BigNum& b1, int l) {
  // to carry out this arithmetic left shift, we cheat.  Instead of physically
  // shifting the data array l bits to the left, we shift just enough to get
  // the correct word alignment, and then pad the array on the right with as
  // many zeros as we need.
  VNL::BigNum rslt;                      // result of shift
  rslt.sign = b1.sign;                  // result follows sign of input
  Counter growth = Counter(l / 16);     // # of words rslt will grow by
  Data shift = Data(l % 16);            // amount to actually shift
  Data rshift = 16 - shift;             // amount to shift next word by
  Data carry =                          // value that will be shifted
    b1.data[b1.count - 1] >> (16 - shift); // out end of current array
  rslt.data =                              // allocate new data array
    new Data[rslt.count = b1.count + growth + (carry != 0 ? 1 : 0)];
  Counter i = 0;
  while (i < growth)                            // zero out padded elements
    rslt.data[i++] = 0;
  rslt.data[i++] = b1.data[0] << shift;         // shift first non-zero element
  while (i < rslt.count - 1) {                  // for remaining data words
    rslt.data[i] = (b1.data[i - growth] << shift) + // shift current data word
      (b1.data[i - 1 - growth] >> rshift);          // propagate adjacent
    i++;                                        // carry into current word
  }
  if (i < rslt.count) {
    if (carry)                                  // if last word had overflow
      rslt.data[i] = carry;                     //   store it new data
    else                                        // otherwise,
      rslt.data[i] = (b1.data[i - growth] << shift) // do like the rest
                   + (b1.data[i - 1 - growth] >> rshift);
  }
  VNL::BigNum& result = *((VNL::BigNum*) &rslt);// same physical object
  return result;                                // shallow swap on return
}


// right_shift // right shift (arithmetic) VNL::BigNum by positive number.
// Inputs:  reference to VNL::BigNum, positive shift value
// Outputs:  VNL::BigNum, divided by the corresponding power of two

VNL::BigNum VNL::RightShift (const VNL::BigNum& b1, int l) {
  VNL::BigNum rslt;                              // result of shift
  Counter shrinkage = Counter(l / 16);          // # of words rslt will shrink
  Data shift = Data(l % 16);                    // amount to actually shift
  Data dregs = (b1.data[b1.count-1] >> shift);  // high end data to save
  if (shrinkage + (dregs == 0) < b1.count) {    // if not all data shifted out
    rslt.sign = b1.sign;                        // rslt follows sign of input
                                                // allocate new data
    rslt.data = new Data[rslt.count = b1.count - shrinkage - (dregs == 0 ? 1 : 0)];
    Data lshift = 16 - shift;                   // amount to shift high word
    Counter i = 0;
    while (i < rslt.count - 1) {                // shift current word
      rslt.data[i] = (b1.data[i + shrinkage] >> shift) + // propagate adjacent
        (b1.data[i + shrinkage + 1] << lshift); //   word into current word
      i++;
    }
    if (dregs)                                  // don't lose dregs
      rslt.data[i] = dregs;
    else {
      rslt.data[i] = (b1.data[i + shrinkage] >> shift) +
        (b1.data[i + shrinkage + 1] << lshift);
    }
  }
  VNL::BigNum& result = *((VNL::BigNum*) &rslt);  // same physical object
  return result;                                // shallow swap on return
}

#ifdef VCL_GCC_EGCS // this includes egcs, gcc 2.8 and gcc 2.95
#include <std/complext.cc>
template complex<VNL::BigNum>& __doadv<VNL::BigNum>(complex<VNL::BigNum>*, complex<VNL::BigNum> const&);
template complex<VNL::BigNum> operator/(complex<VNL::BigNum> const&, complex<VNL::BigNum> const&);
template complex<VNL::BigNum> sqrt(complex<VNL::BigNum> const&);
#endif
