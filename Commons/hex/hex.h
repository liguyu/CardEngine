/*
  Author:       Yiming.You
  Version:      1.0
*/

/*
     Hex编码解码模块
	 1. 将bin buffer装换成hex表示
     2. 将hex buffer装换成bin表示
*/

#ifndef __HEX_CODEC_INC
#define __HEX_CODEC_INC

#include <string>

namespace Engine
{
  namespace Codec
  {
		typedef unsigned char byte;

    /**
    * Perform hex encoding
    * @param output an array of at least input_length*2 bytes
    * @param input is some binary data
    * @param input_length length of input in bytes
    * @param uppercase should output be upper or lower case?
    */
    void hex_encode( char output[],
			               const byte input[],
                     size_t input_length,
										 bool uppercase = false );

    /**
    * Perform hex encoding
    * @param input some input
    * @param input_length length of input in bytes
    * @param uppercase should output be upper or lower case?
    * @return hexadecimal representation of input
    */
    std::string hex_encode( const byte input[],
			                      size_t input_length,
                            bool uppercase = false );

    /**
    * Perform hex decoding
    * @param output an array of at least input_length/2 bytes
    * @param input some hex input
    * @param input_length length of input in bytes
    * @param input_consumed is an output parameter which says how many
    *        bytes of input were actually consumed. If less than
    *        input_length, then the range input[consumed:length]
    *        should be passed in later along with more input.
    * @param ignore_ws ignore whitespace on input; if false, throw an
                       exception if whitespace is encountered
    * @return number of bytes written to output
    */
    size_t hex_decode( byte output[],
                       const char input[],
                       size_t input_length,
                       size_t& input_consumed,
                       bool ignore_ws = true );

    /**
    * Perform hex decoding
    * @param output an array of at least input_length/2 bytes
    * @param input some hex input
    * @param input_length length of input in bytes
    * @param ignore_ws ignore whitespace on input; if false, throw an
                       exception if whitespace is encountered
    * @return number of bytes written to output
    */
    size_t hex_decode( byte output[],
                       const char input[],
                       size_t input_length,
                       bool ignore_ws = true );

    /**
    * Perform hex decoding
    * @param output an array of at least input_length/2 bytes
    * @param input some hex input
    * @param ignore_ws ignore whitespace on input; if false, throw an
                       exception if whitespace is encountered
    * @return number of bytes written to output
    */
    size_t hex_decode( byte output[],
                       const std::string& input,
                       bool ignore_ws = true );
  }
}

#endif
