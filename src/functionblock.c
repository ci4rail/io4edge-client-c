
#include "io4edge_internal.h"

const char *TAG = "functionblock";

static void *read_thread(void *arg);

io4e_err_t io4edge_functionblock_client_new_from_host_port(io4edge_functionblock_client_t **handle_p,
    const char *host,
    int port,
    int cmd_timeout_seconds)
{
    io4e_err_t err;
    // allocate memory for the functionblock client
    io4edge_functionblock_client_t *h = calloc(1, sizeof(io4edge_functionblock_client_t));
    if (h == NULL) {
        return IO4E_ERR_NO_MEM;
    }

    if ((err = io4edge_transport_new(host, port, &h->transport)) != IO4E_OK)
        goto ERREXIT;
    h->cmd_timeout = cmd_timeout_seconds;
    h->cmd_context[0] = 'A';
    h->cmd_context[1] = '\0';
    sem_init(&h->cmd_sem, 0, 0);
    h->cmd_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    h->read_thread_stop = false;

    // create streamq
    if ((err = io4e_streamq_new(CONFIG_STREAMQ_CAPACITY, &h->streamq)) != IO4E_OK)
        goto ERREXIT;

    if (pthread_create(&h->read_thread_id, NULL, read_thread, h) != 0) {
        IO4E_LOGE(TAG, "Could not create read thread");
        err = IO4E_FAIL;
        goto ERREXIT;
    }
    *handle_p = h;
ERREXIT:
    if (err != IO4E_OK) {
        if (h->transport != NULL)
            h->transport->destroy(&h->transport);
        if (h->streamq != NULL)
            io4e_streamq_delete(&h->streamq);
        free(h);
    }
    return err;
}

io4e_err_t io4edge_functionblock_client_delete(io4edge_functionblock_client_t **handle_p)
{
    if (*handle_p != NULL) {
        io4edge_functionblock_client_t *h = *handle_p;

        // stop the read thread
        h->read_thread_stop = true;
        pthread_join(h->read_thread_id, NULL);
        io4e_streamq_delete(&h->streamq);

        h->transport->destroy(&h->transport);
        free(h);
        *handle_p = NULL;
    }
    return IO4E_OK;
}

static io4e_err_t map_fb_error(Functionblock__Status status)
{
    switch (status) {
    case FUNCTIONBLOCK__STATUS__OK:
        return IO4E_OK;
    case FUNCTIONBLOCK__STATUS__UNSPECIFIC_ERROR:
        return IO4E_ERR_FB_UNSPECIFIC_ERROR;
    case FUNCTIONBLOCK__STATUS__UNKNOWN_COMMAND:
        return IO4E_ERR_FB_UNKNOWN_COMMAND;
    case FUNCTIONBLOCK__STATUS__NOT_IMPLEMENTED:
        return IO4E_ERR_FB_NOT_IMPLEMENTED;
    case FUNCTIONBLOCK__STATUS__WRONG_CLIENT:
        return IO4E_ERR_FB_WRONG_CLIENT;
    case FUNCTIONBLOCK__STATUS__INVALID_PARAMETER:
        return IO4E_ERR_FB_INVALID_PARAMETER;
    case FUNCTIONBLOCK__STATUS__HW_FAULT:
        return IO4E_ERR_FB_HW_FAULT;
    case FUNCTIONBLOCK__STATUS__STREAM_ALREADY_STARTED:
        return IO4E_ERR_FB_STREAM_ALREADY_STARTED;
    case FUNCTIONBLOCK__STATUS__STREAM_ALREADY_STOPPED:
        return IO4E_ERR_FB_STREAM_ALREADY_STOPPED;
    case FUNCTIONBLOCK__STATUS__STREAM_START_FAILED:
        return IO4E_ERR_FB_STREAM_START_FAILED;
    case FUNCTIONBLOCK__STATUS__TEMPORARILY_UNAVAILABLE:
        return IO4E_ERR_FB_TEMPORARILY_UNAVAILABLE;
    default:
        return IO4E_ERR_FB_UNKNOWN;
    }
}

// caller must free the response if it is not NULL
static io4e_err_t command(io4edge_functionblock_client_t *h,
    Functionblock__Command *cmd,
    Functionblock__Response **res_p)
{
    io4e_err_t err;
    Functionblock__Context context = FUNCTIONBLOCK__CONTEXT__INIT;
    cmd->context = &context;

    if (pthread_mutex_lock(&h->cmd_mutex) != 0) {
        IO4E_LOGE(TAG, "Could not lock cmd_mutex");
        return IO4E_FAIL;
    }

    h->cmd_context[0]++;
    cmd->context->value = h->cmd_context;
    size_t marshaled_cmd_len = functionblock__command__get_packed_size(cmd);
    size_t header_size = h->transport->get_write_offset(h->transport);
    uint8_t marshaled_cmd_buffer[header_size + marshaled_cmd_len];
    functionblock__command__pack(cmd, &marshaled_cmd_buffer[header_size]);

    if (res_p)
        *res_p = NULL;
    if ((err = h->transport->write_msg(h->transport, marshaled_cmd_buffer, marshaled_cmd_len + header_size)) !=
        IO4E_OK) {
        IO4E_LOGE(TAG, "command write failed: %d", err);
        goto EXIT;
    }
    // wait using sem_timedwait for the response
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += h->cmd_timeout;
    if (sem_timedwait(&h->cmd_sem, &ts) == -1) {
        IO4E_LOGE(TAG, "command timed out");
        err = IO4E_ERR_TIMEOUT;
        goto EXIT;
    }

    Functionblock__Response *res = h->cmd_response;
    h->cmd_response = NULL;

    if (res == NULL) {
        IO4E_LOGE(TAG, "command response is NULL");
        err = IO4E_FAIL;
        goto EXIT;
    }
    // check context
    if (strcmp(res->context->value, h->cmd_context) != 0) {
        IO4E_LOGE(TAG, "command response context mismatch");
        err = IO4E_FAIL;
        goto EXIT_FREE_RES;
    }
    // advance context
    h->cmd_context[0]++;
    if (h->cmd_context[0] == 'Z')
        h->cmd_context[0] = 'A';

    // check status
    if (res->status != FUNCTIONBLOCK__STATUS__OK) {
        IO4E_LOGE(TAG,
            "command failed: %s: %s",
            functionblock__status__descriptor.values[res->status].name,
            res->error->error);
        err = map_fb_error(res->status);
        goto EXIT_FREE_RES;
    }

    if (res_p)
        *res_p = res;
    else {
        err = IO4E_OK;
        goto EXIT_FREE_RES;
    }
    goto EXIT;
EXIT_FREE_RES:
    functionblock__response__free_unpacked(res, NULL);
EXIT:
    pthread_mutex_unlock(&h->cmd_mutex);
    return err;
}

#define MAKE_ANYPB(PBANY, MARSHALED_FS_MSG)                    \
    Google__Protobuf__Any PBANY = GOOGLE__PROTOBUF__ANY__INIT; \
    PBANY.type_url = (char *)MARSHALED_FS_MSG->proto_name;     \
    PBANY.value.len = MARSHALED_FS_MSG->marshaled_msg_len;     \
    PBANY.value.data = MARSHALED_FS_MSG->marshaled_msg;

static bool is_correct_response_type_case(Functionblock__Response *res, Functionblock__Response__TypeCase type_case)
{
    if (res->type_case != type_case) {
        IO4E_LOGE(TAG, "response type case mismatch: %d != %d", res->type_case, type_case);
        return false;
    }
    return true;
}

static bool is_correct_config_response(Functionblock__Response *res,
    Functionblock__ConfigurationResponse__ActionCase action)
{
    if (!is_correct_response_type_case(res, FUNCTIONBLOCK__RESPONSE__TYPE_CONFIGURATION))
        return false;
    if (res->configuration->action_case != action) {
        IO4E_LOGE(TAG, "response action case mismatch: %d != %d", res->configuration->action_case, action);
        return false;
    }
    return true;
}

static bool is_correct_fc_response(Functionblock__Response *res,
    Functionblock__FunctionControlResponse__ActionCase action)
{
    if (!is_correct_response_type_case(res, FUNCTIONBLOCK__RESPONSE__TYPE_FUNCTION_CONTROL))
        return false;
    if (res->functioncontrol->action_case != action) {
        IO4E_LOGE(TAG, "response action case mismatch: %d != %d", res->functioncontrol->action_case, action);
        return false;
    }
    return true;
}

io4e_err_t io4e_functionblock_upload_configuration(io4edge_functionblock_client_t *h, io4e_protomsg_t *fs_msg)
{
    MAKE_ANYPB(pb_any, fs_msg);

    Functionblock__Configuration pb_config = FUNCTIONBLOCK__CONFIGURATION__INIT;
    pb_config.action_case = FUNCTIONBLOCK__CONFIGURATION__ACTION_FUNCTION_SPECIFIC_CONFIGURATION_SET;
    pb_config.functionspecificconfigurationset = &pb_any;

    Functionblock__Command pb_cmd = FUNCTIONBLOCK__COMMAND__INIT;
    pb_cmd.type_case = FUNCTIONBLOCK__COMMAND__TYPE_CONFIGURATION;
    pb_cmd.configuration = &pb_config;

    return command(h, &pb_cmd, NULL);
}

io4e_err_t io4e_functionblock_download_configuration(io4edge_functionblock_client_t *h,
    io4e_protomsg_t *fs_req,
    io4edge_unpack_t unpack,
    void **fs_res_p)
{
    io4e_err_t err;

    MAKE_ANYPB(pb_any, fs_req);

    Functionblock__Configuration pb_config = FUNCTIONBLOCK__CONFIGURATION__INIT;
    pb_config.action_case = FUNCTIONBLOCK__CONFIGURATION__ACTION_FUNCTION_SPECIFIC_CONFIGURATION_GET;
    pb_config.functionspecificconfigurationset = &pb_any;

    Functionblock__Command pb_cmd = FUNCTIONBLOCK__COMMAND__INIT;
    pb_cmd.type_case = FUNCTIONBLOCK__COMMAND__TYPE_CONFIGURATION;
    pb_cmd.configuration = &pb_config;

    Functionblock__Response *pb_res;
    if ((err = command(h, &pb_cmd, &pb_res)) != IO4E_OK)
        return err;

    if (!is_correct_config_response(
            pb_res, FUNCTIONBLOCK__CONFIGURATION_RESPONSE__ACTION_FUNCTION_SPECIFIC_CONFIGURATION_GET))
        goto EXIT_PROTO;

    // unpack
    void *fs_res = unpack(NULL,
        pb_res->configuration->functionspecificconfigurationget->value.len,
        pb_res->configuration->functionspecificconfigurationget->value.data);
    if (fs_res == NULL) {
        IO4E_LOGE(TAG, "unpack failed");
        goto EXIT_PROTO;
    }
    *fs_res_p = fs_res;

    // free command response
    functionblock__response__free_unpacked(pb_res, NULL);
    return IO4E_OK;
EXIT_PROTO:
    functionblock__response__free_unpacked(pb_res, NULL);
    return IO4E_ERR_PROTOBUF;
}

io4e_err_t io4e_functionblock_describe(io4edge_functionblock_client_t *h,
    io4e_protomsg_t *fs_req,
    io4edge_unpack_t unpack,
    void **fs_res_p)
{
    io4e_err_t err;

    MAKE_ANYPB(pb_any, fs_req);

    Functionblock__Configuration pb_config = FUNCTIONBLOCK__CONFIGURATION__INIT;
    pb_config.action_case = FUNCTIONBLOCK__CONFIGURATION__ACTION_FUNCTION_SPECIFIC_CONFIGURATION_DESCRIBE;
    pb_config.functionspecificconfigurationset = &pb_any;

    Functionblock__Command pb_cmd = FUNCTIONBLOCK__COMMAND__INIT;
    pb_cmd.type_case = FUNCTIONBLOCK__COMMAND__TYPE_CONFIGURATION;
    pb_cmd.configuration = &pb_config;

    Functionblock__Response *pb_res;
    if ((err = command(h, &pb_cmd, &pb_res)) != IO4E_OK)
        return err;

    if (!is_correct_config_response(
            pb_res, FUNCTIONBLOCK__CONFIGURATION_RESPONSE__ACTION_FUNCTION_SPECIFIC_CONFIGURATION_DESCRIBE))
        goto EXIT_PROTO;

    // unpack
    void *fs_res = unpack(NULL,
        pb_res->configuration->functionspecificconfigurationdescribe->value.len,
        pb_res->configuration->functionspecificconfigurationdescribe->value.data);
    if (fs_res == NULL) {
        IO4E_LOGE(TAG, "unpack failed");
        goto EXIT_PROTO;
    }
    *fs_res_p = fs_res;

    // free command response
    functionblock__response__free_unpacked(pb_res, NULL);
    return IO4E_OK;
EXIT_PROTO:
    functionblock__response__free_unpacked(pb_res, NULL);
    return IO4E_ERR_PROTOBUF;
}

io4e_err_t io4e_functionblock_function_control_set(io4edge_functionblock_client_t *h,
    io4e_protomsg_t *fs_req,
    io4edge_unpack_t unpack,
    void **fs_res_p)
{
    io4e_err_t err;

    MAKE_ANYPB(pb_any, fs_req);

    Functionblock__FunctionControl pb_fc = FUNCTIONBLOCK__FUNCTION_CONTROL__INIT;
    pb_fc.action_case = FUNCTIONBLOCK__FUNCTION_CONTROL__ACTION_FUNCTION_SPECIFIC_FUNCTION_CONTROL_SET;
    pb_fc.functionspecificfunctioncontrolset = &pb_any;

    Functionblock__Command pb_cmd = FUNCTIONBLOCK__COMMAND__INIT;
    pb_cmd.type_case = FUNCTIONBLOCK__COMMAND__TYPE_FUNCTION_CONTROL;
    pb_cmd.functioncontrol = &pb_fc;

    Functionblock__Response *pb_res;
    if ((err = command(h, &pb_cmd, &pb_res)) != IO4E_OK)
        return err;

    if (!is_correct_fc_response(
            pb_res, FUNCTIONBLOCK__FUNCTION_CONTROL_RESPONSE__ACTION_FUNCTION_SPECIFIC_FUNCTION_CONTROL_SET))
        goto EXIT_PROTO;

    if (fs_res_p) {
        // unpack
        void *fs_res = unpack(NULL,
            pb_res->functioncontrol->functionspecificfunctioncontrolset->value.len,
            pb_res->functioncontrol->functionspecificfunctioncontrolset->value.data);
        if (fs_res == NULL) {
            IO4E_LOGE(TAG, "unpack failed");
            goto EXIT_PROTO;
        }
        *fs_res_p = fs_res;
    }
    // free command response
    functionblock__response__free_unpacked(pb_res, NULL);
    return IO4E_OK;
EXIT_PROTO:
    functionblock__response__free_unpacked(pb_res, NULL);
    return IO4E_ERR_PROTOBUF;
}

io4e_err_t io4e_functionblock_function_control_get(io4edge_functionblock_client_t *h,
    io4e_protomsg_t *fs_req,
    io4edge_unpack_t unpack,
    void **fs_res_p)
{
    io4e_err_t err;

    MAKE_ANYPB(pb_any, fs_req);

    Functionblock__FunctionControl pb_fc = FUNCTIONBLOCK__FUNCTION_CONTROL__INIT;
    pb_fc.action_case = FUNCTIONBLOCK__FUNCTION_CONTROL__ACTION_FUNCTION_SPECIFIC_FUNCTION_CONTROL_GET;
    pb_fc.functionspecificfunctioncontrolset = &pb_any;

    Functionblock__Command pb_cmd = FUNCTIONBLOCK__COMMAND__INIT;
    pb_cmd.type_case = FUNCTIONBLOCK__COMMAND__TYPE_FUNCTION_CONTROL;
    pb_cmd.functioncontrol = &pb_fc;

    Functionblock__Response *pb_res;
    if ((err = command(h, &pb_cmd, &pb_res)) != IO4E_OK)
        return err;

    if (!is_correct_fc_response(
            pb_res, FUNCTIONBLOCK__FUNCTION_CONTROL_RESPONSE__ACTION_FUNCTION_SPECIFIC_FUNCTION_CONTROL_GET))
        goto EXIT_PROTO;

    // unpack
    void *fs_res = unpack(NULL,
        pb_res->functioncontrol->functionspecificfunctioncontrolget->value.len,
        pb_res->functioncontrol->functionspecificfunctioncontrolget->value.data);
    if (fs_res == NULL) {
        IO4E_LOGE(TAG, "unpack failed");
        goto EXIT_PROTO;
    }
    *fs_res_p = fs_res;

    // free command response
    functionblock__response__free_unpacked(pb_res, NULL);
    return IO4E_OK;
EXIT_PROTO:
    functionblock__response__free_unpacked(pb_res, NULL);
    return IO4E_ERR_PROTOBUF;
}

io4e_err_t io4e_functionblock_start_stream(io4edge_functionblock_client_t *h,
    io4e_protomsg_t *fs_config,
    Functionblock__StreamControlStart *fb_config)
{
    MAKE_ANYPB(pb_any, fs_config);
    Functionblock__StreamControl pb_sc = FUNCTIONBLOCK__STREAM_CONTROL__INIT;
    pb_sc.action_case = FUNCTIONBLOCK__STREAM_CONTROL__ACTION_START;
    pb_sc.start = fb_config;
    fb_config->functionspecificstreamcontrolstart = &pb_any;

    Functionblock__Command pb_cmd = FUNCTIONBLOCK__COMMAND__INIT;
    pb_cmd.type_case = FUNCTIONBLOCK__COMMAND__TYPE_STREAM_CONTROL;
    pb_cmd.streamcontrol = &pb_sc;

    return command(h, &pb_cmd, NULL);
}

io4e_err_t io4edge_functionblock_stop_stream(io4edge_functionblock_client_t *h)
{
    Functionblock__StreamControl pb_sc = FUNCTIONBLOCK__STREAM_CONTROL__INIT;
    pb_sc.action_case = FUNCTIONBLOCK__STREAM_CONTROL__ACTION_STOP;

    Functionblock__Command pb_cmd = FUNCTIONBLOCK__COMMAND__INIT;
    pb_cmd.type_case = FUNCTIONBLOCK__COMMAND__TYPE_STREAM_CONTROL;
    pb_cmd.streamcontrol = &pb_sc;

    return command(h, &pb_cmd, NULL);
}

io4e_err_t io4edge_functionblock_read_stream(io4edge_functionblock_client_t *h,
    io4edge_unpack_t unpack,
    Functionblock__StreamData **sd_p,
    void **fs_data_p,
    long timeout)
{
    io4e_err_t err;
    Functionblock__StreamData *sd;
    void *fs_data;

    *sd_p = NULL;
    *fs_data_p = NULL;

    if ((err = io4e_streamq_pop(h->streamq, (void **)&sd, timeout)) != IO4E_OK) {
        return err;
    }
    if (sd->functionspecificstreamdata == NULL) {
        IO4E_LOGE(TAG, "functionspecificstreamdata is NULL");
        return IO4E_ERR_PROTOBUF;
    }
    fs_data = unpack(NULL, sd->functionspecificstreamdata->value.len, sd->functionspecificstreamdata->value.data);
    if (fs_data == NULL) {
        IO4E_LOGE(TAG, "unpack failed");
        return IO4E_ERR_PROTOBUF;
    }
    *sd_p = sd;
    *fs_data_p = fs_data;
    return IO4E_OK;
}

static void *read_thread(void *arg)
{
    io4edge_functionblock_client_t *h = (io4edge_functionblock_client_t *)arg;
    io4e_err_t err;

    while (!h->read_thread_stop) {
        uint32_t res_len;
        uint8_t *res_buffer;
        if ((err = h->transport->read_msg(h->transport, &res_buffer, &res_len, 1)) != IO4E_OK) {
            if (err == IO4E_ERR_TIMEOUT)
                continue;
            IO4E_LOGE(TAG, "command read failed: %d", err);
            continue;
        }
        if (res_len == 0) {
            IO4E_LOGE(TAG, "command read failed: empty response");
            continue;
        }
        Functionblock__Response *res = functionblock__response__unpack(NULL, res_len, res_buffer);
        if (res == NULL) {
            IO4E_LOGE(TAG, "command unpack failed");
            free(res_buffer);
            continue;
        }
        free(res_buffer);

        if (res->type_case == FUNCTIONBLOCK__RESPONSE__TYPE_STREAM) {
            // got stream data
            Functionblock__StreamData *sd = res->stream;
            res->stream = NULL;  // prevent free
            // free response (but not stream data)
            functionblock__response__free_unpacked(res, NULL);

            if ((err = io4e_streamq_push(h->streamq, sd, IO4E_FOREVER)) != IO4E_OK) {
                IO4E_LOGE(TAG, "streamq push failed: %d", err);
                functionblock__response__free_unpacked(res, NULL);
                continue;
            }
        } else {
            // got command response

            if (h->cmd_response != NULL) {
                IO4E_LOGW(TAG, "command response already set");
                functionblock__response__free_unpacked(h->cmd_response, NULL);
            }

            h->cmd_response = res;
            if ((sem_post(&h->cmd_sem) != 0)) {
                free(res);
                IO4E_LOGE(TAG, "sem_post failed");
                goto EXIT_THREAD;
            }
        }
    }
EXIT_THREAD:
    IO4E_LOGD(TAG, "read thread exit");
    return NULL;
}
