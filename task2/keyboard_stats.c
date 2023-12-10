#include <linux/module.h>
#include <linux/keyboard.h>
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <linux/interrupt.h> 
#include <linux/timer.h>    

static struct timer_list my_timer;
static atomic_t count = ATOMIC_INIT(0);
static int my_irq = 1;

irqreturn_t irq_handler(int irq, void *dev_id) {
    atomic_inc_return(&count)
    return IRQ_HANDLED;
}

void my_timer_callback(struct timer_list *t) {
    printk(KERN_INFO "Characters typed in last minute: %d\n", atomic_xchg(&count, 0));
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(60000));
}

static int __init my_module_init(void) {
    printk(KERN_INFO "Module loaded\n");

    if (request_irq(my_irq, irq_handler, IRQF_SHARED, "my_keyboard_irq_handler", (void *)(irq_handler))) {
        printk(KERN_ERR "my_module: cannot register IRQ %d\n", my_irq);
        return -EIO;
    }

    timer_setup(&my_timer, my_timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(60000));

    return 0;
}

static void __exit my_module_cleanup(void) {
    printk(KERN_INFO "Module unloaded\n");
    free_irq(my_irq, (void *)(irq_handler));
    del_timer(&my_timer); 
}

module_init(my_module_init);
module_exit(my_module_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Artyom Kizilov");
MODULE_DESCRIPTION("Keyboard character counter module");
MODULE_VERSION("1.0");
