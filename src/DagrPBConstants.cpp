#include "DagrPBConstants.h"


bool pb_decode_from_bytes(const uint8_t *srcbuf, size_t srcbufsize, const pb_msgdesc_t *fields, void *dest_struct)
{
    pb_istream_t stream = pb_istream_from_buffer(srcbuf, srcbufsize);
    if (!pb_decode(&stream, fields, dest_struct)) {
        Serial.printf("Error: can't decode protobuf %s, pb_msgdesc 0x%p\n", PB_GET_ERROR(&stream), fields);
        return false;
    } else {
        return true;
    }
}

size_t pb_encode_to_bytes(uint8_t *destbuf, size_t destbufsize, const pb_msgdesc_t *fields, const void *src_struct)
{

    pb_ostream_t stream = pb_ostream_from_buffer(destbuf, destbufsize);
    if (!pb_encode(&stream, fields, src_struct)) {
        Serial.printf("Error: can't encode protobuf %s\n", PB_GET_ERROR(&stream));
        assert(0); // FIXME - panic
    } else {
        return stream.bytes_written;
    }
}