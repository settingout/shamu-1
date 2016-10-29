#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/setup.h>

static char new_command_line[COMMAND_LINE_SIZE];

// Function borrowed from 
// http://www.linuxquestions.org/questions/programming-9/replace-a-substring-with-another-string-in-c-170076/#post877511

char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}


static int cmdline_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", new_command_line);
	return 0;
}

static int cmdline_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cmdline_proc_show, NULL);
}

static const struct file_operations cmdline_proc_fops = {
	.open		= cmdline_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init proc_cmdline_init(void)
{
	char *cmd = new_command_line;

	strcpy(cmd, saved_command_line);
	
	// Try to spoof some cmdline values here.
	strcpy(cmd, replace_str(cmd, "androidboot.flash.locked=0", "androidboot.flash.locked=1"));
	strcpy(cmd, replace_str(cmd, "androidboot.write_protect=0", "androidboot.write_protect=1"));
	strcpy(cmd, replace_str(cmd, "androidboot.bl_state=2", "androidboot.bl_state=1"));

	proc_create("cmdline", 0, NULL, &cmdline_proc_fops);
	return 0;
}
module_init(proc_cmdline_init);
