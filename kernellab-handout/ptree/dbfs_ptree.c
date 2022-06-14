#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#define MAX 1000

MODULE_LICENSE("GPL");

static struct dentry *dir, *inputdir, *ptreedir;
static struct task_struct *ptr;

char outstr[MAX];

ssize_t totallen = 0;
ssize_t templen = 0; 

static ssize_t write_pid_to_input(struct file *fp, 
                                const char __user *user_buffer, 
                                size_t length, 
                                loff_t *position)
{
	pid_t inpid;
        char tempstr[MAX];
	totallen = 0;

	memset(outstr, 0, MAX);

	if(copy_from_user(tempstr, user_buffer, length)) {
                return -EFAULT;
        }
        sscanf(tempstr, "%u", &inpid);

	ptr = pid_task(find_get_pid(inpid), PIDTYPE_PID);
	if(!ptr) {
		return -EINVAL;
	}
	
	for( ; ptr->pid != 0; ptr = ptr->parent) {
                memset(tempstr, 0, MAX);
		templen = snprintf(tempstr, MAX, "%s (%d)\n", ptr->comm, ptr->pid);
                //memset(tempstr, 0, MAX);
		snprintf(templen+tempstr, MAX-templen, outstr);

                if(templen >= 0) {
                        totallen += templen;
                }

                strcpy(outstr, tempstr);
	}

        //sscanf(user_buffer, "%u", &input_pid);
        //curr = // Find task_struct using input_pid. Hint: pid_task

        // Tracing process tree from input_pid to init(1) process

        // Make Output Format string: process_command (process_id)

        return length;
}

static ssize_t read_output(struct file *fp, char __user *user_buffer, size_t length, loff_t *position)
{
	ssize_t copiedlen = simple_read_from_buffer(user_buffer, length, position, outstr, totallen);
	return copiedlen;
}

static const struct file_operations dbfs_fops_write = {
        .write = write_pid_to_input,
};

static const struct file_operations dbfs_fops_read = {
	.read = read_output,
};

static int __init dbfs_module_init(void)
{
        // Implement init module code
	dir = debugfs_create_dir("ptree", NULL);
	if(!dir) {
		pr_err("Cannot creat ptree dir\n");
		return -1;
	}
	/*ptreedir = debugfs_create_file("ptree", 00700, dir, NULL, &dbfs_fops_read);
        if(!ptreedir) {
                pr_err("Cannot create ptreedir file\n");
                return -1;
        }*/
	inputdir = debugfs_create_file("input", 00700, dir, NULL, &dbfs_fops_write);
	if(!inputdir) {
		pr_err("Cannot create inputdir file\n");
		return -1;
	}
	        ptreedir = debugfs_create_file("ptree", 00700, dir, NULL, &dbfs_fops_read);
        if(!ptreedir) {
                pr_err("Cannot create ptreedir file\n");
                return -1;
        }
#if 0
        dir = debugfs_create_dir("ptree", NULL);
        
        if (!dir) {
                printk("Cannot create ptree dir\n");
                return -1;
        }

        inputdir = debugfs_create_file("input", , , , );
        ptreedir = debugfs_create_("ptree", , , ); // Find suitable debugfs API
#endif
	
	printk("dbfs_ptree module initialize done\n");

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        // Implement exit module code
	debugfs_remove_recursive(dir);
	printk("dbfs_ptree module exit\n");
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
