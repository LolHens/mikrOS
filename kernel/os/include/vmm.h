#ifndef VMM_H
#define VMM_H

#include "stdint.h"
#include "multiboot.h"
#include "pmm.h"
#include "console.h"

#define PD_PRESENT (1 << 0)
#define PD_WRITE   (1 << 1)
#define PD_PUBLIC  (1 << 2)
#define PD_WTCACHE (1 << 3)
#define PD_NOCACHE (1 << 4)
#define PD_USED    (1 << 5)
#define PD_USER0   (1 << 9)
#define PD_USER1   (1 << 10)
#define PD_USER2   (1 << 11)

#define PT_PRESENT (1 << 0)
#define PT_WRITE   (1 << 1)
#define PT_PUBLIC  (1 << 2)
#define PT_WTCACHE (1 << 3)
#define PT_NOCACHE (1 << 4)
#define PT_USED    (1 << 5)
#define PT_WRITTEN (1 << 6)
#define PT_USER0   (1 << 9)
#define PT_USER1   (1 << 10)
#define PT_USER2   (1 << 11)

#define PT_ALLOCATABLE PT_USER0

#define ACTIVE_PAGETBL_VADDR 0x200000
#define ACTIVE_CONTEXT_VADDR (ACTIVE_PAGETBL_VADDR + 0x100000)

#define STATIC_ALLOC_VADDR 0x1900000
#define USERSPACE_BOTTOM  0x10000000

struct vmm_context {
    uint32_t* pagedir;
};

uint32_t vmm_init(struct multiboot_info* mb_info);
uint32_t vmm_create_pagedir(struct multiboot_info* mb_info);
void     vmm_activate_pagedir(uint32_t context);
void     vmm_free(void* vaddr);
void     vmm_unmap(void* vaddr);
void*    vmm_alloc(uint32_t* retpaddr);
void*    vmm_alloc_addr(void* vaddr, uint32_t* paddr);
void*    vmm_alloc_static(uint32_t vaddr, uint32_t flags);
uint32_t vmm_get_current_pagedir(void);
void     map_address_context(uint32_t* pagedir, uint32_t vaddr, uint32_t paddr, uint32_t flags);
void     map_address_active(uint32_t vaddr, uint32_t paddr, uint32_t flags);  


#endif