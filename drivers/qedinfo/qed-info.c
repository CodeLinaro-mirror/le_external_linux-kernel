#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/string.h>

/* Meta Information */
MODULE_LICENSE("GPL-2.0");
MODULE_AUTHOR("Lantronix <nsverdlenko@lantronix.com>");
MODULE_DESCRIPTION("QED info sysfs");
MODULE_VERSION("1.01");

static struct kobject *qedfs_kobj;

#define MAX_QEDSERIAL_LEN 24
static char qed_serialno[MAX_QEDSERIAL_LEN];

static int __init get_qed_serialno(char *str)
{
        size_t len = strlen(str);
        if (len>(MAX_QEDSERIAL_LEN-1)) len = MAX_QEDSERIAL_LEN-1;
        strncpy(qed_serialno, str, len);
        qed_serialno[len]=0;
        return 1;
}

__setup ("qed.serialno=", get_qed_serialno);


static ssize_t qedsn_show(struct kobject *kobj, struct kobj_attribute *attr, char *buffer)
{
        return sprintf(buffer, qed_serialno);
}


static struct kobj_attribute qedfs_attr = __ATTR(qed_serialno, 0444, qedsn_show, 0);

static int __init qedfs_init(void)
{
        pr_notice("qedfs-driver - Creating /sys/kernel/qedfs/qed_serialno\n");

        /* Creating the folder qedfs */
        qedfs_kobj = kobject_create_and_add("qedfs", kernel_kobj);
        if(!qedfs_kobj) {
                pr_err("qedfs-driver - Error creating /sys/kernel/qedfs\n");
                return -ENOMEM;
        }

        /* Create the sysfs file qed_serialno */
        if(sysfs_create_file(qedfs_kobj, &qedfs_attr.attr)) {
                pr_err("qedfs-driver - Error creating /sys/kernel/qedfs/qed_serialno\n");
                kobject_put(qedfs_kobj);
                return -ENOMEM;
        }
        return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit qedfs_exit(void) {
        pr_notice("qedfs-driver - Deleting /sys/kernel/qedfs/qed_serialno\n");
        sysfs_remove_file(qedfs_kobj, &qedfs_attr.attr);
        kobject_put(qedfs_kobj);
}

module_init(qedfs_init);
module_exit(qedfs_exit);

