#if !FD_HAS_HOSTED
#error "This target requires FD_HAS_HOSTED"
#endif

#include "fd_tls_proto.h"

#include <stdlib.h>

int
LLVMFuzzerInitialize( int  *   argc,
                      char *** argv ) {
  /* Set up shell without signal handlers */
  putenv( "FD_LOG_BACKTRACE=0" );
  fd_boot( argc, argv );
  atexit( fd_halt );
  return 0;
}

int
LLVMFuzzerTestOneInput( uchar const * data,
                        ulong         data_sz ) {

  fd_tls_record_hdr_t hdr;
  long res = fd_tls_decode_record_hdr( &hdr, data, data_sz );
  if( res<0L ) return 0;
  FD_TEST( res==4L );
  data    += 4UL;
  data_sz -= 4UL;

  uint rec_sz = fd_tls_u24_to_uint( hdr.sz );
  if( rec_sz > data_sz ) return 0;

  switch( hdr.type ) {
    case FD_TLS_RECORD_CLIENT_HELLO: {
      fd_tls_client_hello_t ch;
      fd_tls_decode_client_hello( &ch, data, rec_sz );
      break;
    }
    case FD_TLS_RECORD_SERVER_HELLO: {
      fd_tls_server_hello_t sh;
      fd_tls_decode_server_hello( &sh, data, rec_sz );
      break;
    }
    case FD_TLS_RECORD_ENCRYPTED_EXT: {
      fd_tls_enc_ext_t ee;
      fd_tls_decode_enc_ext( &ee, data, rec_sz );
      break;
    }
    case FD_TLS_RECORD_CERT_VERIFY: {
      fd_tls_cert_verify_t cv;
      fd_tls_decode_cert_verify( &cv, data, rec_sz );
      break;
    }
    case FD_TLS_RECORD_FINISHED: {
      fd_tls_finished_t fin;
      fd_tls_decode_finished( &fin, data, rec_sz );
      break;
    }
  }
  return 0;
}