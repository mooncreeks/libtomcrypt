/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 */

#include "tomcrypt.h"

#ifdef LTC_MECC

int ecc_set_key(const unsigned char *in, unsigned long inlen, int type, ecc_key *key)
{
   int err;
   void *prime = key->dp.prime;
   void *a     = key->dp.A;
   void *b     = key->dp.B;

   if (type == PK_PRIVATE && inlen <= (unsigned long)key->dp.size) {
      /* load private key */
      if ((err = mp_read_unsigned_bin(key->k, (unsigned char *)in, inlen)) != CRYPT_OK) {
         goto error;
      }
      if (mp_iszero(key->k)) {
         err = CRYPT_INVALID_PACKET;
         goto error;
      }
      /* compute public key */
      if ((err = ltc_mp.ecc_ptmul(key->k, &key->dp.base, &key->pubkey, a, prime, 1)) != CRYPT_OK)         { goto error; }
      key->type = type;
   }
   else if (type == PK_PUBLIC) {
      /* load public key */
      if ((err = ltc_ecc_import_point(in, inlen, prime, a, b, key->pubkey.x, key->pubkey.y)) != CRYPT_OK) { goto error; }
      if ((err = mp_set(key->pubkey.z, 1)) != CRYPT_OK)                                                   { goto error; }
      key->type = type;
   }

   /* point on the curve + other checks */
   if ((err = ltc_ecc_verify_key(key)) != CRYPT_OK)                                                       { goto error; }

   return CRYPT_OK;

error:
   ecc_free(key);
   return err;
}

#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */