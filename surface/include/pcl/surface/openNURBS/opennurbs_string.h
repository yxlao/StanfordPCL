/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2011 Robert McNeel & Associates. All rights reserved.
// OpenNURBS, Rhinoceros, and Rhino3D are registered trademarks of Robert
// McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//
// For complete openNURBS copyright information see <http://www.opennurbs.org>.
//
////////////////////////////////////////////////////////////////
*/

#if !defined(ON_STRING_INC_)
#define ON_STRING_INC_

/*
  This class is intended to be used to determine if a file's
  contents have changed.
*/
class ON_CLASS ON_CheckSum {
  public:
    ON_CheckSum();
    ~ON_CheckSum();

    // zeros all fields.
    void Zero();

    /*
    Returns:
      True if checksum is set.
    */
    bool IsSet() const;

    // C++ default operator=, operator==,
    // and copy constructor work fine.

    /*
    Descripton:
      Set check sum values for a buffer
    Parameters:
      size - [in]
        number of bytes in buffer
      buffer - [in]
      time - [in]
        last modified time in seconds since Jan 1, 1970, UCT
    Returns:
      True if checksum is set.
    */
    bool SetBufferCheckSum(size_t size, const void *buffer, time_t time);

    /*
    Descripton:
      Set check sum values for a file.
    Parameters:
      fp - [in] pointer to a file opened with ON:FileOpen(...,L"rb")
    Returns:
      True if checksum is set.
    */
    bool SetFileCheckSum(FILE *fp);

    /*
    Descripton:
      Set check sum values for a file.
    Parameters:
      filename - [in] name of file.
    Returns:
      True if checksum is set.
    */
    bool SetFileCheckSum(const wchar_t *filename);

    /*
    Description:
      Test buffer to see if it has a matching checksum.
    Paramters:
      size - [in]   size in bytes
      buffer - [in]
    Returns:
      True if the buffer has a matching checksum.
    */
    bool CheckBuffer(size_t size, const void *buffer) const;

    /*
    Description:
      Test buffer to see if it has a matching checksum.
    Paramters:
      fp - [in] pointer to file opened with ON::OpenFile(...,L"rb")
      bSkipTimeCheck - [in] if true, the time of last
         modification is not checked.
    Returns:
      True if the file has a matching checksum.
    */
    bool CheckFile(FILE *fp, bool bSkipTimeCheck = false) const;

    /*
    Description:
      Test buffer to see if it has a matching checksum.
    Paramters:
      filename - [in]
      bSkipTimeCheck - [in] if true, the time of last
         modification is not checked.
    Returns:
      True if the file has a matching checksum.
    */
    bool CheckFile(const wchar_t *filename, bool bSkipTimeCheck = false) const;

    bool Write(class ON_BinaryArchive &) const;
    bool Read(class ON_BinaryArchive &);

    void Dump(class ON_TextLog &) const;

  public:
    size_t m_size;       // bytes in the file.
    time_t m_time;       // last modified time in seconds since Jan 1, 1970, UCT
    ON__UINT32 m_crc[8]; // crc's
};

/////////////////////////////////////////////////////////////////////////////
//
// ON_String is a char (a.k.a single byte or ascii) string
//
// ON_wString is a wide char (a.k.a double byte or unicode) string
//

class ON_String;  // char (a.k.a single byte or ascii) string
class ON_wString; // wide character (a.k.a double byte or unicode) string

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class ON_CLASS ON_String {
  public:
    // Constructors
    ON_String();
    ON_String(const ON_String &);

    ON_String(const char *);
    ON_String(const char *, int /*length*/); // from substring
    ON_String(char, int = 1 /* repeat count */);

    ON_String(const unsigned char *);
    ON_String(const unsigned char *, int /*length*/); // from substring
    ON_String(unsigned char, int = 1 /* repeat count */);

    // construct a UTF-8 string string from a UTF-16 string.
    ON_String(const wchar_t *src);             // src = UTF-16 string
    ON_String(const wchar_t *src, int length); // from a UTF-16 substring
    ON_String(const ON_wString &src);          // src = UTF-16 string

#if defined(ON_OS_WINDOWS)
    // Windows support
    bool LoadResourceString(HINSTANCE, UINT); // load from Windows string
                                              // resource 2047 chars max
#endif

    void Create();
    void
    Destroy(); // releases any memory and initializes to default empty string
    void EmergencyDestroy();

    /*
    Description:
      Enables reference counting.  I limited cases, this is useful
      for large strings or strings that are frequently passed around.
      Reference counted strings must be carefully managed in
      when multi-threading is used.
    Parameters:
      If EnableReferenceCounting()
      is not called, then the string will not be referanceThe default is to not
    use reference counted strings.
    */
    void EnableReferenceCounting(bool bEnable);

    /*
    Returns:
      True if the string is reference counted.
    */
    bool IsReferenceCounted() const;

    // Attributes & Operations
    // as an array of characters
    int Length() const;
    bool IsEmpty() const; // returns true if length == 0
    void Empty(); // sets length to zero - if possible, memory is retained

    char &operator[](int);
    char operator[](int) const;
    char GetAt(int) const;
    void SetAt(int, char);
    void SetAt(int, unsigned char);
    operator const char *() const; // as a C string

    // overloaded assignment
    ON_String &operator=(const ON_String &);
    ON_String &operator=(char);
    ON_String &operator=(const char *);
    ON_String &operator=(unsigned char);
    ON_String &operator=(const unsigned char *);
    ON_String &operator=(
        const wchar_t *src); // src = UTF-16 string, result is a UTF-8 string
    ON_String &operator=(
        const ON_wString &src); // src = UTF-16 string, result is a UTF-8 string

    // operator+()
    ON_String operator+(const ON_String &) const;
    ON_String operator+(char) const;
    ON_String operator+(unsigned char) const;
    ON_String operator+(const char *) const;
    ON_String operator+(const unsigned char *) const;

    // string comparison
    bool operator==(const ON_String &) const;
    bool operator==(const char *) const;
    bool operator!=(const ON_String &) const;
    bool operator!=(const char *) const;
    bool operator<(const ON_String &) const;
    bool operator<(const char *) const;
    bool operator>(const ON_String &) const;
    bool operator>(const char *) const;
    bool operator<=(const ON_String &) const;
    bool operator<=(const char *) const;
    bool operator>=(const ON_String &) const;
    bool operator>=(const char *) const;

    // string concatenation
    void Append(const char *, int); // append specified number of characters
    void Append(const unsigned char *,
                int); // append specified number of characters
    const ON_String &operator+=(const ON_String &);
    const ON_String &operator+=(char);
    const ON_String &operator+=(unsigned char);
    const ON_String &operator+=(const char *);
    const ON_String &operator+=(const unsigned char *);

    // string comparison
    // If this < string, returns < 0.
    // If this = string, returns 0.
    // If this < string, returns > 0.
    int Compare(const char *) const;
    int Compare(const unsigned char *) const;

    int CompareNoCase(const char *) const;
    int CompareNoCase(const unsigned char *) const;

    // Description:
    //   Simple case sensitive wildcard matching. A question mark (?) in the
    //   pattern matches a single character.  An asterisk (*) in the pattern
    //   mathes zero or more occurances of any character.
    //
    // Parameters:
    //   pattern - [in] pattern string where ? and * are wild cards.
    //
    // Returns:
    //   true if the string mathes the wild card pattern.
    bool WildCardMatch(const char *) const;
    bool WildCardMatch(const unsigned char *) const;

    // Description:
    //   Simple case insensitive wildcard matching. A question mark (?) in the
    //   pattern matches a single character.  An asterisk (*) in the pattern
    //   mathes zero or more occurances of any character.
    //
    // Parameters:
    //   pattern - [in] pattern string where ? and * are wild cards.
    //
    // Returns:
    //   true if the string mathes the wild card pattern.
    bool WildCardMatchNoCase(const char *) const;
    bool WildCardMatchNoCase(const unsigned char *) const;

    /*
    Description:
      Replace all substrings that match token1 with token2
    Parameters:
      token1 - [in]
      token2 - [in]
    Returns:
      Number of times token1 was replaced with token2.
    */
    int Replace(const char *token1, const char *token2);
    int Replace(const unsigned char *token1, const unsigned char *token2);
    int Replace(char token1, char token2);
    int Replace(unsigned char token1, unsigned char token2);

    // simple sub-string extraction
    ON_String Mid(int, // index of first char
                  int  // count
    ) const;
    ON_String Mid(int // index of first char
    ) const;
    ON_String Left(int // number of chars to keep
    ) const;
    ON_String Right(int // number of chars to keep
    ) const;

    // upper/lower/reverse conversion
    void MakeUpper();
    void MakeLower();
    void MakeReverse();
    void TrimLeft(const char * = NULL);
    void TrimRight(const char * = NULL);
    void TrimLeftAndRight(const char * = NULL);

    // remove occurrences of chRemove
    int Remove(const char chRemove);

    // searching (return starting index, or -1 if not found)
    // look for a single character match
    int Find(char) const;
    int Find(unsigned char) const;
    int ReverseFind(char) const;
    int ReverseFind(unsigned char) const;

    // look for a specific sub-string
    int Find(const char *) const;
    int Find(const unsigned char *) const;

    // simple formatting
    void ON_MSC_CDECL Format(const char *, ...);
    void ON_MSC_CDECL Format(const unsigned char *, ...);

    // Low level access to string contents as character array
    void ReserveArray(size_t); // make sure internal array has at least
                               // the requested capacity.
    void ShrinkArray();        // shrink internal storage to minimum size
    void SetLength(size_t);    // set length (<=capacity)
    char *Array();
    const char *Array() const;

    /*
    Returns:
      Total number of bytes of memory used by this class.
      (For use in ON_Object::SizeOf() overrides.
    */
    unsigned int SizeOf() const;

    ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

    // Implementation
  public:
    ~ON_String();

  protected:
    char *m_s; // pointer to ref counted string array
               // m_s - 12 bytes points at the string's ON_aStringHeader

    // implementation helpers
    struct ON_aStringHeader *Header() const;
    void CreateArray(int);
    void CopyArray();
    void CopyToArray(const ON_String &);
    void CopyToArray(int, const char *);
    void CopyToArray(int, const unsigned char *);
    void CopyToArray(int, const wchar_t *);
    void AppendToArray(const ON_String &);
    void AppendToArray(int, const char *);
    void AppendToArray(int, const unsigned char *);
    static int Length(const char *); // handles NULL pointers without crashing
    static int
    Length(const unsigned char *); // handles NULL pointers without crashing
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
// ON_wString
//

class ON_CLASS ON_wString {
  public:
    // Constructors
    ON_wString();
    ON_wString(const ON_wString &);

    ON_wString(const ON_String &src); // src = UTF-8 string

    ON_wString(const char *src); // src = nul; terminated UTF-8 string
    ON_wString(const char *src, int /*length*/); // from UTF-8 substring
    ON_wString(char, int = 1 /* repeat count */);

    ON_wString(const unsigned char *src); // src = nul; terminated UTF-8 string
    ON_wString(const unsigned char *src,
               int /*length*/); // from UTF-8 substring
    ON_wString(unsigned char, int = 1 /* repeat count */);

    ON_wString(const wchar_t *);
    ON_wString(const wchar_t *, int /*length*/); // from substring
    ON_wString(wchar_t, int = 1 /* repeat count */);

#if defined(ON_OS_WINDOWS)
    // Windows support
    bool LoadResourceString(HINSTANCE, UINT); // load from string resource
                                              // 2047 characters max
#endif

    void Create();
    void
    Destroy(); // releases any memory and initializes to default empty string
    void EmergencyDestroy();

    /*
    Description:
      Enables reference counting.  I limited cases, this is useful
      for large strings or strings that are frequently passed around.
      Reference counted strings must be carefully managed in
      when multi-threading is used.
    Parameters:
      If EnableReferenceCounting()
      is not called, then the string will not be referanceThe default is to not
    use reference counted strings.
    */
    void EnableReferenceCounting(bool bEnable);

    /*
    Returns:
      True if the string is reference counted.
    */
    bool IsReferenceCounted() const;

    // Attributes & Operations
    // as an array of characters
    int Length() const;
    bool IsEmpty() const;
    void Empty(); // sets length to zero - if possible, memory is retained

    wchar_t &operator[](int);
    wchar_t operator[](int) const;
    wchar_t GetAt(int) const;
    void SetAt(int, char);
    void SetAt(int, unsigned char);
    void SetAt(int, wchar_t);
    operator const wchar_t *() const; // as a UNICODE string

    // overloaded assignment
    const ON_wString &operator=(const ON_wString &);
    const ON_wString &operator=(const ON_String &src); // src = UTF-8 string
    const ON_wString &operator=(char);
    const ON_wString &operator=(const char *src); // src = UTF-8 string
    const ON_wString &operator=(unsigned char);
    const ON_wString &operator=(const unsigned char *src); // src = UTF-8 string
    const ON_wString &operator=(wchar_t);
    const ON_wString &operator=(const wchar_t *);

    // string concatenation
    void Append(const char *sUTF8,
                int); // append specified number of elements from a UTF-8 string
    void Append(const unsigned char *sUTF8,
                int); // append specified number of elements from a UTF-8 string
    void Append(const wchar_t *, int); // append specified number of elements
    const ON_wString &operator+=(const ON_wString &);
    const ON_wString &operator+=(const ON_String &sUTF8); // append UTF-8 string
    const ON_wString &operator+=(char);
    const ON_wString &operator+=(unsigned char);
    const ON_wString &operator+=(wchar_t);
    const ON_wString &operator+=(const char *sUTF8); // append UTF-8 string
    const ON_wString &
    operator+=(const unsigned char *sUTF8); // append UTF-8 string
    const ON_wString &operator+=(const wchar_t *);

    // operator+()
    ON_wString operator+(const ON_wString &) const;
    ON_wString
    operator+(const ON_String &sUTF8) const; // concatinate with a UTF-8 string
    ON_wString operator+(char) const;
    ON_wString operator+(unsigned char) const;
    ON_wString operator+(wchar_t) const;
    ON_wString
    operator+(const char *sUTF8) const; // concatinate with a UTF-8 string
    ON_wString operator+(
        const unsigned char *sUTF8) const; // concatinate with a UTF-8 string
    ON_wString operator+(const wchar_t *) const;

    // string comparison
    bool operator==(const ON_wString &) const;
    bool operator==(const wchar_t *) const;
    bool operator!=(const ON_wString &) const;
    bool operator!=(const wchar_t *) const;
    bool operator<(const ON_wString &) const;
    bool operator<(const wchar_t *) const;
    bool operator>(const ON_wString &) const;
    bool operator>(const wchar_t *) const;
    bool operator<=(const ON_wString &) const;
    bool operator<=(const wchar_t *) const;
    bool operator>=(const ON_wString &) const;
    bool operator>=(const wchar_t *) const;

    // string comparison
    // If this < string, returns < 0.
    // If this == string, returns 0.
    // If this < string, returns > 0.
    int Compare(const char *sUTF8) const;          // compare to UTF-8 string
    int Compare(const unsigned char *sUTF8) const; // compare to UTF-8 string
    int Compare(const wchar_t *) const;

    int CompareNoCase(const char *sUTF8) const; // compare to UTF-8 string
    int
    CompareNoCase(const unsigned char *sUTF8) const; // compare to UTF-8 string
    int CompareNoCase(const wchar_t *) const;

    // Description:
    //   Simple case sensitive wildcard matching. A question mark (?) in the
    //   pattern matches a single character.  An asterisk (*) in the pattern
    //   mathes zero or more occurances of any character.
    //
    // Parameters:
    //   pattern - [in] pattern string where ? and * are wild cards.
    //
    // Returns:
    //   true if the string mathes the wild card pattern.
    bool WildCardMatch(const wchar_t *) const;

    // Description:
    //   Simple case insensitive wildcard matching. A question mark (?) in the
    //   pattern matches a single character.  An asterisk (*) in the pattern
    //   mathes zero or more occurances of any character.
    //
    // Parameters:
    //   pattern - [in] pattern string where ? and * are wild cards.
    //
    // Returns:
    //   true if the string mathes the wild card pattern.
    bool WildCardMatchNoCase(const wchar_t *) const;

    /*
    Description:
      Replace all substrings that match token1 with token2
    Parameters:
      token1 - [in]
      token2 - [in]
    Returns:
      Number of times toke1 was replaced with token2
    */
    int Replace(const wchar_t *token1, const wchar_t *token2);
    int Replace(wchar_t token1, wchar_t token2);

    /*
    Description:
      Replaces all characters in the string whose values are
      not '0-9', 'A-Z', or 'a-z' with a percent sign followed
      by a 2 digit hex value.
    */
    void UrlEncode();

    /*
    Description:
      Replaces all %xx where xx a two digit hexadecimal number,
      with a single character. Returns false if the orginal
      string contained
    */
    bool UrlDecode();

    /*
    Description:
      Replace all white-space characters with the token.
      If token is zero, the string will end up with
      internal 0's
    Parameters:
      token - [in]
      whitespace - [in] if not null, this is a 0 terminated
        string that lists the characters considered to be
        white space.  If null, then (1,2,...,32,127) is used.
    Returns:
      Number of whitespace characters replaced.
    See Also:
      ON_wString::RemoveWhiteSpace
    */
    int ReplaceWhiteSpace(wchar_t token, const wchar_t *whitespace = 0);

    /*
    Description:
      Removes all white-space characters with the token.
    Parameters:
      whitespace - [in] if not null, this is a 0 terminated
        string that lists the characters considered to be
        white space.  If null, then (1,2,...,32,127) is used.
    Returns:
      Number of whitespace characters removed.
    See Also:
      ON_wString::ReplaceWhiteSpace
    */
    int RemoveWhiteSpace(const wchar_t *whitespace = 0);

    // simple sub-string extraction
    ON_wString Mid(int, // index of first char
                   int  // count
    ) const;
    ON_wString Mid(int // index of first char
    ) const;
    ON_wString Left(int // number of chars to keep
    ) const;
    ON_wString Right(int // number of chars to keep
    ) const;

    // upper/lower/reverse conversion
    void MakeUpper();
    void MakeLower();
    void MakeReverse();
    void TrimLeft(const wchar_t * = NULL);
    void TrimRight(const wchar_t * = NULL);
    void TrimLeftAndRight(const wchar_t * = NULL);

    /*
    Description:
      Remove all occurrences of c.
    */
    int Remove(wchar_t c);

    // searching (return starting index, or -1 if not found)
    // look for a single character match
    int Find(char) const;
    int Find(unsigned char) const;
    int Find(wchar_t) const;
    int ReverseFind(char) const;
    int ReverseFind(unsigned char) const;
    int ReverseFind(wchar_t) const;

    // look for a specific sub-string
    int Find(const char *) const;
    int Find(const unsigned char *) const;
    int Find(const wchar_t *) const;

    // simple formatting - be careful with %s in format string
    void ON_MSC_CDECL Format(const char *, ...);
    void ON_MSC_CDECL Format(const unsigned char *, ...);
    void ON_MSC_CDECL Format(const wchar_t *, ...);

    // Low level access to string contents as character array
    void ReserveArray(size_t); // make sure internal array has at least
                               // the requested capacity.
    void ShrinkArray();        // shrink internal storage to minimum size
    void SetLength(size_t);    // set length (<=capacity)
    wchar_t *Array();
    const wchar_t *Array() const;

    /*
    Returns:
      Total number of bytes of memory used by this class.
      (For use in ON_Object::SizeOf() overrides.
    */
    unsigned int SizeOf() const;

    /*
    Returns:
      CRC of the string.
    */
    ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

    /*
    Returns:
      CRC of the lower case version of the string. Useful
      for case insensitive CRCs and hash codes.
    */
    ON__UINT32 DataCRCLower(ON__UINT32 current_remainder) const;

    // Implementation
  public:
    ~ON_wString();

  protected:
    wchar_t *m_s; // pointer to ref counted string array
                  // m_s - 12 bytes points at the string's ON_wStringHeader

    // implementation helpers
    struct ON_wStringHeader *Header() const;
    void CreateArray(int);
    void CopyArray();
    void CopyToArray(const ON_wString &);
    void CopyToArray(int, const char *);
    void CopyToArray(int, const unsigned char *);
    void CopyToArray(int, const wchar_t *);
    void AppendToArray(const ON_wString &);
    void AppendToArray(int, const char *);
    void AppendToArray(int, const unsigned char *);
    void AppendToArray(int, const wchar_t *);
    static int Length(const char *); // handles NULL pointers without crashing
    static int
    Length(const unsigned char *); // handles NULL pointers without crashing
    static int
    Length(const wchar_t *); // handles NULL pointers without crashing
};

class ON_CLASS ON_UnitSystem {
  public:
    ON_UnitSystem(); // default constructor units are millimeters.
    ~ON_UnitSystem();

    ON_UnitSystem(ON::unit_system);
    ON_UnitSystem &operator=(ON::unit_system);

    bool IsValid() const;

    void Default(); // millimeters = default unit system

    bool Read(class ON_BinaryArchive &);
    bool Write(class ON_BinaryArchive &) const;
    void Dump(class ON_TextLog &) const;

    ON::unit_system m_unit_system;

    // The m_custom_unit_... settings apply when m_unit_system =
    // ON::custom_unit_system
    double m_custom_unit_scale;    // 1 meter = m_custom_unit_scale custom units
    ON_wString m_custom_unit_name; // name of custom units

    // Custom units example:
    //    1 Nautical league = 5556 meters
    //    So, if you wanted your unit system to be nautical leagues
    //    your ON_UnitSystem would be
    //      m_unit_system       = ON::custom_unit_system
    //      m_custom_unit_scale = 1.0/5556.0 = 0.0001799856...
    //      m_custom_unit_name  = L"Nautical leagues"
};

#endif
