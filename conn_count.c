#include <linux/init.h>
#include <linux/module.h>
#include "conn_count.h"

#define VERSION "0.1"
#define NAME "conn_count"
#define TIME_UNIT 5*HZ

static struct timer_list timer;       ///< 定时器,定时打印链接数信息
static bool end=false;                ///< 标识释放取消定时器
static struct conn_count_hash hash;

/// @brief 计算ip地址哈希
static unsigned int get_addr_hash(__be32 addr)
{
	return 0;
}

/// @brief 打印连接数哈希结构,被定时器函数定时调用
/// @note 读操作,暂不加锁
static void print_hash(const struct conn_count_hash *hash)
{
	if(NULL==hash)
	{
		printk(KERN_INFO "hash:null");
		return;
	}
	printk(KERN_INFO "total:%lu\n",hash->total);
}


/// @brief 初始化哈希结构
static void init_hash(struct conn_count_hash *hash)
{
	int i=0;

	if(NULL==hash)
		return;
	hash->total=0;
	spin_lock_init(&hash->lock);
	for(i=0;i<HASH_SIZE;i++)
		INIT_HLIST_HEAD(hash->head+i);
}

/// @brief 定时器函数
static void timer_func(unsigned long data)
{
	if(true==end)
		return;
	print_hash((const struct conn_count_hash*)data);
	mod_timer(&timer,jiffies+TIME_UNIT);
}


/// @brief 设置定时器
void set_timer(struct timer_list *timer)
{
	if(NULL==timer)
		return;
	init_timer(timer);
	setup_timer(timer,timer_func,(unsigned long)&hash);
	timer->expires=jiffies+TIME_UNIT;
	add_timer(timer);
}

/// @brief 释放连接数哈希接结构的内存
static void free_hash(struct conn_count_hash *hash)
{
	spin_lock_bh(&hash->lock);
	spin_unlock_bh(&hash->lock);
}

/// @brief 增加连接数
void add_conn_count(__be32 addr)
{
	spin_lock_bh(&hash.lock);
	hash.total++;
	/// 额外处理
	spin_unlock_bh(&hash.lock);
}
EXPORT_SYMBOL(add_conn_count);

/// @breif 减少连接数
void dec_conn_count(__be32 addr)
{
	spin_lock_bh(&hash.lock);
	hash.total--;
	/// 额外处理
	spin_unlock_bh(&hash.lock);
}
EXPORT_SYMBOL(dec_conn_count);

int __init conn_count_init(void)
{
	init_hash(&hash);
	set_timer(&timer);
	printk(KERN_INFO "load %s %s ok\n",NAME,VERSION);
	return 0;
};

void __exit conn_count_exit(void)
{
	end=true;
	del_timer(&timer);
	free_hash(&hash);
	printk(KERN_INFO "unload %s %s ok\n",NAME,VERSION);
}

module_init(conn_count_init);
module_exit(conn_count_exit);

