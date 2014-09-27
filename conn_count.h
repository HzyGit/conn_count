#ifndef CONN_COUNT_H
#define CONN_COUNT_H
#include <linux/list.h>

#define HASH_SIZE 255
struct conn_count_addr
{
	struct hlist_node head;
	__be32 addr;
	unsigned long count;
};

struct conn_count_hash
{
	struct hlist_head head[HASH_SIZE];
	unsigned long total;
	spinlock_t lock;
};


#endif  /// COUNN_COUNT_H
