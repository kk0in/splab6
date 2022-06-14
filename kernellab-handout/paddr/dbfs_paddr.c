#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>
#include <linux/pgtable.h>

#define MAX 100

MODULE_LICENSE("GPL");

static struct dentry *dir, *output;
static struct task_struct *task;

long power(long base, unsigned int exp) {
	long result = 1;
	while(exp) {
		if(exp%2 == 1) {
			result *= base;
		}
		base *= base;
		exp /= 2;
	}
	return result;
}

static ssize_t read_output(struct file *fp,
                        char __user *user_buffer,
                        size_t length,
                        loff_t *position)
{
        // Implement read file operation
	struct mm_struct *mm;
	pgd_t *pgdp;
	p4d_t *p4dp;
	pud_t *pudp;
	pmd_t *pmdp;
	pte_t *ptep;
	phys_addr_t pfn;

	pid_t input_pid;
	long int vaddr = 0;

	int i;
	ssize_t copiedlen;
	unsigned char kernelbuffer[MAX];
	unsigned char outbuf[MAX];
	int outputlen = (length<MAX) ? length : MAX;

	memset(kernelbuffer, 0, MAX);
	memset(outbuf, 0, MAX);

	if(copy_from_user(kernelbuffer, user_buffer, outputlen)) {
		return -EFAULT;
	}

	input_pid = (int)(kernelbuffer[1])*16*16 + (int)(kernelbuffer[0]);
	for(i=0; i<6; i++) {
		vaddr += (long)(kernelbuffer[8+i])*power(16*16, i);
	}


	task = pid_task(find_get_pid(input_pid), PIDTYPE_PID);
	if(!task) {
		return -EINVAL;
	}

	mm = task->mm;

	pgdp = pgd_offset(mm, vaddr);
	if(pgd_none(*pgdp) || pgd_bad(*pgdp)) {
		return -EINVAL;
	}
	p4dp = p4d_offset(pgdp, vaddr);
        if(p4d_none(*p4dp) || p4d_bad(*p4dp)) {
                return -EINVAL;
        }
	pudp = pud_offset(p4dp, vaddr);
        if(pud_none(*pudp) || pud_bad(*pudp)) {
                return -EINVAL;
        }
	pmdp = pmd_offset(pudp, vaddr);
        if(pmd_none(*pmdp) || pmd_bad(*pmdp)) {
                return -EINVAL;
        }
	ptep = pte_offset_kernel(pmdp, vaddr);
        if(pte_none(*ptep) || !pte_present(*ptep)) {
                return -EINVAL;
        }

	pfn = pte_pfn(*ptep);

	for(i=0; i<outputlen; i++) {
		if(i<16) {
			outbuf[i] = kernelbuffer[i];
		}
		else if(i<17) {
			outbuf[i] = kernelbuffer[i-8];
		}
		else if(i<18) {
			outbuf[i] = (kernelbuffer[i-8])%16;
			outbuf[i] += (pfn%16)*16;
			pfn /= 16;
		}
		else {
			outbuf[i] = (pfn%16);
			pfn /= 16;
			outbuf[i] += (pfn%16)*16;
			pfn /= 16;
		}
	}
	outbuf[outputlen] = '\0';
	copiedlen = simple_read_from_buffer(user_buffer, length, position, outbuf, outputlen);
	return copiedlen;
}

static const struct file_operations dbfs_fops = {
        // Mapping file operations with your functions
	.read = read_output,
};

static int __init dbfs_module_init(void)
{
        // Implement init module
        dir = debugfs_create_dir("paddr", NULL);
        if(!dir) {
                printk(KERN_ERR "Cannot creat paddr dir\n");
                return -1;
        }
        output = debugfs_create_file("output", 00700, dir, NULL, &dbfs_fops);
        if(!output) {
                printk(KERN_ERR "Cannot create output file\n");
                return -1;
        }
        
#if 0
        dir = debugfs_create_dir("paddr", NULL);

        if (!dir) {
                printk("Cannot create paddr dir\n");
                return -1;
        }

        // Fill in the arguments below
        output = debugfs_create_file("output", , , , );
#endif

	printk("dbfs_paddr module initialize done\n");

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        // Implement exit module
	debugfs_remove_recursive(dir);
	printk("dbfs_paddr module exit\n");
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
