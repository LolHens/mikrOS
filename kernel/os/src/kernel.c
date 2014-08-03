#include "kernel.h"
#include "vfs.h"
#include "ramfs/fifo.h"

struct cpu_state* syscall(struct cpu_state* cpu) {
	switch (cpu->eax) {
	case 1: /* exit */
		cpu = terminate_current(cpu);
		break;

	case 2: /* fork */
	{
		save_cpu_state(cpu);

		uint32_t forkpdir = vmm_fork_current();

		struct task* ntask = init_task(forkpdir, 0);
		fork_task_state(ntask);

		cpu->eax = ntask->PID;
	}
		break;

	case 201: /* putc */
		cpu->eax = kprintf("%c", cpu->ebx);
		break;
	case 202: /* puts */
		cpu->eax = kprintf("%s", cpu->ebx);
		break;
	case 203: /* vmm_alloc_ucont */
		cpu->eax = (uint32_t) vmm_alloc_ucont(cpu->ebx);
		break;
	case 204: /* vmm_free */
		cpu->eax = 0;
		if (cpu->ebx >= PROGRAM_BOTTOM) { //Only in PROGRAM AREA ;)
			vmm_free((void*) cpu->ebx);
		}
		break;
	case 205: /* pmm_print_stats */
		pmm_print_stats();
		break;
	default:
		kprintf("Invalid Syscall %d...", cpu->eax);
		break;
	}

	return cpu;
}

void kernel_main(struct multiboot_info* mb_info) {
	uint32_t kernel_init_pdir = vmm_init();

    kprintf("Initializing vfs...\n");

    vfs_init_root();
    ramfs_fifo_init();

    uint32_t size = 4096;

    vfs_create_kfile("/dev/ktty", ramfs_fifo_driver_struct(), &size);
    vfs_create_dir("/dev/drives");
    vfs_create_dir("/dev/ports");
    vfs_create_dir("/dev/cpu");
    vfs_create_dir("/dev/memory");
    vfs_create_dir("/dev/audio");

    vfs_debug_ls("/dev");

	while(1);

	map_address_active((uint32_t) mb_info, (uint32_t) mb_info, 0);
	map_address_active((uint32_t) mb_info->mi_mods_addr,
			(uint32_t) mb_info->mi_mods_addr, 0);

	if (mb_info->mi_flags & MULTIBOOT_INFO_HAS_MODS) {
		for (uint32_t i = 0; i < mb_info->mi_mods_count; i++) {
			kprintf("Loading mod at %x \n", mb_info->mi_mods_addr[i].start);

			uint32_t elf_mod_pdir = vmm_create_pagedir();
			void* elf_mod_entry = 0;

			vmm_activate_pagedir(elf_mod_pdir);

			vmm_map_range(mb_info->mi_mods_addr[i].start,
					mb_info->mi_mods_addr[i].start,
					mb_info->mi_mods_addr[i].end
							- mb_info->mi_mods_addr[i].start, 0);

			struct elf_header* header = mb_info->mi_mods_addr[i].start;
			struct elf_program_header* ph;

			/* Ist es ueberhaupt eine ELF-Datei? */
			if (header->magic != ELF_MAGIC) {
				kprintf("Invalid ELF-Magic!\n");
				continue;
			}

			elf_mod_entry = (void*) (header->entry);

			ph = (struct elf_program_header*) (((char*) header)
					+ header->ph_offset);
			for (uint32_t n = 0; n < header->ph_entry_count; n++, ph++) {
				void* dest = (void*) ph->virt_addr;
				void* src = ((char*) header) + ph->offset;

				/* Nur Program Header vom Typ LOAD laden */
				if (ph->type != 1) {
					continue;
				}

				for (uint32_t offset = 0; offset < ph->mem_size; offset += 0x1000) {
					vmm_alloc_addr(dest + offset, 0);
				}

				memcpy(dest, src, ph->file_size);
			}

			vmm_activate_pagedir(kernel_init_pdir);

			init_task(elf_mod_pdir, elf_mod_entry);
		}

		enableScheduling();
	} else {
		kprintf("No Modules loadable. Microkernel shutting down.\nThank you for using this pointless version of mikrOS\n");
	}

	while (1) {
	}
}
