#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>       
#include <linux/cdev.h>     
#include <linux/device.h>   
#include <linux/uaccess.h>  
#include <linux/stat.h>     

#define DEVICE_NAME "my_ring_dev"
#define CLASS_NAME  "ring_class"
#define BUFFER_SIZE 64      // Маленький размер, чтобы сразу увидеть затирание

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;

// Кольцевой буфер и индексы
static unsigned char ring_buffer[BUFFER_SIZE];
static unsigned int head = 0;
static unsigned int tail = 0;

// Установка прав 0660 (Owner: RW, Group: RW)
static char *ring_devnode(struct device *dev, umode_t *mode) {
    if (mode) *mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    return NULL;
}

// ОТКРЫТИЕ
static int dev_open(struct inode *inodep, struct file *filep) {
    pr_info("Ring Dev: открыт. Head=%u, Tail=%u\n", head, tail);
    return 0;
}

// ЧТЕНИЕ (ИЗ ядра ПОЛЬЗОВАТЕЛЮ) - вычерпываем данные
static ssize_t dev_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    size_t bytes_read = 0;
    unsigned char c;

    // Читаем, пока пользователь просит (len) и пока хвост не догнал голову
    while (len > 0 && tail != head) {
        c = ring_buffer[tail];

        if (copy_to_user(buf + bytes_read, &c, 1)) {
            return -EFAULT;
        }

        tail = (tail + 1) % BUFFER_SIZE; // Двигаем хвост по кругу
        bytes_read++;
        len--;
    }

    return bytes_read; 
}

// ЗАПИСЬ (ОТ пользователя В ядро) - наполняем кольцо
static ssize_t dev_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    size_t i;
    unsigned char c;

    for (i = 0; i < len; i++) {
        if (copy_from_user(&c, buf + i, 1)) {
            return -EFAULT;
        }

        ring_buffer[head] = c;
        unsigned int next = (head + 1) % BUFFER_SIZE;

        // Если голова наткнулась на хвост — двигаем хвост (затираем старое)
        if (next == tail) {
            tail = (tail + 1) % BUFFER_SIZE;
        }
        
        head = next; // Двигаем голову вперед
    }

    pr_info("Ring Dev: записано %zu байт. Новая голова: %u\n", len, head);
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open  = dev_open,
    .read  = dev_read,
    .write = dev_write,
};

// Твой каскадный switch в init
static int __init my_init(void) {
    int step = 0;
    int ret = 0;

    // 1. Номер
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) goto error_handler;
    step = 1;

    // 2. cdev
    cdev_init(&my_cdev, &fops);
    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret < 0) goto error_handler;
    step = 2;

    // 3. Класс
    my_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(my_class)) {
        ret = PTR_ERR(my_class);
        goto error_handler;
    }
    my_class->devnode = ring_devnode; // Права 0660
    step = 3;

    // 4. Устройство
    if (IS_ERR(device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME))) {
        ret = -1;
        goto error_handler;
    }

    pr_info("Ring Dev: Взлетели! Файл /dev/%s готов.\n", DEVICE_NAME);
    return 0;

error_handler:
    switch (step) {
        case 3: class_destroy(my_class);
        case 2: cdev_del(&my_cdev);
        case 1: unregister_chrdev_region(dev_num, 1);
    }
    return ret;
}

static void __exit my_exit(void) {
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("Ring Dev: Выгружен. До связи!\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Master of 15-Year-Old Beast");
