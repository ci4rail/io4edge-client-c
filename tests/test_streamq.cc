#include <unistd.h>
#include <ctime>
#include "gtest/gtest.h"

extern "C" {
#include "io4edge_internal.h"
}

TEST(StreamQ, InitialEmpty)
{
    streamq_t *q;
    io4e_err_t err = io4e_streamq_new(5, &q);
    EXPECT_EQ(err, IO4E_OK);
    ASSERT_TRUE(q != nullptr);
    EXPECT_EQ(io4e_streamq_entries(q), 0);
}

TEST(StreamQ, PushPop)
{
    streamq_t *q;
    io4e_err_t err = io4e_streamq_new(5, &q);

    for (uint64_t i = 0; i < 5; i++) {
        err = io4e_streamq_push(q, (void *)i, 0);
        EXPECT_EQ(err, IO4E_OK);
    }
    EXPECT_EQ(io4e_streamq_entries(q), 5);

    void *p;
    for (uint64_t i = 0; i < 5; i++) {
        err = io4e_streamq_pop(q, &p, 0);
        EXPECT_EQ(err, IO4E_OK);
        EXPECT_EQ(p, (void *)i);
    }

    // Test q overrun
    for (uint64_t i = 0; i < 5; i++) {
        err = io4e_streamq_push(q, (void *)i, 0);
        EXPECT_EQ(err, IO4E_OK);
    }
    err = io4e_streamq_push(q, (void *)6LL, 0);
    EXPECT_EQ(err, IO4E_ERR_TIMEOUT);
    EXPECT_EQ(io4e_streamq_entries(q), 5);
    for (uint64_t i = 0; i < 5; i++) {
        err = io4e_streamq_pop(q, &p, 0);
        EXPECT_EQ(err, IO4E_OK);
        EXPECT_EQ(p, (void *)i);
    }
    err = io4e_streamq_pop(q, &p, 0);
    EXPECT_EQ(err, IO4E_ERR_TIMEOUT);
}

static void *slow_consumer_thread(void *arg)
{
    streamq_t *q = (streamq_t *)arg;
    io4e_err_t err;
    struct timespec ts;

    void *p;
    for (uint64_t i = 0; i < 100; i++) {
        err = io4e_streamq_pop(q, &p, 1);
        EXPECT_EQ(err, IO4E_OK);
        EXPECT_EQ(p, (void *)i);
        usleep(10000);
    }
    return NULL;
}

TEST(StreamQ, MultiThreadSlowConsumer)
{
    streamq_t *q;
    io4e_err_t err = io4e_streamq_new(5, &q);

    pthread_t thread;
    pthread_create(&thread, NULL, slow_consumer_thread, q);

    for (uint64_t i = 0; i < 100; i++) {
        err = io4e_streamq_push(q, (void *)i, 5);
        EXPECT_EQ(err, IO4E_OK);
    }
    pthread_join(thread, NULL);
}

static void *fast_consumer_thread(void *arg)
{
    streamq_t *q = (streamq_t *)arg;
    io4e_err_t err;
    struct timespec ts;

    void *p;
    for (uint64_t i = 0; i < 100; i++) {
        err = io4e_streamq_pop(q, &p, 2);
        EXPECT_EQ(err, IO4E_OK);
        EXPECT_EQ(p, (void *)i);
    }
    return NULL;
}

TEST(StreamQ, MultiThreadFastConsumer)
{
    streamq_t *q;
    io4e_err_t err = io4e_streamq_new(5, &q);

    pthread_t thread;
    pthread_create(&thread, NULL, fast_consumer_thread, q);

    for (uint64_t i = 0; i < 100; i++) {
        err = io4e_streamq_push(q, (void *)i, 1);
        usleep(10000);
        EXPECT_EQ(err, IO4E_OK);
    }
    pthread_join(thread, NULL);
}
