#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "stub.h"

#define PAGE_SIZE 4096
#define PAGE_HEAD(x) ((unsigned long)(x) & ~(PAGE_SIZE - 1))
#define PAGE_NEXT_HEAD(x) (PAGE_HEAD(x) + PAGE_SIZE)

/*
  打桩，将原函数地址指向的内容替换为跳转到桩函数
  0xFF 4byte offset --> 0xE9 4byte offset  0xFF call 指令   0xE9 jmp 指令
  新偏移量计算：fn_stub - fn - 5  5表示1字节指令和4字节偏移
  @stub 保存原地址内容
  @fn 原函数
  @fn_stub 桩函数
 */
void stub_set(struct func_stub *stub, void *fn, void *fn_stub)
{
    stub->fn = fn;
    memcpy(stub->buf, fn, 5);

	/* 修改fn所在的页面权限 */
    if (mprotect((void *)PAGE_HEAD(fn), PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC))
    {
        perror("Couldn't mprotect 1");
        exit(errno);
    }

	/* fn可能在2个页面，修改fn所在页面的下一页权限，有可能次页没映射，很可能失败 */
	if (mprotect((void *)PAGE_NEXT_HEAD(fn), PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC))
    {
    }

    *(char *)fn = (char)0xE9;
    *(int *)((char *)fn + 1) = (char *)fn_stub - (char *)fn - 5;

    if (mprotect((void *)PAGE_HEAD(fn), PAGE_SIZE, PROT_READ | PROT_EXEC))
    {
        perror("Couldn't mprotect 2");
        exit(errno);
    }

	if (mprotect((void *)PAGE_NEXT_HEAD(fn), PAGE_SIZE, PROT_READ | PROT_EXEC))
    {
    }
}

void stub_reset(struct func_stub *stub)
{
    if (mprotect((void *)PAGE_HEAD(stub->fn), PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC))
    {
        perror("Couldn't mprotect 3");
        exit(errno);
    }
	if (mprotect((void *)PAGE_NEXT_HEAD(stub->fn), PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC))
    {
    }

    memcpy(stub->fn, stub->buf, 5);

    if (mprotect((void *)PAGE_HEAD(stub->fn), PAGE_SIZE, PROT_READ | PROT_EXEC))
    {
        perror("Couldn't mprotect 4");
        exit(errno);
    }
	if (mprotect((void *)PAGE_NEXT_HEAD(stub->fn), PAGE_SIZE, PROT_READ | PROT_EXEC))
    {
    }
}
