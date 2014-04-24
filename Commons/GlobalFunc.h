/*
  公用操作方法
  Author:       yiming.you
	Version:      1.0
	Create Date:  2012-04-24
	Review Date:
*/

#ifndef GLOBALFUNC_H_
#define GLOBALFUNC_H_
#include "../Commons/targetver.h"
#include "./Des/des.h"

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <assert.h>
#include <locale.h>

namespace Engine
{
  namespace Func
  {
    template<class T, int N>
    inline T get_n_byte_int( unsigned char *value_str, int len )
    {
      assert( N <= len );

      T res = 0;

      for ( int i = 0; i < N; i++  )
      {
        res |= value_str[i] << ( N - i - 1 ) * sizeof( unsigned char ) * 8;
      }

      return res;
    }

    template<class T, int N>
    void set_n_byte_int( unsigned char *value_str, int len, T num )
    {
      assert( N <= len );

      for ( int i = 0; i < N; i++  )
      {
        value_str[i] = ( num >> ( ( N - i - 1 ) * sizeof( unsigned char ) * 8 ) ) & 0xFF;
      }
    }

    template<class T, int N>
    void set_n_byte_lt_int( unsigned char *value_str, int len, T num )
    {
      assert( N <= len );
      T tmp = num;
      for ( int i = 0; i < N; i++ )
      {
        value_str[i] = tmp % 256;
        tmp = tmp / 256;
      }
    }

    template<class T, int N>
    inline T get_n_byte_lt_int( unsigned char *value_str, int len )
    {
      assert( N <= len );

      T res = value_str[0];
      T tmp = 256;
      for ( int i = 1; i < N; i++  )
      {
        res += value_str[i] * tmp;
        tmp = tmp * 256;
      }

      return res;
    }

    inline
    void hex2dec( const char *sHexStr, int iHexLen, unsigned char *uDecBuf, int& iDecBUfLen )
    {
      int i = 0;
      unsigned long ul;
      char sHexTmp[3];
      int offset = 0;
      int dlen = iHexLen / 2;

      memset( sHexTmp, 0, sizeof( sHexTmp ) );
      if ( iHexLen % 2 )
      {
        sHexTmp[0] = '0';
        sHexTmp[1] = sHexStr[0];
        ul = strtoul( sHexTmp, NULL, 16 );
        uDecBuf[0] = ( unsigned char )ul;
        offset++;
      }
      for ( i = 0; i < dlen; i++ )
      {
        memcpy( sHexTmp, &sHexStr[2 * i + offset], 2 );
        ul = strtoul( sHexTmp, NULL, 16 );
        uDecBuf[i + offset] = ( unsigned char )ul;
      }
      iDecBUfLen = i + offset;
    }

    inline
    void dec2hex( const unsigned char *uDecBuf, int iDecLen, char *sHexStr )
    {
      int i = 0;
      int k = 0;
      for ( i = 0; i < iDecLen; i++ )
      {
        k = uDecBuf[i];
        sprintf( &sHexStr[2 * i], "%02X", k );
      }
    }

    //十进制asc码字符串转换为bcd保存的数组
    inline
    void asc2bcd( const char *sAscStr, int iAscLen, unsigned char *uBcdBuf, int& uBcdBufLen )
    {
      int i = 0;
      int offset = 0;
      int len = iAscLen / 2;
      unsigned char lch, hch;
      if ( iAscLen % 2 )
      {
        lch = 0;
        hch = sAscStr[0] - '0';
        hch = hch & 0x0F;
        uBcdBuf[0] = hch | lch;
        offset++;
      }
      for ( i = 0; i < len; i++ )
      {
        lch = sAscStr[2 * i + offset] - '0';
        hch = sAscStr[2 * i + offset + 1] - '0';
        lch = lch << 4;
        hch = hch & 0x0F;
        lch = lch & 0xF0;
        uBcdBuf[i + offset] = hch | lch;
      }
      uBcdBufLen = i + offset;
    }

    inline
    void bcd2asc( const unsigned char *uBcdBuf, int iBcdLen, char *sAscStr )
    {
      int i;
      unsigned char lch, hch;

      for ( i = 0; i < iBcdLen; i++ )
      {
        hch = ( uBcdBuf[i] & 0x0F );
        lch = ( uBcdBuf[i] & 0xF0 );
        lch = lch >> 4;
        sAscStr[2 * i] = lch + '0';
        sAscStr[2 * i + 1] = hch + '0';
      }
    }

    inline
    void GetCurrentDatetime( char *dateStr, char *timeStr )
    {
      time_t curr;
      struct tm curr_tm;
      time( &curr );
      curr_tm = *localtime( &curr );
      sprintf( dateStr, "%.04d%.02d%.02d", curr_tm.tm_year + 1900
               , curr_tm.tm_mon + 1, curr_tm.tm_mday );

      sprintf( timeStr, "%.02d%.02d%.02d", curr_tm.tm_hour
               , curr_tm.tm_min, curr_tm.tm_sec );

    }

    inline
    void GetCurrentDate( char *dateStr )
    {
      time_t curr;
      struct tm curr_tm;
      time( &curr );
      curr_tm = *localtime( &curr );
      sprintf( dateStr, "%.04d%.02d%.02d", curr_tm.tm_year + 1900
               , curr_tm.tm_mon + 1, curr_tm.tm_mday );
    }

    inline
    int EncodeUCS2( unsigned char *SourceBuf, unsigned char *DestBuf )
    {
      int len, i, j = 0;
      wchar_t wcbuf[255];

      setlocale( LC_ALL, "" );
      len = mbstowcs( wcbuf, ( const char* )SourceBuf, 255 ); /* convert mutibytes string to wide charater string */
      for ( i = 0; i < len; i++ )
      {
        DestBuf[j++] = wcbuf[i] >> 8;   /* height byte */
        DestBuf[j++] = wcbuf[i] & 0xff; /* low byte */
      }
      return len * 2;
    }

    inline
    int DecodeUCS2( unsigned char *SourceBuf, unsigned char *DestBuf, int len )
    {
      wchar_t wcbuf[255];
      int i;

      setlocale( LC_ALL, "" );
      for ( i = 0; i < len / 2; i++ )
      {
        wcbuf[i] = SourceBuf[2 * i]; // height byte
        wcbuf[i] = ( wcbuf[i] << 8 ) + SourceBuf[2 * i + 1]; // low byte
      }
      return wcstombs( ( char* )DestBuf, wcbuf, len ); /* convert wide charater string to mutibytes string */
    }

    inline
    int EncryptDes( unsigned char* key, int keyLen, unsigned char *input, int dataLen, unsigned char *output )
    {
      assert( keyLen == 8 );
      assert( dataLen == 8 );
      des_context ctx;
      des_set_key( &ctx, key );
      des_encrypt( &ctx, input, output );
      return 0;
    }

    inline
    int EncryptOperPasswd( const char *operCode, const char *password, char *encryptedPassword )
    {
      unsigned char key[8];
      unsigned char plaintext[8];
      unsigned char ciphertext[8];
      int i, j;
      memset( key, 0, sizeof key );
      memcpy( key, operCode, strlen( operCode ) );
      memset( plaintext, 0, sizeof plaintext );
      memcpy( plaintext, password, strlen( password ) );
      EncryptDes( key, 8, plaintext, 8, ciphertext );
      for ( i = 0, j = 0; i < 8; ++i )
        j += sprintf( encryptedPassword + j, "%02X", ciphertext[i] );
      return 0;
    }

    inline
    size_t CpuStr2Hex( const char *str, size_t len, uint8 *hex )
    {
      char temp[3];
      size_t i;
      memset( temp, 0, sizeof temp );
      for ( i = 0; i < len; i += 2 )
      {
        memcpy( temp, str + i, 2 );
        hex[i / 2] = ( uint8 )strtoul( temp, NULL, 16 );
      }
      return i / 2;
    }

	inline
	std::string ltohex( long l )
    {
      char a[20];
      if ( l < 256 )
        sprintf( a, "%02X", ( int )l );
      else
        sprintf( a, "%X", ( int )l );
      return std::string( a );
    }

	inline
	void string_replace( std::string & strBig, const std::string & strsrc, const std::string &strdst )
    {
      std::string::size_type pos = 0;
      std::string::size_type srclen = strsrc.size();
      std::string::size_type dstlen = strdst.size();
      while ( ( pos = strBig.find( strsrc, pos ) ) != std::string::npos )
      {
        strBig.replace( pos, srclen, strdst );
        pos += dstlen;
      }
    }

	inline
	std::string lpad( std::string& str, int padlen, unsigned char padchar )
    {
      int iLen = padlen - str.length();
      if ( iLen <= 0 )
        return str;
      str.insert( 0, iLen, padchar );
      return str;
    }

	inline
	char* lpad( char *str, int padlen, unsigned char padchar )
    {
      int iLen = padlen - strlen( str );
      if ( iLen <= 0 )
        return str;
      memmove( str + iLen, str, strlen( str ) );
      memset( str, padchar, iLen );
      return str;
    }

  } //namespace Func
} //namespace Engine

#endif //GLOBALFUNC_H_
