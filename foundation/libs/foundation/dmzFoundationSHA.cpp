#include <dmzFoundationSHA.h>
#include <sha2.h>

/*!

\enum dmz::SHATypeEnum
\ingroup Foundation
\brief Specifies the SHA algorithm to use.
\sa dmz::SHA

\class dmz::SHA
\ingroup Foundation
\brief Provides Secure Hashing Algorithms (SHA).

*/

/*!

\var dmz::SHA1
\ingroup Foundation
\brief SHA-1 Algorithm.

*/

/*!

\var dmz::SHA224
\ingroup Foundation
\brief SHA-224 Algorithm.

*/

/*!

\var dmz::SHA256
\ingroup Foundation
\brief SHA-256 Algorithm.

*/

/*!

\var dmz::SHA384
\ingroup Foundation
\brief SHA-384 Algorithm.

*/

/*!

\var dmz::SHA512
\ingroup Foundation
\brief SHA-512 Algorithm.

*/

namespace {

typedef void (*sha_init_ptr) (SHA_CTX *);
typedef void (*sha_update_ptr) (SHA_CTX *, const uint8_t *, size_t);
typedef char *(*sha_end_ptr) (SHA_CTX *, char *);

};

//! \cond
struct dmz::SHA::State {

   SHATypeEnum type;
   sha_init_ptr init;
   sha_update_ptr update;
   sha_end_ptr end;
   SHA_CTX context;

   void setup (const SHATypeEnum Type) {

      type = Type;

      if (Type == SHA1) {

         init = SHA1_Init;
         update = SHA1_Update;
         end = SHA1_End;
      }
      else if (Type == SHA224) {

         init = SHA224_Init;
         update = SHA224_Update;
         end = SHA224_End;
      }
      else if (Type == SHA256) {

         init = SHA256_Init;
         update = SHA256_Update;
         end = SHA256_End;
      }
      else if (Type == SHA384) {

         init = SHA384_Init;
         update = SHA384_Update;
         end = SHA384_End;
      }
      else {

         type = SHA512;
         init = SHA512_Init;
         update = SHA512_Update;
         end = SHA512_End;
      }

      init (&context);
   }

   State (const SHATypeEnum TheType) : type (TheType) { setup (TheType); }
};
//! \endcond


//! Constructor.
dmz::SHA::SHA (const SHATypeEnum Type) : _state (*(new State (Type))) {;}


//! Destructor.
dmz::SHA::~SHA () { delete &_state; }


/*!

\brief Returns the type of the SHA.
\sa dmz::SHATypeEnum

*/
dmz::SHATypeEnum
dmz::SHA::get_type () const { return _state.type; }


/*!

\brief Initializes the SHA.
\param[in] Type Specifies the type of SHA to use.
\sa dmz::SHATypeEnum

*/
void
dmz::SHA::init (const SHATypeEnum Type) { _state.setup (Type); }


/*!

\brief Adds data to the SHA.
\param[in] Buffer Data to add to the SHA.
\param[in] Size Number of bytes of data in the \a Buffer.

*/
void
dmz::SHA::add_data (const char *Buffer, const Int32 Size){

   _state.update (&(_state.context), (const uint8_t *)Buffer, (size_t)Size);
}


/*!

\brief Finalized the SHA.
\return Returns as String with character representation of the SHA digest.

*/
dmz::String
dmz::SHA::finish () {

   String result;

   char buffer[SHA512_DIGEST_STRING_LENGTH];
   buffer[0] = '\0';

   _state.end (&(_state.context), buffer);

   result = buffer;

   init (_state.type);

   return result;
}

