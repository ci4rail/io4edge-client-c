#include "io4edge_client.h"
#include <stdio.h>
#include "binaryIoTypeA/protobuf-c/binaryIoTypeA/v1alpha1/binaryIoTypeA.pb-c.h"
#include "io4edge/protobuf-c/functionblock/v1alpha1/io4edge_functionblock.pb-c.h"
#include "io4edge/protobuf-c/functionblock/v1alpha1/google/protobuf/any.pb-c.h"
#include <stdlib.h>

int main(void)
{
    Functionblock__Response res = FUNCTIONBLOCK__RESPONSE__INIT;
    Functionblock__StreamData stream = FUNCTIONBLOCK__STREAM_DATA__INIT;
    Functionblock__Context context = FUNCTIONBLOCK__CONTEXT__INIT;
    context.value = "test";
    res.type_case = FUNCTIONBLOCK__RESPONSE__TYPE_STREAM;
    res.stream = &stream;
    res.context = &context;

    Google__Protobuf__Any any = GOOGLE__PROTOBUF__ANY__INIT;
    BinaryIoTypeA__StreamData data = BINARY_IO_TYPE_A__STREAM_DATA__INIT;
    data.n_samples = 500;
    data.samples = malloc(sizeof(void *) * data.n_samples);
    BinaryIoTypeA__Sample sample = BINARY_IO_TYPE_A__SAMPLE__INIT;
    sample.value = 100;

    for (int i = 0; i < data.n_samples; i++) {
        data.samples[i] = &sample;
    }

    uint8_t databuf[binary_io_type_a__stream_data__get_packed_size(&data)];
    binary_io_type_a__stream_data__pack(&data, databuf);

    any.value.data = databuf;
    any.value.len = sizeof(databuf);
    res.stream->functionspecificstreamdata = &any;

    uint8_t buf[functionblock__response__get_packed_size(&res)];
    functionblock__response__pack(&res, buf);

    Functionblock__Response *res2;
    res2 = functionblock__response__unpack(NULL, sizeof(buf), buf);
    if (res2 == NULL) {
        printf("unpack failed\n");
        return 1;
    }
    Functionblock__StreamData *sd2 = res2->stream;
    res2->stream = NULL;

    functionblock__response__free_unpacked(res2, NULL);
    printf("sd2->functionspecificstreamdata->value.len = %ld\n", sd2->functionspecificstreamdata->value.len);

    functionblock__stream_data__free_unpacked(sd2, NULL);
    return 0;
}
